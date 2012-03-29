#include "ZeeB/SoftElectron.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ITHistSvc.h"

//ROOT includes
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TMath.h"
#include "TVector3.h"


#include "McParticleEvent/TruthParticleContainer.h"
#include "Navigation/NavigationToken.h"
#include "egammaEvent/ElectronAssociation.h"
#include "egammaEvent/ElectronContainer.h"

#include "HepMC/GenEvent.h"

#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "Particle/TrackParticleContainer.h"

/// the Electron
#include "egammaEvent/egamma.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"


#include "CLHEP/Vector/LorentzVector.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "HepMC/SimpleVector.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "TruthUtils/IHforTool.h"

#include <map>
#include <algorithm>

SoftElectron::SoftElectron(const std::string& name, ISvcLocator* pSvcLocator) 
    : Algorithm(name, pSvcLocator),
    m_mcTruthContainer(0),
    m_eventInfo(0),
    m_mcEventCollection(0),
    m_vxContainer(0),
    m_electronCollection(0),
    m_histos(0),
    m_mcTruthClassifier("MCTruthClassifier")
{
  declareProperty("MCParticleContainer",m_truthParticleContainerName = "SpclMC");
  declareProperty("TrackParticleContainer", m_trackParticleContainerName = "TrackParticleCandidate");
  declareProperty("MuonContainer",m_muonContainerName = "StacoMuonCollection");
  declareProperty("ElectronContainer", m_electronContainerName = "ElectronAODCollection");
  declareProperty("PrimaryVertexContainer", m_primaryVertexContainerName = "VxPrimaryCandidate");
  declareProperty("MCEventContainer", m_mcEventContainerName = "GEN_AOD");
  declareProperty("HardElLowPtcut",m_hardElLowPtCut=25);
  declareProperty("SoftElLowPtcut",m_softElLowPtcut=2);
  declareProperty("SoftElHighPtcut",m_softElHighPtCut=25);
 
}

SoftElectron::~SoftElectron() {}

StatusCode SoftElectron::initialize() 
{
    MsgStream mlog( msgSvc(), name() );

    StatusCode sc = service("StoreGateSvc", m_storeGate);
    
    m_hfor_tool = ToolHandle<IHforTool>("HforTool/hforTool", this);
    if(m_hfor_tool.retrieve().isFailure()){
        mlog <<MSG::ERROR 
                << ": Failed to retrieve tool "
                <<endreq;
        return StatusCode::FAILURE;
    }
    if(m_mcTruthClassifier.retrieve().isFailure())
    {
        mlog <<MSG::FATAL
           << "Failed to retrieve Truth Classifier Tool"
           << endreq;
    }


    // THistSvc init:
    sc = service("THistSvc", m_histos);
    if ( sc.isFailure() )
    {
        mlog << MSG::FATAL << ">>> Unable to locate the Histogram service" << endreq;
    }
    
    // histograms:
    sc = this->BookHistograms();
    if( sc.isFailure())
    {
        mlog <<MSG::FATAL << "Could not register histograms" <<endreq;
    }
    return StatusCode::SUCCESS;
}		 

double phiCorr(double phi)
{
  double newphi = phi;
  if (phi<-M_PI) newphi += 2*M_PI;
  if (phi> M_PI) newphi -= 2*M_PI;
  return newphi;
}



StatusCode SoftElectron::execute() 
{
    MsgStream mlog( msgSvc(), name() );

    this->ClearContainers();
    mlog << MSG::INFO<< "SoftElectron::execute()" << endreq;
    StatusCode sc = StatusCode::SUCCESS;

    /** Testing HFOR 
    if ( m_hfor_tool->execute().isSuccess() )
    {
        std::string hfor_type = m_hfor_tool->getDecision();
    }
    **/
    
    /** Retrieve Event header: */
    sc = this->LoadContainers();
    if (sc.isFailure())
    {
        mlog << MSG::FATAL <<"Could not load containers" <<endreq;
        return StatusCode::FAILURE;
    }

    this->FindTruthParticle();
    this->DoElectronMatch();

    return sc;
}

StatusCode SoftElectron::finalize() 
{
  return StatusCode::SUCCESS;
}

StatusCode SoftElectron::BookHistograms() 
{
    MsgStream mlog( msgSvc(), name() );

    StatusCode sc ;

    mlog<<MSG::INFO<<"Booking histograms" <<endreq;

    m_h1Hists["h1_softElEta"]   = new TH1F("softElEta","AuthorSofte",100,-5,5);
    m_h1Hists["h1_softElPt"]    = new TH1F("softElPt","AuthorSofte;p_{T} [GeV]",100,0,100);
    m_h1Hists["h1_softElPhi"]   = new TH1F("softElPhi","AuthorSofte",80,-4,4);

    m_h1Hists["h1_nBHadrons"]   = new TH1F("nBHadrons","",4,0,4);
    m_h1Hists["h1_nBSemilept"]  = new TH1F("nBSemilept","",4,0,4);

    m_h1Hists["h1_softElEtaResol"]   = new TH1F("softElEtaResol","AuthorSofte",500,-1,1);
    m_h1Hists["h1_softElPtResol"]    = new TH1F("softElPtResol","AuthorSofte;p_{T} [GeV]",500,-1,1);
    m_h1Hists["h1_softElPhiResol"]   = new TH1F("softElPhiResol","AuthorSofte",500,-1,1);

    m_h1Hists["h1_hardElEta"]   = new TH1F("hardElEta","AuthorElectron",100,-5,5);
    m_h1Hists["h1_hardElPt"]    = new TH1F("hardElPt","AuthorElectron;p_{T} [GeV]",500,0,500);
    m_h1Hists["h1_hardElPhi"]   = new TH1F("hardElPhi","AuthorElectron",80,-4,4);

    m_h1Hists["h1_hardElEtaResol"]   = new TH1F("hardElEtaResol","AuthorElectron",500,-1,1);
    m_h1Hists["h1_hardElPtResol"]    = new TH1F("hardElPtResol","AuthorElectron;p_{T} [GeV]",500,-1,1);
    m_h1Hists["h1_hardElPhiResol"]   = new TH1F("hardElPhiResol","AuthorElectron",500,-1,1);

    m_h1Hists["h1_softElOrigin"]    = new TH1F("softElOrigin","Mother of  soft Electron",1,0,1);
    m_h1Hists["h1_hardElOrigin"]    = new TH1F("hardElOrigin","Mother of  hard Electron",1,0,1);
 
    m_h2Hists["h2_ElHardVsSoft"]    = new TH2F("ElHardVsSoft",";# Hard el; #Soft el",10,0,10,10,0,10);
    m_h2Hists["h2_ElHardVsSoftTrue"]= new TH2F("ElHardVsSoftTrue",";# Hard Z el; #soft B el",10,0,10,10,0,10);

    for(std::map<std::string,TH1F*>::iterator iter = m_h1Hists.begin(); iter != m_h1Hists.end(); ++iter)
    {
        iter->second->Sumw2();
        sc = m_histos->regHist("/AANT/"+iter->first, iter->second);
        if(sc.isFailure())
        {
            mlog<<MSG::FATAL<<"Could not register histogram"<<endreq;
            return StatusCode::FAILURE;
        }
    }

    for(std::map<std::string,TH2F*>::iterator iter = m_h2Hists.begin(); iter != m_h2Hists.end(); ++iter)
    {
        iter->second->Sumw2();
        sc = m_histos->regHist("/AANT/"+iter->first, iter->second);
        if(sc.isFailure())
        {
            mlog<<MSG::FATAL<<"Could not register histogram"<<endreq;
            return StatusCode::FAILURE;
        }
    }
    return StatusCode::SUCCESS;
}

bool SoftElectron::isBHadron(const HepMC::GenParticle* p) 
{
    int mpdg = abs(p->pdg_id());
    return (   ( 500 < mpdg && mpdg < 599 )   ||
            ( 10500 < mpdg && mpdg < 10599 ) ||
            (  5000 < mpdg && mpdg < 5999  ) ||
            ( 20500 < mpdg && mpdg < 20599 ) );
}

bool SoftElectron::isCHadron(const HepMC::GenParticle* p) 
{
    int mpdg = abs(p->pdg_id());
    return (   ( 400 < mpdg && mpdg < 499 )   || 
            ( 10400 < mpdg && mpdg < 10499 ) ||
            (  4000 < mpdg && mpdg < 4999  ) ||
            ( 20400 < mpdg && mpdg < 20499 ) );
}

void SoftElectron::FindTruthParticle()
{
    MsgStream mlog( msgSvc(), name() );

    const HepMC::GenEvent* GenEvent = *(m_mcEventCollection->begin());
    if(!GenEvent)
    {
        mlog<<MSG::DEBUG <<"Could not find GenEvent" <<endreq;
        return ;
    }
    HepMC::GenEvent::particle_const_iterator  pitr = GenEvent->particles_begin();
    for(; pitr !=  GenEvent->particles_end(); ++pitr)
    {
        const HepMC::GenParticle* part = (*pitr);
        if(part->status() == 200)
        {
            if(isBHadron(part)) 
            {
                mlog<<MSG::INFO <<"B Particle: "<<part->status() <<endreq;
                m_h1Hists["h1_nBHadrons"]->Fill(1);
            }
            if(isCHadron(part))
            {
            }
        }
    }
}

void SoftElectron::DoElectronMatch()
{
    ElectronContainer::const_iterator elItr = m_electronCollection->begin();
    MsgStream mlog(msgSvc(), name());

    std::pair<unsigned int, unsigned int> result;
    MCTruthPartClassifier::ParticleDef partDef;
    for(;elItr != m_electronCollection->end(); ++elItr)
    {
        Analysis::Electron* Electron = (*elItr);
        
        double el_pt   = Electron->hlv().perp()/1000;
        double el_eta  = Electron->hlv().eta();
        double el_phi  = Electron->hlv().phi();

        double el_cl_pt     = 0;
        double el_cl_eta    = 0;
        double el_cl_phi    = 0;
        const CaloCluster* ElCluster = Electron->cluster();
        if(ElCluster)
        {
            el_cl_pt     = ElCluster->pt()/1000;
            el_cl_eta    = ElCluster->eta();
            el_cl_phi    = ElCluster->phi();
        }

        double el_trk_pt    = -100;
        double el_trk_eta   = -100;
        double el_trk_phi   = -100;
        const Rec::TrackParticle* ElTrkPartcl = Electron->trackParticle();
        if(ElTrkPartcl)
        {
            el_trk_pt    = ElTrkPartcl->pt()/1000;
            el_trk_eta   = ElTrkPartcl->eta();
            el_trk_phi   = ElTrkPartcl->phi();
        }

        result = m_mcTruthClassifier->particleTruthClassifier(Electron);
        unsigned int iTypeOfPart = result.first;
        unsigned int iPartOrig   = result.second; 
        std::string partOrigin  = partDef.sParticleOrigin[iPartOrig];
        std::string partType    = partDef.sParticleType[iTypeOfPart];
 
        const HepMC::GenParticle* mother    = m_mcTruthClassifier->getMother();
 
        
        double el_truth_pt  = -100;
        double el_truth_eta = -100;
        double el_truth_phi = -100;

        const HepMC::GenParticle* particle  = m_mcTruthClassifier->getGenPart();
        if(particle)
        {
            el_truth_pt  = particle->momentum().perp()/1000;
            el_truth_eta = particle->momentum().eta();
            el_truth_phi = particle->momentum().phi();
        }
        
        //Soft Electrons
        if( (*elItr)->author(egammaParameters::AuthorSofte) && !(*elItr)->author(egammaParameters::AuthorElectron) && el_truth_pt < m_softElHighPtCut && el_truth_pt > m_softElLowPtcut)
        {
            if((std::abs(el_trk_eta) >2.5 ))
                continue;

            m_h1Hists["h1_softElPt"]->Fill(el_trk_pt);
            m_h1Hists["h1_softElPtResol"]->Fill(el_trk_pt - el_truth_pt);

            m_h1Hists["h1_softElEta"]->Fill(el_trk_eta);
            m_h1Hists["h1_softElEtaResol"]->Fill(el_trk_eta - el_truth_eta);

            m_h1Hists["h1_softElPhi"]->Fill(el_trk_phi);
            m_h1Hists["h1_softElPhiResol"]->Fill(el_trk_phi - el_truth_phi);

            m_h1Hists["h1_softElOrigin"]->Fill(partOrigin.c_str(),1);

            m_nSoftEl++;
            if(partOrigin == "ZBoson")
            {
                m_nSoftZEl++;
            }
            else if(partOrigin == "BottomMeson")
            {
                m_nSoftBEl++;
                m_h1Hists["h1_nBSemilept"]->Fill(1);
            }
            else if(partOrigin =="CharmedMeson")
            {
                HepMC::GenVertex *pvtx = mother->production_vertex();
                if(pvtx)
                {
                    HepMC::GenVertex::particle_iterator itr = pvtx->particles_begin(HepMC::parents);
                    bool parentBHadron(false);
                    for(; itr != pvtx->particles_end(HepMC::parents); ++itr)
                    {
                        HepMC::GenParticle* gp = (*itr);
                        if(isBHadron(gp))
                        {
                           parentBHadron = true;
                        }
                    }
                    if(!parentBHadron)
                    {
                        m_nSoftBEl++;
                    }
                }
            }
        }
        //Hard Electrons
        else if((*elItr)->author(egammaParameters::AuthorElectron) && el_truth_pt> m_hardElLowPtCut)
        {
            if(!(std::abs(el_cl_eta) <2.47  || 1.37 < std::abs(el_cl_eta) || std::abs(el_cl_eta) < 1.52))
                continue;

            m_h1Hists["h1_hardElPt"]->Fill(el_cl_pt);
            m_h1Hists["h1_hardElPtResol"]->Fill(el_cl_pt - el_truth_pt);

            m_h1Hists["h1_hardElEta"]->Fill(el_cl_eta);
            m_h1Hists["h1_hardElEtaResol"]->Fill(el_cl_eta - el_truth_eta);

            m_h1Hists["h1_hardElPhi"]->Fill(el_cl_phi);
            m_h1Hists["h1_hardElPhiResol"]->Fill(el_cl_phi - el_truth_phi);

            m_h1Hists["h1_hardElOrigin"]->Fill(partOrigin.c_str(),1);

            m_nHardEl++;
            if(partOrigin == "ZBoson")
            {
                m_nHardZEl++;
            }
            else if(partOrigin == "BottomMeson")
            {
                m_nHardBEl++;
                m_h1Hists["h1_nBSemilept"]->Fill(1);
            }
            else if(partOrigin == "CharmedMeson")
            {
                HepMC::GenVertex *pvtx = mother->production_vertex();
                if(pvtx)
                {
                    HepMC::GenVertex::particle_iterator itr = pvtx->particles_begin(HepMC::parents);
                    bool parentBHadron(false);
                    for(; itr != pvtx->particles_end(HepMC::parents); ++itr)
                    {
                        HepMC::GenParticle* gp = (*itr);
                        if(isBHadron(gp))
                        {
                           parentBHadron = true;
                        }
                    }
                    if(!parentBHadron)
                    {
                        m_nHardBEl++;
                    }
                }
            }
        }
    }
    m_h2Hists["h2_ElHardVsSoft"]    -> Fill(m_nHardEl,m_nSoftEl);
    m_h2Hists["h2_ElHardVsSoftTrue"]-> Fill(m_nHardZEl,m_nSoftBEl);
}

StatusCode SoftElectron::LoadContainers()
{
    MsgStream mlog( msgSvc(), name() );

    StatusCode sc ;
    
    sc = m_storeGate->retrieve(m_eventInfo);
    if(sc.isFailure())
    {
        mlog << MSG::ERROR <<"Could not retireve EventInfo"<< endreq;
        return sc;
    }
   
    sc = m_storeGate->retrieve(m_vxContainer, m_primaryVertexContainerName);
    if(sc.isFailure())
    {
        mlog << MSG::ERROR <<"could not retrive PrimaryVertex contaienr" <<endreq;
        return sc;
    }
 
    sc = m_storeGate->retrieve(m_mcEventCollection, m_mcEventContainerName);
    if(sc.isFailure())
    {
        mlog << MSG::ERROR <<"could no retrive MC EventCollection" <<endreq;
        return sc;
    }
 
    sc=m_storeGate->retrieve(m_mcTruthContainer, m_truthParticleContainerName);
    if(sc.isFailure())
    {
        mlog << MSG::ERROR << "could not retrive TruthParticlecontainer" <<endreq;
        return sc;
    }
    
    sc = m_storeGate->retrieve( m_electronCollection,m_electronContainerName);
    if(sc.isFailure())
    {
        mlog << MSG::ERROR <<"could not retrieve ElectronContainer" <<endreq;
        return sc;
    }

    return sc;
}

void SoftElectron::ClearContainers()
{
    m_nHardEl  = 0;
    m_nSoftEl  = 0;
    
    m_nHardZEl = 0;
    m_nSoftZEl = 0;
    
    m_nHardBEl = 0;
    m_nSoftBEl = 0;

}
