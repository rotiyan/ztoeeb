#include "ZeeB/SoftElectron.h"
#include "ZeeB/HerwigTruthClassifier.h"

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

#include <iostream>
#include <algorithm>

bool MomentumCompare(const HepMC::GenParticle* i,const HepMC::GenParticle* j)
{
    return (i->momentum().perp() < j->momentum().perp());
}

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
  declareProperty("EtaCut",m_etaMax=2.47);
  declareProperty("CrackEtaMin",m_crackEtaMin=1.37);
  declareProperty("CrackEtaMax",m_crackEtaMax=1.52);
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

    this->ClearCounters();

    this->FindTruthParticle();
    this->DoElectronMatch();

    this->FillHistograms();


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

    m_h1Hists["softElEta"]   = new TH1F("softElEta","AuthorSofte",100,-5,5);
    m_h1Hists["softElPt"]    = new TH1F("softElPt","AuthorSofte;p_{T} [GeV]",100,0,100);
    m_h1Hists["softElPhi"]   = new TH1F("softElPhi","AuthorSofte",80,-4,4);

    m_h1Hists["nBHadrons"]   = new TH1F("nBHadrons","",4,0,4);
    m_h1Hists["nBSemilept"]  = new TH1F("nBSemilept","",4,0,4);

    m_h1Hists["nCHadrons"]   = new TH1F("nCHadrons","",4,0,4);
    m_h1Hists["nCSemilept"]  = new TH1F("nCSemilept","",4,0,4);

    m_h1Hists["BMultplcty"]  = new TH1F("BMultplcty","B Hadron  multiplicity",10,0,10);
    m_h1Hists["CMultplcty"]  = new TH1F("CMultplcty","C Hadron multiplicity",10,0,10);
    

    //TO be deleted
    m_h1Hists["BStatus"]     = new TH1F("BStatus","Status of semilept B hadron",100,150,250);
    m_h1Hists["CStatus"]     = new TH1F("CStatus","Status of semilept C hadron",100,150,250);
    m_h1Hists["BLeadingPt"]  = new TH1F("BLeadingPt","Leading B Hadron Pt",500,0,500);
    m_h1Hists["BSubLeadingPt"]=new TH1F("BSubLeadingPt","Subleading B hadron Pt",500,0,500);
    m_h1Hists["BSubSubLeadingPt"] = new TH1F("BSubSubLeadingPt","SubSubleading B hadron",500,0,500);

    m_h1Hists["hardEl_cntr"]        = new TH1F("hardEl_cntr","Number of Hard Electrons after cuts",4,0,4);
    m_h1Hists["hardElMatched_cntr"] = new TH1F("hardElMatched_cntr","Number of Hard Electrons truth matched",4,0,4);
    
    m_h1Hists["softEl_cntr"]        = new TH1F("softEl_cntr","Number of Soft Electrons after cuts",4,0,4);
    m_h1Hists["softElMatched_cntr"] = new TH1F("softElMatched_cntr","Number of Soft Electrons truth matched",4,0,4);


    m_h1Hists["softElEtaResol"]   = new TH1F("softElEtaResol","AuthorSofte",500,-1,1);
    m_h1Hists["softElPtResol"]    = new TH1F("softElPtResol","AuthorSofte;p_{T} [GeV]",500,-1,1);
    m_h1Hists["softElPhiResol"]   = new TH1F("softElPhiResol","AuthorSofte",500,-1,1);

    m_h1Hists["hardElEta"]   = new TH1F("hardElEta","AuthorElectron",100,-5,5);
    m_h1Hists["hardElPt"]    = new TH1F("hardElPt","AuthorElectron;p_{T} [GeV]",500,0,500);
    m_h1Hists["hardElPhi"]   = new TH1F("hardElPhi","AuthorElectron",80,-4,4);

    m_h1Hists["hardElEtaResol"]   = new TH1F("hardElEtaResol","AuthorElectron",500,-1,1);
    m_h1Hists["hardElPtResol"]    = new TH1F("hardElPtResol","AuthorElectron;p_{T} [GeV]",500,-1,1);
    m_h1Hists["hardElPhiResol"]   = new TH1F("hardElPhiResol","AuthorElectron",500,-1,1);

    m_h1Hists["softElOrigin"]    = new TH1F("softElOrigin","Mother of  soft Electron",1000,0,1000);
    m_h1Hists["hardElOrigin"]    = new TH1F("hardElOrigin","Mother of  hard Electron",1000,0,1000);
 
    m_h2Hists["ElHardVsSoft"]    = new TH2F("ElHardVsSoft",";# Hard el; #Soft el",10,0,10,10,0,10);
    m_h2Hists["ElHardVsSoftTrue"]= new TH2F("ElHardVsSoftTrue",";# Hard Z el; #soft B el",10,0,10,10,0,10);

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
        if(std::abs(part->momentum().eta()) > m_etaMax)
            continue ;
        int status = part->status();
        if(status == 196 || status == 197 || status ==198)
        {
            if(isBHadron(part)) 
            {
                m_BHadronContainer.push_back(part);
                mlog<<MSG::INFO <<"B Particle: "<<part->status() <<endreq;
                m_h1Hists["nBHadrons"]->Fill(1);

                m_nBHadrons++;
            }
            if(isCHadron(part))
            {
                m_CHadronContainer.push_back(part);
                m_h1Hists["nCHadrons"]->Fill(1);
                m_nCHadrons++;
            }
        }
    }
}

void SoftElectron::DoElectronMatch()
{
    ElectronContainer::const_iterator elItr = m_electronCollection->begin();
    MsgStream mlog(msgSvc(), name());

    MCTruthPartClassifier::ParticleDef partDef;
    for(;elItr != m_electronCollection->end(); ++elItr)
    {
        Analysis::Electron* Electron = (*elItr);
        HepMC::GenParticle* myParent = this->GetElectronParent(Electron);

        double el_truth_pt  = -100;
        double el_truth_eta = -100;
        double el_truth_phi = -100;
        if(myParent)
        {
            el_truth_pt     = myParent->momentum().perp()/1000;
            el_truth_eta    = myParent->momentum().eta();
            el_truth_phi    = myParent->momentum().phi();
        }
        
        //Track Particle 
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

        //Cluster
        double el_cl_pt     = -100;
        double el_cl_eta    = -100;
        double el_cl_phi    = -100;

        const CaloCluster* ElCluster = Electron->cluster();
        if(ElCluster)
        {
            el_cl_pt     = ElCluster->pt()/1000;
            el_cl_eta    = ElCluster->eta();
            el_cl_phi    = ElCluster->phi();
        }


        //Soft Electrons
        if( (*elItr)->author(egammaParameters::AuthorSofte) && el_trk_pt< m_softElHighPtCut && el_trk_pt> m_softElLowPtcut)
        {

            if(!(std::abs(el_trk_eta) <m_etaMax))
                continue;
 
            mlog <<MSG::INFO<< "IN SOFT ELECTRON " <<endreq;

            if(myParent)
            {
                m_h1Hists["softElPt"]->Fill(el_trk_pt);
                m_h1Hists["softElPtResol"]->Fill(el_trk_pt - el_truth_pt);

                m_h1Hists["softElEta"]->Fill(el_trk_eta);
                m_h1Hists["softElEtaResol"]->Fill(el_trk_eta - el_truth_eta);

                m_h1Hists["softElPhi"]->Fill(el_trk_phi);
                m_h1Hists["softElPhiResol"]->Fill(el_trk_phi - el_truth_phi);

                this->FillCounters("soft",myParent);
            }

        }
        //Hard Electrons
        else if((*elItr)->author(egammaParameters::AuthorElectron) && el_cl_pt> m_hardElLowPtCut)
        {
            if( !(std::abs(el_cl_eta) < m_etaMax && !(std::abs(el_cl_eta)> m_crackEtaMin && std::abs(el_cl_eta))< m_crackEtaMax)) 
                continue;
            mlog <<MSG::INFO<< "IN HARD ELECTRON " <<endreq;

            if(myParent)
            {
                m_h1Hists["hardElPt"]->Fill(el_cl_pt);
                m_h1Hists["hardElPtResol"]->Fill(el_cl_pt - el_truth_pt);

                m_h1Hists["hardElEta"]->Fill(el_cl_eta);
                m_h1Hists["hardElEtaResol"]->Fill(el_cl_eta - el_truth_eta);

                m_h1Hists["hardElPhi"]->Fill(el_cl_phi);
                m_h1Hists["hardElPhiResol"]->Fill(el_cl_phi - el_truth_phi);

                this->FillCounters("hard",myParent);
            }

        }
    }
}

void SoftElectron::FillCounters(std::string type,HepMC::GenParticle* myParent)
{
    int ElCntr  = 0;
    int ZElCntr = 0;
    int BElCntr = 0;
    int CElcntr = 0;

    std::string ElCntrKey       = type + "El_cntr";
    std::string ElMatchedCntrKey= type + "ElMatched_cntr";
    std::string ElOrignKey      = type + "ElOrigin";
    
    
    ElCntr ++;
    m_h1Hists[ElCntrKey]->Fill(1);

    if(myParent)
    {

        m_h1Hists[ElMatchedCntrKey]->Fill(1);
        m_h1Hists[ElOrignKey]->Fill(myParent->pdg_id());

        ElCntr ++;
        if(myParent->pdg_id() ==23)
        {
            m_nZEl++;
            ZElCntr++;
        }
        else if(this->isBHadron(myParent))
        {
            m_nBEl++;
            BElCntr++;
            m_h1Hists["nBSemilept"]->Fill(1);
        }
        else if(this->isCHadron(myParent))
        {
            HepMC::GenVertex *pvtx = myParent->production_vertex();
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
                if(parentBHadron)
                {
                    //Do nothing for the moment. 
                    //We are not lookin for the cascaded decay  yet
                }
            }
        }
    }

    //Assign the counters to hard and soft respectively
    if(type =="hard")
    {
        m_nHardEl   += ElCntr;
        m_nHardZEl  += ZElCntr;
        m_nHardBEl  += BElCntr;
    }
    else if(type =="soft")
    {
        m_nSoftEl   += ElCntr;
        m_nSoftZEl  += ZElCntr;
        m_nHardBEl  += BElCntr;
    }
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

void SoftElectron::ClearCounters()
{
    m_nBHadrons= 0;
    m_nCHadrons= 0;

    m_nBEl     = 0;
    m_nZEl     = 0;

    m_nHardEl  = 0;
    m_nSoftEl  = 0;
    
    m_nHardZEl = 0;
    m_nSoftZEl = 0;
    
    m_nHardBEl = 0;
    m_nSoftBEl = 0;

    m_BHadronContainer.clear();
    m_CHadronContainer.clear();

}

void SoftElectron::FillHistograms()
{
    m_h1Hists["BMultplcty"]->Fill(m_nBHadrons);
    m_h1Hists["CMultplcty"]->Fill(m_nCHadrons);

    m_h2Hists["ElHardVsSoft"]    -> Fill(m_nHardEl,m_nSoftEl);
    m_h2Hists["ElHardVsSoftTrue"]-> Fill(m_nHardZEl,m_nSoftBEl);

    //Leading and sub leading and subsubleading
    sort(m_BHadronContainer.begin(), m_BHadronContainer.end(),MomentumCompare);
    if(m_BHadronContainer.size()>2)
    {
        m_h1Hists["BLeadingPt"]->Fill(m_BHadronContainer.at(2)->momentum().perp()/1000);
        m_h1Hists["BSubLeadingPt"]->Fill(m_BHadronContainer.at(1)->momentum().perp()/1000);
        m_h1Hists["BSubSubLeadingPt"]->Fill(m_BHadronContainer.at(0)->momentum().perp()/1000);
    }
}

HepMC::GenParticle* SoftElectron::GetElectronParent(Analysis::Electron* Electron)
{
    m_mcTruthClassifier->particleTruthClassifier(Electron);
    const HepMC::GenParticle* particle  = m_mcTruthClassifier->getGenPart();


    HepMC::GenParticle* myParent = 0;
    if(particle)
    {
        HerwigTruthClassifier myTruthClassifier(particle);
        myParent= myTruthClassifier.GetParent();
        if(myParent)
        {
            if(this->isBHadron(myParent))
            {
                m_h1Hists["nBSemilept"]->Fill(1);
                m_h1Hists["BStatus"]->Fill(myParent->status());
            }
            else if(this->isCHadron(myParent))
            {
                m_h1Hists["nCSemilept"]->Fill(1);
                m_h1Hists["CStatus"]->Fill(myParent->status());
            }
        }
    }
    return myParent;
}
