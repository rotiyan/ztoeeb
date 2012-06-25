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
#include "EventInfo/EventType.h"

#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "TruthUtils/IHforTool.h"

#include <iostream>
#include <algorithm>

SoftElectron::SoftElectron(const std::string& name, ISvcLocator* pSvcLocator) 
    : Algorithm(name, pSvcLocator),
    m_mcTruthContainer(0),
    m_eventInfo(0),
    m_mcEventCollection(0),
    m_vxContainer(0),
    m_electronCollection(0),
    m_histos(0),
    m_mcTruthClassifier("MCTruthClassifier"),
    m_tree(0),
    m_elEtaCut(2.5),
    m_elCrackEtaCutLow(1.37),
    m_elCrackEtaCutHigh(1.52)
{
    declareProperty("MCParticleContainer",m_truthParticleContainerName = "SpclMC");
    declareProperty("TrackParticleContainer", m_trackParticleContainerName = "TrackParticleCandidate");
    declareProperty("MuonContainer",m_muonContainerName = "StacoMuonCollection");
    declareProperty("ElectronContainer", m_electronContainerName = "ElectronAODCollection");
    declareProperty("PrimaryVertexContainer", m_primaryVertexContainerName = "VxPrimaryCandidate");
    declareProperty("MCEventContainer", m_mcEventContainerName = "GEN_AOD");
    declareProperty("HforType",m_hforType ="isBB");
    declareProperty("FillGenInfo",m_fillGenInfo=false);
    declareProperty("DoTruthMatching",m_doTruthMatching=false);
    declareProperty("ElPtCut",m_elPtCut=15);//GeV
}

SoftElectron::~SoftElectron() {}

StatusCode SoftElectron::initialize() 
{
    MsgStream mlog( msgSvc(), name() );

    mlog <<MSG::INFO<< "HFOR TYPE: "<< m_hforType << endreq;
    mlog <<MSG::INFO<< "FILLGENINFO "<<m_fillGenInfo <<endreq;
    mlog <<MSG::INFO<< "DOTRUTHMATCHING"<<m_doTruthMatching <<endreq;

    StatusCode sc = service("StoreGateSvc", m_storeGate);
    
    m_hfor_tool = ToolHandle<IHforTool>("HforTool/hforTool", this);
    if(m_hfor_tool.retrieve().isFailure()){
        mlog <<MSG::ERROR 
                << ": Failed to retrieve tool "
                <<endreq;
        return StatusCode::FAILURE;
    }

    if(m_doTruthMatching)
    {
        if(m_mcTruthClassifier.retrieve().isFailure())
        {
            mlog <<MSG::FATAL
               << "Failed to retrieve Truth Classifier Tool"
               << endreq;
        }
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

    /** Retrieve AOD Containers */
    sc = this->LoadContainers();
    if (sc.isFailure())
    {
        mlog << MSG::FATAL <<"Could not load containers" <<endreq;
        return StatusCode::FAILURE;
    }

    //Book Ntuple Containers
    this->BookNtupleContainers();

    if(m_fillGenInfo) //MC
    {
        /** Heavy Flavor Overlap Removal **/
        std::string hfor_type ="";
        if ( m_hfor_tool->execute().isSuccess() )
            hfor_type = m_hfor_tool->getDecision();

        if(hfor_type ==m_hforType)
        {
            this->FillElectrons();
            this->FindTruthParticle();
            if(m_doTruthMatching)
                this->DoElectronMatch();
        }
    }
    else //data
    {
        this->FillElectrons();
    }

    //end of event loop 
    m_tree->Fill();
    this->ClearContainers();
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

    //Electrons
    m_tree->Branch("el_trk_Pt",&m_el_trk_Pt);
    m_tree->Branch("el_trk_Eta",&m_el_trk_Eta);
    m_tree->Branch("el_trk_Phi",&m_el_trk_Phi);
    m_tree->Branch("el_cl_Pt",&m_el_cl_Pt);
    m_tree->Branch("el_cl_Eta",&m_el_cl_Eta);
    m_tree->Branch("el_cl_Phi",&m_el_cl_Phi);
    m_tree->Branch("el_truth_Pt",&m_el_truth_Pt);
    m_tree->Branch("el_truth_Eta",&m_el_truth_Eta);
    m_tree->Branch("el_truth_Phi",&m_el_truth_Phi);
    m_tree->Branch("elIsMtchd",&m_elMtchd);
    m_tree->Branch("mtchdParent",&m_mtchdParent);
    m_tree->Branch("mtchdGrndParent",&m_mtchdGrndParent);
    m_tree->Branch("elAuthor",&m_elAuthor);
    m_tree->Branch("elAuthorSofte",&m_elAuthorSofte);
    m_tree->Branch("el_charge",&m_el_charge);

    m_tree->Branch("el_loose",&m_el_id_loose);
    m_tree->Branch("el_loosePP",&m_el_id_loosepp);
    m_tree->Branch("el_medium",&m_el_id_medium);
    m_tree->Branch("el_mediumPP",&m_el_id_mediumpp);
    m_tree->Branch("el_tight",&m_el_id_tight);
    m_tree->Branch("el_tightPP",&m_el_id_tightpp);

    //Truth 
    m_tree->Branch("ZElPt",&m_ZElPt);
    m_tree->Branch("ZElEta",&m_ZElEta);
    m_tree->Branch("ZElPhi",&m_ZElPhi);

    m_tree->Branch("BPDG",&m_BPDG);
    m_tree->Branch("BStatus",&m_BStatus);
    m_tree->Branch("BPt",&m_BPt);
    m_tree->Branch("BEta",&m_BEta);
    m_tree->Branch("BPhi",&m_BPhi);
    m_tree->Branch("BBC",&m_BBC);
    m_tree->Branch("BisSemiElectron",&m_BisSemiElectron);
    m_tree->Branch("BsemiElPt",&m_BsemiElPt);
    m_tree->Branch("BsemiElEta",&m_BsemiElEta);
    m_tree->Branch("BsemiElPhi",&m_BsemiElPhi);
    
    m_tree->Branch("CPDG",&m_CPDG);
    m_tree->Branch("CStatus",&m_CStatus);
    m_tree->Branch("CPt",&m_CPt);
    m_tree->Branch("CEta",&m_CEta);
    m_tree->Branch("CPhi",&m_CPhi);
    m_tree->Branch("CisSemiElectron",&m_CisSemiElectron);
    m_tree->Branch("CParentBC",&m_CparentBC);
    m_tree->Branch("CGrndParentBC",&m_CgrndParentBC);
    m_tree->Branch("CsemiElPt",&m_CsemiElPt);
    m_tree->Branch("CsemiElEta",&m_CsemiElEta);
    m_tree->Branch("CsemiElPhi",&m_CsemiElPhi);

    //Truth ME quark
    m_tree->Branch("bQuarkME_pt",&m_bQuarkME_pt);
    m_tree->Branch("bQuarkME_eta",&m_bQuarkME_eta);
    m_tree->Branch("bQuarkME_phi",&m_bQuarkME_phi);
    m_tree->Branch("bQuarkME_pdg",&m_bQuarkME_pdg);


    //Register TTree
    sc = m_histos->regTree("/AANT/el",m_tree);
    return StatusCode::SUCCESS;
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
        if(part->barcode() > 10000)
            continue;

        //Decaying Z-boson
        if(std::abs(part->pdg_id()) ==23 && part->status()==155)
        {
            std::vector<const HepMC::GenParticle*> zchildren = this->GetChildren(part);
            for(std::vector<const HepMC::GenParticle*>::iterator Iter = zchildren.begin(); Iter != zchildren.end(); ++Iter)
            {
                m_ZElPt->push_back((*Iter)->momentum().perp()/1000);
                m_ZElEta->push_back((*Iter)->momentum().eta());
                m_ZElPhi->push_back((*Iter)->momentum().phi());
            }

        }

        //ME b-quark
        if(std::abs(part->pdg_id())==5)
        {
            bool hasmpiparent(false);
            bool hasbhadronparent(false);

            HepMC::GenVertex* prodVtx = part->production_vertex();
            if(prodVtx)
            {

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
                    m_BBC->push_back(part->barcode());

                    bool hasDaughterEl(false);
                    std::vector<const HepMC::GenParticle*> children = this->GetChildren(part);
                    for(std::vector<const HepMC::GenParticle*>::iterator Iter = children.begin(); Iter != children.end(); ++Iter)
                    {
                        if(std::abs((*Iter)->pdg_id()) ==11)
                        {
                            hasDaughterEl = true;
                            m_BsemiElPt->push_back((*Iter)->momentum().perp()/1000);
                            m_BsemiElEta->push_back((*Iter)->momentum().eta());
                            m_BsemiElPhi->push_back((*Iter)->momentum().phi());

                            break;
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

                    std::vector<const HepMC::GenParticle*>parentVec     = this->GetParents(part);
                    for(std::vector<const HepMC::GenParticle*>::iterator Iter = parentVec.begin(); Iter != parentVec.end(); ++Iter)
                    {
                        m_CparentBC->push_back((*Iter)->barcode());

                        std::vector<const HepMC::GenParticle*> grndPrntVec  = this->GetParents((*Iter));
                        for (std::vector<const HepMC::GenParticle*>::iterator gIter = grndPrntVec.begin(); gIter != grndPrntVec.end(); ++gIter)
                        {
                            m_CgrndParentBC->push_back((*gIter)->barcode());
                        }
                    }

                    bool hasDaughterEl(false);
                    std::vector<const HepMC::GenParticle*> children     = this->GetChildren(part);
                    for(std::vector<const HepMC::GenParticle*>::iterator Iter = children.begin(); Iter != children.end(); ++Iter)
                    {
                        if(std::abs((*Iter)->pdg_id()) ==11)
                        {
                            hasDaughterEl = true;
                            m_CsemiElPt->push_back((*Iter)->momentum().perp()/1000);
                            m_CsemiElEta->push_back((*Iter)->momentum().eta());
                            m_CsemiElPhi->push_back((*Iter)->momentum().phi());

                            //It is highly unlikely a HF hadron to have 2 electron daughters. But still 
                            break;
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

void SoftElectron::FillElectrons()
{
    MsgStream mlog(msgSvc(), name());

    for(unsigned int i = 0; i < m_electronCollection->size(); ++i)
    {
        const Analysis::Electron* Electron = m_electronCollection->at(i);

        //Fill Common electron properties
        m_el_charge->at(i)      = Electron->charge();
        m_el_id_loose->at(i)    = Electron->passID(egammaPID::ElectronIDLoose);
        m_el_id_loosepp->at(i)  = Electron->passID(egammaPID::ElectronIDLoosePP);
        m_el_id_medium->at(i)   = Electron->passID(egammaPID::ElectronIDMedium);
        m_el_id_mediumpp->at(i) = Electron->passID(egammaPID::ElectronIDMediumPP);
        m_el_id_tight->at(i)    = Electron->passID(egammaPID::ElectronIDTight);
        m_el_id_tightpp->at(i)  = Electron->passID(egammaPID::ElectronIDTightPP);
        
        //Track Particle 
        const Rec::TrackParticle* ElTrkPartcl = Electron->trackParticle();
        if(ElTrkPartcl)
        {
            double elTrkPt  = ElTrkPartcl->pt()/1000;
            if(elTrkPt > m_elPtCut)
            {
                m_el_trk_Pt->at(i)  = ElTrkPartcl->pt()/1000;
                m_el_trk_Eta->at(i) = ElTrkPartcl->eta();
                m_el_trk_Phi->at(i) = ElTrkPartcl->phi();
            }
        }

        //Cluster
        const CaloCluster* ElCluster = Electron->cluster();
        if(ElCluster)
        {
            double elClPt   = ElCluster->pt()/1000;
            double elClEta  = ElCluster->eta();
            double elClPhi  = ElCluster->phi();

            //if( (std::abs(elClEta) >m_elCrackEtaCutHigh && std::abs(elClEta) < m_elCrackEtaCutLow))
            if(std::abs(elClEta) < m_elEtaCut && elClPt > m_elPtCut )
            {
                m_el_cl_Pt->at(i)   = elClPt;
                m_el_cl_Eta->at(i)  = elClEta;
                m_el_cl_Phi->at(i)  = elClPhi;
            }
        }

        //Soft Electrons
        if(Electron->author(egammaParameters::AuthorSofte))
            m_elAuthorSofte->at(i)  = 1;

        if(Electron->author(egammaParameters::AuthorElectron))
            m_elAuthor->at(i)   = 1;

    }
}

void SoftElectron::DoElectronMatch()
{
    int ElSize      = m_electronCollection->size();

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
            m_el_truth_Pt->at(i)    = elParent->momentum().perp()/1000;
            m_el_truth_Eta->at(i)   = elParent->momentum().eta();
            m_el_truth_Phi->at(i)   = elParent->momentum().phi();
        }
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
 
    sc = m_storeGate->retrieve( m_electronCollection,m_electronContainerName);
    if(sc.isFailure())
    {
        mlog << MSG::ERROR <<"could not retrieve ElectronContainer" <<endreq;
        return sc;
    }

    if(m_fillGenInfo)
    {
        sc=m_storeGate->retrieve(m_mcTruthContainer, m_truthParticleContainerName);
        if(sc.isFailure())
        {
            mlog << MSG::ERROR << "could not retrive TruthParticlecontainer" <<endreq;
            return sc;
        }
    
        sc = m_storeGate->retrieve(m_mcEventCollection, m_mcEventContainerName);
        if(sc.isFailure())
        {
            mlog << MSG::ERROR <<"could no retrive MC EventCollection" <<endreq;
            return sc;
        }
    }

    return sc;
}

void SoftElectron::BookNtupleContainers()
{
    //Truth
    m_ZElPt         = new std::vector<double>();
    m_ZElEta        = new std::vector<double>();
    m_ZElPhi        = new std::vector<double>();

    m_BPDG          = new std::vector<int>();
    m_BStatus       = new std::vector<int>();
    m_BPt           = new std::vector<double>();
    m_BEta          = new std::vector<double>();
    m_BPhi          = new std::vector<double>();
    m_BBC           = new std::vector<int>();
    m_BisSemiElectron= new std::vector<int>();
    m_BsemiElPt     = new std::vector<double>();
    m_BsemiElEta    = new std::vector<double>();
    m_BsemiElPhi    = new std::vector<double>();

    m_CPDG          = new std::vector<int>();
    m_CStatus       = new std::vector<int>();
    m_CPt           = new std::vector<double>();
    m_CEta          = new std::vector<double>();
    m_CPhi          = new std::vector<double>();
    m_CisSemiElectron= new std::vector<int>();
    m_CparentBC     = new std::vector<int>();
    m_CgrndParentBC = new std::vector<int>();
    m_CsemiElPt     = new std::vector<double>();
    m_CsemiElEta    = new std::vector<double>();
    m_CsemiElPhi    = new std::vector<double>();

    m_bQuarkME_pt   = new std::vector<double>();
    m_bQuarkME_eta  = new std::vector<double>();
    m_bQuarkME_phi  = new std::vector<double>();
    m_bQuarkME_pdg  = new std::vector<int>();

    //TruthMatch Electron
    int ElSize      = m_electronCollection->size();
    m_elMtchd           =   new std::vector<int>(ElSize,-100);       
    m_mtchdParent       =   new std::vector<int>(ElSize,-100);       
    m_mtchdGrndParent   =   new std::vector<int>(ElSize,-100);
    m_el_truth_Pt       =   new std::vector<double>(ElSize,-100);    
    m_el_truth_Eta      =   new std::vector<double>(ElSize,-100);    
    m_el_truth_Phi      =   new std::vector<double>(ElSize,-100);    

    //Reco Electron
    m_el_trk_Pt     =   new std::vector<double>(ElSize,-100);    
    m_el_trk_Eta    =   new std::vector<double>(ElSize,-100);    
    m_el_trk_Phi    =   new std::vector<double>(ElSize,-100);    
    m_el_cl_Pt      =   new std::vector<double>(ElSize,-100);    
    m_el_cl_Eta     =   new std::vector<double>(ElSize,-100);    
    m_el_cl_Phi     =   new std::vector<double>(ElSize,-100);    
    
    m_elAuthor      =   new std::vector<int>(ElSize,-100);       
    m_elAuthorSofte =   new std::vector<int>(ElSize,-100);       
    m_el_charge     =   new std::vector<int>(ElSize,-100);


    m_el_id_loose   =   new std::vector<bool>(ElSize,false);
    m_el_id_loosepp =   new std::vector<bool>(ElSize,false);
    m_el_id_medium  =   new std::vector<bool>(ElSize,false);
    m_el_id_mediumpp=   new std::vector<bool>(ElSize,false);
    m_el_id_tight   =   new std::vector<bool>(ElSize,false);
    m_el_id_tightpp =   new std::vector<bool>(ElSize,false);

}

void SoftElectron::ClearContainers()
{
    delete m_el_trk_Pt;
    delete m_el_trk_Eta;
    delete m_el_trk_Phi;
    delete m_el_cl_Pt;
    delete m_el_cl_Eta;
    delete m_el_cl_Phi;

    delete m_elAuthor;
    delete m_elAuthorSofte;
    delete m_el_charge;

    delete m_el_id_loose;
    delete m_el_id_loosepp;
    delete m_el_id_medium;
    delete m_el_id_mediumpp;
    delete m_el_id_tight;
    delete m_el_id_tightpp;

    delete m_ZElPt;
    delete m_ZElEta;
    delete m_ZElPhi;

    delete m_BPDG;
    delete m_BStatus;
    delete m_BPt;
    delete m_BEta;
    delete m_BPhi;
    delete m_BBC;
    delete m_BisSemiElectron;
    delete m_BsemiElPt;
    delete m_BsemiElEta;
    delete m_BsemiElPhi;
    delete m_CPDG;
    delete m_CStatus;
    delete m_CPt;
    delete m_CEta;
    delete m_CPhi;
    delete m_CisSemiElectron;
    delete m_CparentBC;
    delete m_CgrndParentBC;
    delete m_CsemiElPt;
    delete m_CsemiElEta;
    delete m_CsemiElPhi;

    delete m_bQuarkME_pt;
    delete m_bQuarkME_eta;
    delete m_bQuarkME_phi;
    delete m_bQuarkME_pdg;

    delete m_el_truth_Pt;
    delete m_el_truth_Eta;
    delete m_el_truth_Phi;
    delete m_elMtchd;
    delete m_mtchdParent;
    delete m_mtchdGrndParent;
}

const HepMC::GenParticle* SoftElectron::GetElectronParent(const Analysis::Electron* Electron)
{
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

std::vector<const HepMC::GenParticle*> SoftElectron::GetParents(const HepMC::GenParticle* p)
{
    std::vector<const HepMC::GenParticle*> parentVec; 

    HepMC::GenVertex* pvtx  = p->production_vertex();
    if(pvtx)
    {
        HepMC::GenVertex::particle_iterator pin = pvtx->particles_begin(HepMC::parents);
        for(;pin != pvtx->particles_end(HepMC::parents); ++pin)
        {
            const HepMC::GenParticle* parent = (*pin);
            if(parent->pdg_id() == p->pdg_id())
            {
                return this->GetParents(parent);
            }
            else 
            {
                parentVec.push_back(parent);
            }
        }
    }
    return parentVec;
}
