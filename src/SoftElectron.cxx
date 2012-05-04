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
    m_tree->Branch("mtchdParent",&m_mtchdParent);
    m_tree->Branch("mtchdGrndParent",&m_mtchdGrndParent);
    m_tree->Branch("elAuthor",&m_elAuthor);
    m_tree->Branch("elAuthorSofte",&m_elAuthorSofte);
    m_tree->Branch("BPDG",&m_BPDG);
    m_tree->Branch("BStatus",&m_BStatus);
    m_tree->Branch("BPt",&m_BPt);
    m_tree->Branch("BEta",&m_BEta);
    m_tree->Branch("BPhi",&m_BPhi);
    m_tree->Branch("BisSemiElectron",&m_BisSemiElectron);
    m_tree->Branch("CPDG",&m_CPDG);
    m_tree->Branch("CStatus",&m_CStatus);
    m_tree->Branch("CPt",&m_CPt);
    m_tree->Branch("CEta",&m_CEta);
    m_tree->Branch("CPhi",&m_CPhi);
    m_tree->Branch("CisSemiElectron",&m_CisSemiElectron);

    m_tree->Branch("bQuarkME_pt",&m_bQuarkME_pt);
    m_tree->Branch("bQuarkME_eta",&m_bQuarkME_eta);
    m_tree->Branch("bQuarkME_phi",&m_bQuarkME_phi);
    m_tree->Branch("bQuarkME_pdg",&m_bQuarkME_pdg);


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
    m_BPt           = new std::vector<double>();
    m_BEta          = new std::vector<double>();
    m_BPhi          = new std::vector<double>();
    m_BisSemiElectron= new std::vector<int>();
    m_CPDG          = new std::vector<int>();
    m_CStatus       = new std::vector<int>();
    m_CPt           = new std::vector<double>();
    m_CEta          = new std::vector<double>();
    m_CPhi          = new std::vector<double>();
    m_CisSemiElectron= new std::vector<int>();

    m_bQuarkME_pt   = new std::vector<double>();
    m_bQuarkME_eta  = new std::vector<double>();
    m_bQuarkME_phi  = new std::vector<double>();
    m_bQuarkME_pdg  = new std::vector<int>();

    HepMC::GenEvent::particle_const_iterator  pitr = GenEvent->particles_begin();
    for(; pitr !=  GenEvent->particles_end(); ++pitr)
    {
        const HepMC::GenParticle* part = (*pitr);
        if(part->barcode() > 10000)
            continue;


        HepMC::GenVertex* prodVtx = part->production_vertex();
        if(prodVtx)
        {
            bool hasmpiparent(false);
            bool hasbhadronparent(false);

            HepMC::GenVertex::particle_iterator pin = prodVtx->particles_begin(HepMC::parents) ;

            for(; pin != prodVtx->particles_end(HepMC::parents) && !hasbhadronparent; ++pin)
            {
                int pdgin(abs((*pin)->pdg_id()));
                if ( (pdgin%10000)/1000 == 5 || (pdgin%1000)/100 == 5 )
                    hasbhadronparent = true;
                if ( pdgin == 0  && (*pin)->status()== 120)
                    hasmpiparent = true;
            }
            if(!hasbhadronparent && !hasmpiparent)
            {
                if(abs(part->pdg_id())==5 && (part->status() == 123  || part->status() ==124))
                {
                    m_bQuarkME_pt->push_back(part->momentum().perp()/1000);
                    m_bQuarkME_eta->push_back(part->momentum().eta());
                    m_bQuarkME_phi->push_back(part->momentum().phi());
                    m_bQuarkME_pdg->push_back(part->pdg_id());
                }
            }
        }

        if(part->momentum().perp()/1000 > 2)
        {
            int pdgid   = part->pdg_id();
            int rest1   = abs(pdgid) % 1000;
            int rest2   = abs(pdgid) % 10000;
           
            if( (rest2 >=5000 && rest2 < 6000)  || (rest1 >=500 && rest1 < 600))
            {
                if(isFinalState(part,5))
                {
                    m_BPDG->push_back(part->pdg_id());
                    m_BStatus->push_back(part->status());

                    m_BPt->push_back(part->momentum().perp()/1000);
                    m_BEta->push_back(part->momentum().eta());
                    m_BPhi->push_back(part->momentum().phi());

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
                        m_BisSemiElectron->push_back(1);
                    }
                    else
                    {
                        m_BisSemiElectron->push_back(0);
                    }
                }
            }
            if( (rest2 >=4000 && rest2 < 5000) || (rest1 >=400 && rest1 <500) )
            {
                if(isFinalState(part,4))
                {
                    m_CPDG->push_back(part->pdg_id());
                    m_CStatus->push_back(part->status());

                    m_CPt->push_back(part->momentum().perp()/1000);
                    m_CEta->push_back(part->momentum().eta());
                    m_CPhi->push_back(part->momentum().phi());

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
                        m_CisSemiElectron->push_back(1);
                    }
                    else
                    {
                        m_CisSemiElectron->push_back(0);
                    }
                }
            }
        }
    }
}

bool SoftElectron::isFinalState(const HepMC::GenParticle* part, int type)
{
    if(part->end_vertex())
    {
        HepMC::GenVertex::particle_iterator pin = part->end_vertex()->particles_begin(HepMC::children);
        for(; pin != part->end_vertex()->particles_end(HepMC::children); ++pin)
        {
            const HepMC::GenParticle* child = (*pin);
            if(child->barcode() > 100000)
                continue;
            int pdgid   = child->pdg_id();
            int rest1   = abs(pdgid)%1000;
            int rest2   = abs(pdgid)%10000;

            if(type==5)
            {
                if( (rest2 >= 5000 && rest2 < 6000) || (rest1>=500 && rest1 <600))
                    return false;
                if(child->barcode()<10000)
                {
                    if(!isFinalState(child,type))
                        return false;
                }
            }
            if(type==4)
            {
                if( (rest2 >=4000 && rest2 <5000) || (rest1>=400 && rest1 <500))
                    return false;
                if(child->barcode()<10000)
                {
                    if(!isFinalState(child,type))
                        return false;
                }
            }
        }            
    }
    return true;
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
    m_mtchdGrndParent=  new std::vector<int>(ElSize,-100);

    MsgStream mlog(msgSvc(), name());

    MCTruthPartClassifier::ParticleDef partDef;
    for(unsigned int i = 0; i < m_electronCollection->size(); ++i)
    {
        const Analysis::Electron* Electron = m_electronCollection->at(i);
        const HepMC::GenParticle* elParent = this->GetElectronParent(Electron);

        if(elParent)
        {
            const HepMC::GenParticle* elgrndParent = this->GetMother(elParent);
            if(elgrndParent)
                m_mtchdGrndParent->at(i)    = elgrndParent->pdg_id();

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
    delete m_mtchdGrndParent;
    delete m_BPDG;
    delete m_BStatus;
    delete m_BPt;
    delete m_BEta;
    delete m_BPhi;
    delete m_BisSemiElectron;
    delete m_CPDG;
    delete m_CStatus;
    delete m_CPt;
    delete m_CEta;
    delete m_CPhi;
    delete m_CisSemiElectron;
    delete m_bQuarkME_pt;
    delete m_bQuarkME_eta;
    delete m_bQuarkME_phi;
    delete m_bQuarkME_pdg;

}

void SoftElectron::FillHistograms()
{

}

const HepMC::GenParticle* SoftElectron::GetElectronParent(const Analysis::Electron* Electron)
{
    
    const CaloCluster* ElCluster = Electron->cluster();
 
    const HepMC::GenParticle* elParent = 0;

    m_mcTruthClassifier->particleTruthClassifier(Electron);
    const HepMC::GenParticle* particle  = m_mcTruthClassifier->getGenPart();
    if(particle)
    {
        elParent= this->GetMother(particle);
    }
    return elParent;
}

const HepMC::GenParticle*  SoftElectron::GetMother(const HepMC::GenParticle* thePart)
{
    const HepMC::GenVertex*   partOriVert = thePart->production_vertex();
    long partPDG       = thePart->pdg_id();
    int  partBarcode   = thePart->barcode()%1000000;
    long MotherPDG(0);

    const HepMC::GenVertex*   MothOriVert(0);
    const HepMC::GenParticle* theMoth(0);

    if(!partOriVert) return theMoth;

    int itr=0;
    do
    { 
        if(itr!=0)  partOriVert = MothOriVert;  
   
        for (HepMC::GenVertex::particles_in_const_iterator
                itrMother = partOriVert->particles_in_const_begin();
                itrMother!= partOriVert->particles_in_const_end(); ++itrMother)
        {
            MotherPDG   = (*itrMother)->pdg_id();
            MothOriVert = (*itrMother)->production_vertex();
            theMoth     = (*itrMother); 
            if(MotherPDG==partPDG) break;
        }
        itr++;
        if(itr>100)
        {
            std::cout <<"getMother:: infinite while" << std::endl; 
            break;
        }
    }while (MothOriVert !=0 &&MotherPDG==partPDG && partBarcode<200000);

    return theMoth;
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
