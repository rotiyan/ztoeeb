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
    m_tree->Fill();
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

    /**
     * TTree
     */
    m_tree  = new TTree("el","Electron Tree");
    m_tree->Branch("el_trk_Pt",&m_el_trk_PtBr);
    m_tree->Branch("el_trk_Eta",&m_el_trk_EtaBr);
    m_tree->Branch("el_trk_Phi",&m_el_trk_PhiBr);
    m_tree->Branch("el_cl_Pt",&m_el_cl_PtBr);
    m_tree->Branch("el_cl_Eta",&m_el_cl_EtaBr);
    m_tree->Branch("el_cl_Phi",&m_el_cl_PhiBr);
    m_tree->Branch("el_truth_Pt",&m_el_truth_PtBr);
    m_tree->Branch("el_truth_Eta",&m_el_truth_EtaBr);
    m_tree->Branch("el_truth_Phi",&m_el_truth_PhiBr);
    m_tree->Branch("elIsMtchd",&m_elMtchd);
    m_tree->Branch("mtchdPrnt",&m_mtchdParent);
    m_tree->Branch("elAuthor",&m_elAuthor);
    m_tree->Branch("elAuthorSofte",&m_elAuthorSofte);
    m_tree->Branch("BPDG",&m_BPDG);
    m_tree->Branch("Bstatus",&m_BStatus);
    m_tree->Branch("BSemiPDG",&m_BSemiPDG);
    m_tree->Branch("BSemiStatus",&m_BSemiStatus);
    m_tree->Branch("CPDG",&m_CPDG);
    m_tree->Branch("CStatus",&m_CStatus);
    m_tree->Branch("CSemiPDG",&m_CSemiPDG);
    m_tree->Branch("CSemiStatus",&m_CSemiStatus);


    //Register TTree
    sc = m_histos->regTree("/AANT/el",m_tree);
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

    m_BPDG          = new std::vector<int>();
    m_BStatus       = new std::vector<int>();
    m_BSemiPDG      = new std::vector<int>();
    m_BSemiStatus   = new std::vector<int>();
    m_CPDG          = new std::vector<int>();
    m_CStatus       = new std::vector<int>();
    m_CSemiPDG      = new std::vector<int>();
    m_CSemiStatus   = new std::vector<int>();
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
                    m_BPDG->push_back(part->pdg_id());
                    m_BStatus->push_back(part->status());
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
                        m_BSemiPDG->push_back(part->pdg_id());
                        m_BSemiStatus->push_back(part->status());
                    }
                }
            }
            if(isCHadron(part))
            {
                if(part->status()==196 || part->status()==197 || part->status()==198 || part->status() ==199 || part->status()==200)
                {
                    m_CPDG->push_back(part->pdg_id());
                    m_CStatus->push_back(part->status());
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
                        m_CSemiPDG->push_back(part->pdg_id());
                        m_CSemiStatus->push_back(part->status());
                    }
                }
            }
        }
    }
}

void SoftElectron::DoElectronMatch()
{
    int ElSize      = m_electronCollection->size();

    m_el_trk_PtBr   =   new std::vector<double>(ElSize,-100);    
    m_el_trk_EtaBr  =   new std::vector<double>(ElSize,-100);    
    m_el_trk_PhiBr  =   new std::vector<double>(ElSize,-100);    
    m_el_cl_PtBr    =   new std::vector<double>(ElSize,-100);    
    m_el_cl_EtaBr   =   new std::vector<double>(ElSize,-100);    
    m_el_cl_PhiBr   =   new std::vector<double>(ElSize,-100);    
    m_el_truth_PtBr =   new std::vector<double>(ElSize,-100);    
    m_el_truth_EtaBr=   new std::vector<double>(ElSize,-100);    
    m_el_truth_PhiBr=   new std::vector<double>(ElSize,-100);    
 
    m_elAuthor      =   new std::vector<int>(ElSize,-100);       
    m_elAuthorSofte =   new std::vector<int>(ElSize,-100);       
    m_elMtchd       =   new std::vector<int>(ElSize,-100);       
    m_mtchdParent   =   new std::vector<int>(ElSize,-100);       

    MsgStream mlog(msgSvc(), name());

    MCTruthPartClassifier::ParticleDef partDef;
    for(int i = 0; i < m_electronCollection->size(); ++i)
    {
        const Analysis::Electron* Electron = m_electronCollection->at(i);
        HepMC::GenParticle* elParent = this->GetElectronParent(Electron);

        if(elParent)
        {
            m_elMtchd->at(i)        = 1;
            m_mtchdParent->at(i)    = elParent->pdg_id();
            m_el_truth_PtBr->at(i)  = elParent->momentum().perp()/1000;
            m_el_truth_EtaBr->at(i) = elParent->momentum().eta();
            m_el_truth_PhiBr->at(i) = elParent->momentum().phi();
        }
        
        //Track Particle 
        const Rec::TrackParticle* ElTrkPartcl = Electron->trackParticle();
        if(ElTrkPartcl)
        {
            m_el_trk_PtBr->at(i)    = ElTrkPartcl->pt()/1000;
            m_el_trk_EtaBr->at(i)   = ElTrkPartcl->eta();
            m_el_trk_PhiBr->at(i)   = ElTrkPartcl->phi();
        }

        //Cluster
        const CaloCluster* ElCluster = Electron->cluster();
        if(ElCluster)
        {
            m_el_cl_PtBr->at(i)     = ElCluster->pt()/1000;
            m_el_cl_EtaBr->at(i)    = ElCluster->eta();
            m_el_cl_PhiBr->at(i)    = ElCluster->phi();
        }

        //Soft Electrons
        if(Electron->author(egammaParameters::AuthorSofte))
            m_elAuthorSofte->at(i)  = 1;

        if(Electron->author(egammaParameters::AuthorElectron))
            m_elAuthor->at(i)   = 1;

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
    delete m_elAuthor;
    delete m_elAuthorSofte;
    delete m_el_trk_PtBr;
    delete m_el_trk_EtaBr;
    delete m_el_trk_PhiBr;
    delete m_el_cl_PtBr;
    delete m_el_cl_EtaBr;
    delete m_el_cl_PhiBr;
    delete m_el_truth_PtBr;
    delete m_el_truth_EtaBr;
    delete m_el_truth_PhiBr;
    delete m_elMtchd;
    delete m_mtchdParent;
    delete m_BPDG;
    delete m_BStatus;
    delete m_BSemiPDG;
    delete m_BSemiStatus;
    delete m_CPDG;
    delete m_CStatus;
    delete m_CSemiPDG;
    delete m_CSemiStatus;
}

void SoftElectron::FillHistograms()
{

}

HepMC::GenParticle* SoftElectron::GetElectronParent(const Analysis::Electron* Electron)
{
    
    const CaloCluster* ElCluster = Electron->cluster();
 
    HepMC::GenParticle* elParent = 0;

    m_mcTruthClassifier->particleTruthClassifier(Electron);
    const HepMC::GenParticle* particle  = m_mcTruthClassifier->getGenPart();
    if(particle)
    {
        
        HerwigTruthClassifier myTruthClassifier(particle);
        elParent= myTruthClassifier.GetParent();
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
    return 1;
}
