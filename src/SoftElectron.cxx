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

bool MomentumCompare(const HepMC::GenParticle* i, const HepMC::GenParticle* j)
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
    m_tree(0),
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
    m_h1Hists["MatchEffVsPt"]->Divide(m_h1Hists["MatchedElPt"],m_h1Hists["RecoElPt"]);

  return StatusCode::SUCCESS;
}

StatusCode SoftElectron::BookHistograms() 
{
    MsgStream mlog( msgSvc(), name() );

    StatusCode sc ;

    mlog<<MSG::INFO<<"Booking histograms" <<endreq;

    /**
     * TTree
     */
    m_tree  = new TTree("el","Electron Tree");
    m_tree->Branch("elPt",m_elPtBr);
    m_tree->Branch("elEta",m_elEtaBr);
    m_tree->Branch("elPhi",m_elPhiBr);
    m_tree->Branch("elIsMtchd",m_elMtchd);
    m_tree->Branch("elIsBMtchd",m_BMtchd);
    m_tree->Branch("elIsCMtchd",m_CMtchd);

    m_h1Hists["softElEta"]   = new TH1F("softElEta","AuthorSofte",100,-5,5);
    m_h1Hists["softElPt"]    = new TH1F("softElPt","AuthorSofte;p_{T} [GeV]",100,0,100);
    m_h1Hists["softElPhi"]   = new TH1F("softElPhi","AuthorSofte",80,-4,4);

    m_h1Hists["nBHadrons"]   = new TH1F("nBHadrons","",4,0,4);
    m_h1Hists["nBSemilept"]  = new TH1F("nBSemilept","",4,0,4);
    m_h1Hists["nBSemileptTrue"] = new TH1F("nBSemileptTrue","",4,0,4);


    m_h1Hists["nCHadrons"]   = new TH1F("nCHadrons","",4,0,4);
    m_h1Hists["nCSemilept"]  = new TH1F("nCSemilept","",4,0,4);
    m_h1Hists["nCSemileptTrue"] = new TH1F("nCSemileptTrue","",4,0,4);

    m_h1Hists["BMultplcty"]  = new TH1F("BMultplcty","B Hadron  multiplicity",10,0,10);
    m_h1Hists["CMultplcty"]  = new TH1F("CMultplcty","C Hadron multiplicity",10,0,10);
    

    //TO be deleted
    m_h1Hists["BStatus"]     = new TH1F("BStatus","Status of semilept B hadron",100,150,250);
    m_h1Hists["BLeadingPt"]  = new TH1F("BLeadingPt","Leading B Hadron Pt",500,0,500);
    m_h1Hists["BSubLeadingPt"]=new TH1F("BSubLeadingPt","Subleading B hadron Pt",500,0,500);
    m_h1Hists["BSubSubLeadingPt"] = new TH1F("BSubSubLeadingPt","SubSubleading B hadron",500,0,500);
    m_h1Hists["CStatus"]     = new TH1F("CStatus","Status of semilept C hadron",100,150,250);
    m_h1Hists["CLeadingPt"]  = new TH1F("CLeadingPt","Leading C Hadron Pt",500,0,500);
    m_h1Hists["CSubLeadingPt"]=new TH1F("CSubLeadingPt","Subleading C hadron Pt",500,0,500);
    m_h1Hists["CSubSubLeadingPt"] = new TH1F("CSubSubLeadingPt","SubSubleading C hadron",500,0,500);


    m_h1Hists["hardEl_cntr"]        = new TH1F("hardEl_cntr","Number of Hard Electrons after cuts",4,0,4);
    m_h1Hists["hardElMatched_cntr"] = new TH1F("hardElMatched_cntr","Number of Hard Electrons truth matched",4,0,4);

    m_h1Hists["el_cntr"]        = new TH1F("el_cntr","Electron Counter",4,0,4);
    m_h1Hists["elMatched_cntr"] = new TH1F("elMatched_cntr","Truth matched Electron Counter",4,0,4);
    
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

    //Reconstruction
    m_h1Hists["RecoElPt"]       = new TH1F("RecoElPt","Reco Electron Pt [GeV]",500,0,500);
    m_h1Hists["MatchedElPt"]    = new TH1F("MatchedElPt","Truth Matched Electron Pt(Reco); [GeV]",500,0,500);
    m_h1Hists["MatchedTruthElPt"]= new TH1F("MatchedTruthElPt","Truth Matched Electron Pt( Gen); [GeV]",500,0,500);
    m_h1Hists["MatchEffVsPt"]   = new TH1F("MatchEffVsPt","Truth Match EffVsPt;[GeV]",500,0,500);

    m_h2Hists["ElERatio"]       = new TH2F("ElERatio","Electron Energy ratio; #eta; E_reco/E_gen",-5,5,10,500,0,500);
    m_h2Hists["ElPRatio"]       = new TH2F("ElPRatio","Electron momentum Ratio; #eta; p_reco/p_gen",-5,5,10,500,0,500);
    m_h2Hists["ElHardVsSoft"]    = new TH2F("ElHardVsSoft",";# Hard el; #Soft el",10,0,10,10,0,10);
    m_h2Hists["ElHardVsSoftTrue"]= new TH2F("ElHardVsSoftTrue",";# Hard Z el; #soft B el",10,0,10,10,0,10);

    for(std::map<std::string,TH1F*>::iterator iter = m_h1Hists.begin(); iter != m_h1Hists.end(); ++iter)
    {
        iter->second->Sumw2();
        sc = m_histos->regHist("/AANT/Hist/"+iter->first, iter->second);
        if(sc.isFailure())
        {
            mlog<<MSG::FATAL<<"Could not register histogram"<<endreq;
            return StatusCode::FAILURE;
        }
    }

    for(std::map<std::string,TH2F*>::iterator iter = m_h2Hists.begin(); iter != m_h2Hists.end(); ++iter)
    {
        iter->second->Sumw2();
        sc = m_histos->regHist("/AANT/Hist/"+iter->first, iter->second);
        if(sc.isFailure())
        {
            mlog<<MSG::FATAL<<"Could not register histogram"<<endreq;
            return StatusCode::FAILURE;
        }
    }
    //Register TTree
    m_histos->regTree("/AANT/Tree",m_tree);
    return StatusCode::SUCCESS;
}

bool SoftElectron::isBHadron(const HepMC::GenParticle* p) 
{
    int mpdg = abs(p->pdg_id());
    return (mpdg ==511 || mpdg == 521 || mpdg == 531);
    
    /*return (   ( 500 < mpdg && mpdg < 599 )   ||
            ( 10500 < mpdg && mpdg < 10599 ) ||
            (  5000 < mpdg && mpdg < 5999  ) ||
            ( 20500 < mpdg && mpdg < 20599 ) );*/
}

bool SoftElectron::isCHadron(const HepMC::GenParticle* p) 
{
    int mpdg = abs(p->pdg_id());
    return (mpdg == 411 || mpdg == 421 || mpdg == 431);

    /*return (   ( 400 < mpdg && mpdg < 499 )   || 
            ( 10400 < mpdg && mpdg < 10499 ) ||
            (  4000 < mpdg && mpdg < 4999  ) ||
            ( 20400 < mpdg && mpdg < 20499 ) );*/
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
        if(part->momentum().perp()/1000 > 2)
        {
            if(isBHadron(part)) 
            {
                if(part->status()==196 || part->status()==197 || part->status()==198 || part->status() ==199 || part->status()==200)
                {
                    m_h1Hists["nBHadrons"]->Fill(1);
                    
                    bool hasDaughterEl(false);
                    std::vector<const HepMC::GenParticle*> children = this->GetChildren(part);
                    for(std::vector<const HepMC::GenParticle*>::iterator Iter = children.begin(); Iter != children.end(); ++Iter)
                    {
                        if(std::abs((*Iter)->pdg_id()) ==11)
                        {
                            hasDaughterEl = true;
                        }
                    }
                    if(hasDaughterEl)
                    {
                        m_h1Hists["nBSemileptTrue"]->Fill(1);
                    }
                }
            }
            if(isCHadron(part))
            {
                if(part->status()==196 || part->status()==197 || part->status()==198 || part->status() ==199 || part->status()==200)
                {
                    m_h1Hists["nCHadrons"]->Fill(1);
                    
                    bool hasDaughterEl(false);
                    std::vector<const HepMC::GenParticle*> children = this->GetChildren(part);
                    for(std::vector<const HepMC::GenParticle*>::iterator Iter = children.begin(); Iter != children.end(); ++Iter)
                    {
                        if(std::abs((*Iter)->pdg_id()) ==11)
                        {
                            hasDaughterEl = true;
                        }
                    }
                    if(hasDaughterEl)
                    {
                        m_h1Hists["nCSemileptTrue"]->Fill(1);
                    }
                }
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
        HepMC::GenParticle* elParent = this->GetElectronParent(Electron);

        double el_truth_pt  = -100;
        double el_truth_eta = -100;
        double el_truth_phi = -100;
        if(elParent)
        {
            el_truth_pt     = elParent->momentum().perp()/1000;
            el_truth_eta    = elParent->momentum().eta();
            el_truth_phi    = elParent->momentum().phi();
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
        if( ((*elItr)->author(egammaParameters::AuthorSofte) 
                    || ( (*elItr)->author(egammaParameters::AuthorSofte) &&(*elItr)->author(egammaParameters::AuthorElectron)))
                && el_trk_pt< m_softElHighPtCut && el_trk_pt> m_softElLowPtcut)
        {

            if(!(std::abs(el_trk_eta) <m_etaMax))
                continue;
 
            if(elParent)
            {
                m_h1Hists["softElPt"]->Fill(el_trk_pt);
                m_h1Hists["softElPtResol"]->Fill(el_trk_pt - el_truth_pt);

                m_h1Hists["softElEta"]->Fill(el_trk_eta);
                m_h1Hists["softElEtaResol"]->Fill(el_trk_eta - el_truth_eta);

                m_h1Hists["softElPhi"]->Fill(el_trk_phi);
                m_h1Hists["softElPhiResol"]->Fill(el_trk_phi - el_truth_phi);

                this->FillCounters("soft",elParent);
            }

        }
        //Hard Electrons
        else if((*elItr)->author(egammaParameters::AuthorElectron) && el_cl_pt> m_hardElLowPtCut)
        {
            if( !(std::abs(el_cl_eta) < m_etaMax && (std::abs(el_cl_eta) < m_crackEtaMin || std::abs(el_cl_eta)> m_crackEtaMax) ) )
                continue;

            if(elParent)
            {
                m_h1Hists["hardElPt"]->Fill(el_cl_pt);
                m_h1Hists["hardElPtResol"]->Fill(el_cl_pt - el_truth_pt);

                m_h1Hists["hardElEta"]->Fill(el_cl_eta);
                m_h1Hists["hardElEtaResol"]->Fill(el_cl_eta - el_truth_eta);

                m_h1Hists["hardElPhi"]->Fill(el_cl_phi);
                m_h1Hists["hardElPhiResol"]->Fill(el_cl_phi - el_truth_phi);

                this->FillCounters("hard",elParent);
            }

        }
    }
}

void SoftElectron::FillCounters(std::string type,HepMC::GenParticle* elParent)
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

    if(elParent)
    {

        m_h1Hists[ElMatchedCntrKey]->Fill(1);
        m_h1Hists[ElOrignKey]->Fill(elParent->pdg_id());

        ElCntr ++;
        if(elParent->pdg_id() ==23)
        {
            m_nZEl++;
            ZElCntr++;
        }
        else if(this->isBHadron(elParent))
        {
            m_nBEl++;
            BElCntr++;
        }
        else if(this->isCHadron(elParent))
        {
            HepMC::GenVertex *pvtx = elParent->production_vertex();
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

    m_TruthBvec.clear();
    m_TruthCvec.clear();

}

void SoftElectron::FillHistograms()
{
    m_h1Hists["BMultplcty"]->Fill(m_nBHadrons);
    m_h1Hists["CMultplcty"]->Fill(m_nCHadrons);

    m_h2Hists["ElHardVsSoft"]    -> Fill(m_nHardEl,m_nSoftEl);
    m_h2Hists["ElHardVsSoftTrue"]-> Fill(m_nHardZEl,m_nSoftBEl);

    //Leading, sub leading and subsubleading
    sort(m_TruthBvec.begin(), m_TruthBvec.end(),MomentumCompare);
    sort(m_TruthCvec.begin(), m_TruthCvec.end(),MomentumCompare);

    
    if(m_TruthBvec.size()>2)
    {
        m_h1Hists["BLeadingPt"]->Fill(m_TruthBvec.at(2)->momentum().perp()/1000);
        m_h1Hists["BSubLeadingPt"]->Fill(m_TruthBvec.at(1)->momentum().perp()/1000);
        m_h1Hists["BSubSubLeadingPt"]->Fill(m_TruthBvec.at(0)->momentum().perp()/1000);
    }
    if(m_TruthCvec.size()>2)
    {
        m_h1Hists["CLeadingPt"]->Fill(m_TruthCvec.at(2)->momentum().perp()/1000);
        m_h1Hists["CSubLeadingPt"]->Fill(m_TruthCvec.at(1)->momentum().perp()/1000);
        m_h1Hists["CSubSubLeadingPt"]->Fill(m_TruthCvec.at(0)->momentum().perp()/1000);
    }
}

HepMC::GenParticle* SoftElectron::GetElectronParent(Analysis::Electron* Electron)
{
    m_h1Hists["el_cntr"]->Fill(1);
    
    const CaloCluster* ElCluster = Electron->cluster();
    m_h1Hists["RecoElPt"]->Fill(ElCluster->pt()/1000);
 
    HepMC::GenParticle* elParent = 0;

    m_mcTruthClassifier->particleTruthClassifier(Electron);
    const HepMC::GenParticle* particle  = m_mcTruthClassifier->getGenPart();
    if(particle)
    {
        
        //Account for the Reco Electron Pt in corresponding histograms
        m_h1Hists["MatchedElPt"]->Fill(Electron->cluster()->pt()/1000);
        m_h1Hists["MatchedTruthElPt"]->Fill(particle->momentum().perp()/1000);

        m_h2Hists["ElERatio"]->Fill(particle->momentum().eta(),ElCluster->e()/particle->momentum().e());
        m_h2Hists["ElPRatio"]->Fill(particle->momentum().eta(),ElCluster->pt()/particle->momentum().perp());

        HerwigTruthClassifier myTruthClassifier(particle);
        elParent= myTruthClassifier.GetParent();
        if(elParent)
        {
            m_h1Hists["elMatched_cntr"]->Fill(1);
            if(this->isBHadron(elParent))
            {
                m_h1Hists["nBSemilept"]->Fill(1);
                m_h1Hists["BStatus"]->Fill(elParent->status());
            }
            else if(this->isCHadron(elParent))
            {
                m_h1Hists["nCSemilept"]->Fill(1);
                m_h1Hists["CStatus"]->Fill(elParent->status());
            }
        }
    }
    return elParent;
}

std::vector<const HepMC::GenParticle* > SoftElectron::GetParents(const HepMC::GenParticle* p)
{
    HepMC::GenVertex* pvtx = p->production_vertex();
    std::vector<const HepMC::GenParticle*> parentVec;
    if(pvtx)
    {
        HepMC::GenVertex::particle_iterator pin = pvtx->particles_begin(HepMC::parents);
        for(; pin != pvtx->particles_end(HepMC::parents); ++pin)
        {
            const HepMC::GenParticle* parent = (*pin);
            parentVec.push_back(parent);
        }
    }
    return parentVec;
}

std::vector<const HepMC::GenParticle*> SoftElectron::GetChildren(const HepMC::GenParticle* p)
{
    std::vector<const HepMC::GenParticle*> daughterVec;

    HepMC::GenVertex* evtx = p->end_vertex();
    if (evtx)
    {
        HepMC::GenVertex::particle_iterator pout = evtx->particles_begin(HepMC::children);
        for(; pout != evtx->particles_end(HepMC::children); ++pout)
        {
            const HepMC::GenParticle* daughter = (*pout);
            if(daughter->pdg_id() == p->pdg_id())
            {
                return this->GetChildren(daughter);
            }
            else
            {
                daughterVec.push_back(daughter);
            }
        }
    }
    return daughterVec;
}

bool SoftElectron::IsIn(const HepMC::GenParticle* part , std::vector<const HepMC::GenParticle*> container)
{
    bool matched(false);
    for(std::vector<const HepMC::GenParticle*>::iterator iter = container.begin(); iter != container.end(); ++iter)
    {
        if ((*(*iter)) == (*part))
            matched = true;
    }
    return matched;
}
