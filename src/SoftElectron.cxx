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
#include "TString.h"

#include <utility>

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

bool cmp (const Analysis::Electron* a, const Analysis::Electron* b) {
    return a->cluster()->pt() < b->cluster()->pt();
}

SoftElectron::SoftElectron(const std::string& name, ISvcLocator* pSvcLocator) 
    : Algorithm(name, pSvcLocator),
    m_mcTruthContainer(0),
    m_eventInfo(0),
    m_mcEventCollection(0),
    m_vxContainer(0),
    m_electronCollection(0),
    m_tree(0),
    m_histos(0),
    m_mcTruthClassifier("MCTruthClassifier"),
    m_trigDec( "Trig::TrigDecisionTool" ),
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
    declareProperty("ElPtCut",m_elPtCut=15);//GeV
    declareProperty("TrigDecisionTool",m_trigDec);
}

SoftElectron::~SoftElectron() 
{


}

StatusCode SoftElectron::initialize() 
{
    MsgStream mlog( msgSvc(), name() );

    mlog <<MSG::INFO<< "HFOR TYPE: "<< m_hforType << endreq;
    mlog <<MSG::INFO<< "FILLGENINFO "<<m_fillGenInfo <<endreq;

    StatusCode sc = service("StoreGateSvc", m_storeGate);
    
    //Retrieve Hfor tool
    m_hfor_tool = ToolHandle<IHforTool>("HforTool/hforTool", this);
    if(m_hfor_tool.retrieve().isFailure()){
        mlog <<MSG::ERROR 
                << ": Failed to retrieve tool "
                <<endreq;
        return StatusCode::FAILURE;
    }

    //Retrive MCTruth Classifier
    if(m_mcTruthClassifier.retrieve().isFailure())
    {
        mlog <<MSG::FATAL
           << "Failed to retrieve Truth Classifier Tool"
           << endreq;
    }

    //Retrieve Trig Decision Tool
    if(m_trigDec.retrieve().isFailure())
    {
        mlog<<MSG::FATAL
            << "Failed to retrieve Trigger Decision Tool"
            <<endreq;
    }

    // THistSvc init:
    sc = service("THistSvc", m_histos);
    if ( sc.isFailure() )
    {
        mlog << MSG::FATAL << ">>> Unable to locate the Histogram service" << endreq;
    }
    
    // histograms:
    sc = this->BookTree();
    if( sc.isFailure())
    {
        mlog <<MSG::FATAL << "Could not book tree" <<endreq;
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

    bool vxPass     = false;
    bool trigPass   = false;

    if(m_vxContainer)
    {
        VxContainer::const_iterator vxIter = m_vxContainer->begin();
        int nTracks = (*vxIter)->vxTrackAtVertex()->size();
        if(nTracks>=3)
            vxPass = true;
    }
    
    //Run Number
    m_runNumber     = m_eventInfo->event_ID()->run_number();
    m_lumiNumber    = m_eventInfo->event_ID()->lumi_block();

    unsigned int runD_Min       = 179710;
    unsigned int runJ_Max       = 186775;
    unsigned int runK_Min       = 186873;
    unsigned int runK_Max       = 187815;
    unsigned int runL_Min       = 188902;
    unsigned int runM_Max       = 191933;

    //Check the trigger
    if(m_runNumber >= runD_Min && m_runNumber <= runJ_Max)
    {
        trigPass = m_trigDec->isPassed("EF_2e12_medium");
    }
    else if(m_runNumber >= runK_Min && m_runNumber <= runK_Max)
    {
        trigPass = m_trigDec->isPassed("EF_2e12T_medium");
    }
    else if(m_runNumber >= runL_Min && m_runNumber <= runM_Max)
    {
        trigPass = m_trigDec->isPassed("EF_2e12Tvh_medium");
    }

    //Entering the event loop
    //
    if( vxPass)// && trigPass )
    {
        //Book Ntuple Containers
        this->BookNtupleContainers();

        if(m_fillGenInfo) //MC
        {
            // Heavy Flavor Overlap Removal 
            std::string hfor_type ="";
            //if ( m_hfor_tool->execute().isSuccess() )
            //    hfor_type = m_hfor_tool->getDecision();

            //if(hfor_type ==m_hforType)
            {
                this->FillElectrons();
                this->FindTruthParticle();
            }
        }
        else //data
        {
            this->FillElectrons();
        }
        m_tree->Fill();
        this->ClearContainers(); 
    }
    return sc;
}

StatusCode SoftElectron::finalize() 
{

  return StatusCode::SUCCESS;
}

StatusCode SoftElectron::BookTree() 
{
    MsgStream mlog( msgSvc(), name() );

    StatusCode sc ;

    m_tree      = new TTree("elId","Electron ID Variables");

    m_tree->Branch("numBLayerHits", &m_numberOfbLayerHits);
    m_tree->Branch("numBLayerOutliers",&m_numberOfbLayerOutliers);  
    m_tree->Branch("numBLayerShared",&m_numberOfbLayerSharedHits);  
    m_tree->Branch("numPixelHits",&m_numberOfPixelHits);
    m_tree->Branch("numPixelOutliers",&m_numberOfPixelOutliers);
    m_tree->Branch("numPixelHoles",&m_numberOfPixelHoles);
    m_tree->Branch("numPixelShared",&m_numberOfPixelSharedHits);   
    m_tree->Branch("numGangedPixels",&m_numberOfGangedPixels);
    m_tree->Branch("numGangedFlaggedFakes",&m_numberOfGangedFlaggedFakes);
    m_tree->Branch("numPixelDeadSensors",&m_numberOfPixelDeadSensors);  
    m_tree->Branch("numPixelSpoiltHits",&m_numberOfPixelSpoiltHits);

    m_tree->Branch("numSCTHits",&m_numberOfSCTHits);
    m_tree->Branch("numSCTOutliers",&m_numberOfSCTOutliers);
    m_tree->Branch("numSCTHoles",&m_numberOfSCTHoles);
    m_tree->Branch("numSCTDoubleHoles",&m_numberOfSCTDoubleHoles);
    m_tree->Branch("numSCTSharedHits",&m_numberOfSCTSharedHits);
    m_tree->Branch("numSCTDeadSensors",&m_numberOfSCTDeadSensors);
    m_tree->Branch("numSCTSpoitHits",&m_numberOfSCTSpoiltHits);

    m_tree->Branch("d0",&m_d0);
    m_tree->Branch("d0Err",&m_d0Err);
    m_tree->Branch("z0",&m_z0);
    m_tree->Branch("z0Err",&m_z0Err);

    m_tree->Branch("numTRTHits",&m_numberOfTRTHits);
    m_tree->Branch("numTRTOutliers",&m_numberOfTRTOutliers);
    m_tree->Branch("numTRTHoles",&m_numberOfTRTHoles);
    m_tree->Branch("numTRTHtHits",&m_numberOfTRTHTHits);
    m_tree->Branch("numTRTHTOutliers",&m_numberOfTRTHTOutliers);
    m_tree->Branch("numTRTDeadStraw",&m_numberOfTRTDeadStraws);
    m_tree->Branch("numTRTTubeHits",&m_numberOfTRTTubeHits);
    m_tree->Branch("cone30",&m_cone30);
    m_tree->Branch("ethad",&m_ethad);
    m_tree->Branch("ethad1",&m_ethad1);
    m_tree->Branch("emax",&m_emax);
    m_tree->Branch("emax2",&m_emax2);
    m_tree->Branch("emin",&m_emin);
    m_tree->Branch("isBMatch",&m_isBMatch);
    m_tree->Branch("isCMatch",&m_isCMatch);
    m_tree->Branch("isZMatch",&m_isZMatch);
    m_tree->Branch("isAuthor",&m_isAuthor);
    m_tree->Branch("isSofte",&m_isAuthorSofte);

    m_tree->Branch("elPt",&m_pt);
    m_tree->Branch("elEta",&m_eta);
    m_tree->Branch("elPhi",&m_phi);
    m_tree->Branch("elTrnsE",&m_et);

    m_tree->Branch("f1",&m_f1);
    m_tree->Branch("f1core",&m_f1core);
    m_tree->Branch("emins1",&m_emins1);
    m_tree->Branch("fracs1",&m_fracs1);
    m_tree->Branch("e2tsts1",&m_e2tsts1);
    m_tree->Branch("weta1",&m_weta1);
    m_tree->Branch("wtots1",&m_wtots1);
    m_tree->Branch("emaxs1",&m_emaxs1);
    m_tree->Branch("e233",&m_e233);
    m_tree->Branch("e237",&m_e237);
    m_tree->Branch("e277",&m_e277);
    m_tree->Branch("weta2",&m_weta2);
    m_tree->Branch("f3",&m_f3);
    m_tree->Branch("f3core",&m_f3core);
    m_tree->Branch("etcone",&m_etcone);
    m_tree->Branch("etcone20",&m_etcone20);
    m_tree->Branch("etcone30",&m_etcone30);
    m_tree->Branch("etcone40",&m_etcone40);
    m_tree->Branch("ptcone30",&m_ptcone30);
    m_tree->Branch("deltaEta1",&m_deltaEta1);
    m_tree->Branch("deltaEta2",&m_deltaEta2);
    m_tree->Branch("deltaPhi2",&m_deltaPhi2);
    m_tree->Branch("deltaPhiRescaled",&m_deltaPhiRescaled);
    m_tree->Branch("expectHitInBLayer",&m_expectHitInBLayer);



    sc = m_histos->regTree(Form("/AANT/%s",m_tree->GetName()),m_tree);
    if (sc.isFailure())
    {
        return sc;
    }

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
                    bool hasDaughterEl(false);
                    std::vector<const HepMC::GenParticle*> children = this->GetChildren(part);
                    for(std::vector<const HepMC::GenParticle*>::iterator Iter = children.begin(); Iter != children.end(); ++Iter)
                    {
                        if(std::abs((*Iter)->pdg_id()) ==11)
                        {
                            hasDaughterEl = true;
                            break;
                        }
                    }
                    if(hasDaughterEl)
                    {
                    }
                    else
                    {
                    }
                }
            }
            if( (rest2 >=4000 && rest2 < 5000) || (rest1 >=400 && rest1 <500) )
            {
                if(isFinalState(part,4))
                {
                    std::vector<const HepMC::GenParticle*>parentVec     = this->GetParents(part);
                    for(std::vector<const HepMC::GenParticle*>::iterator Iter = parentVec.begin(); Iter != parentVec.end(); ++Iter)
                    {
                        std::vector<const HepMC::GenParticle*> grndPrntVec  = this->GetParents((*Iter));
                        for (std::vector<const HepMC::GenParticle*>::iterator gIter = grndPrntVec.begin(); gIter != grndPrntVec.end(); ++gIter)
                        {
                        }
                    }

                    bool hasDaughterEl(false);
                    std::vector<const HepMC::GenParticle*> children     = this->GetChildren(part);
                    for(std::vector<const HepMC::GenParticle*>::iterator Iter = children.begin(); Iter != children.end(); ++Iter)
                    {
                        if(std::abs((*Iter)->pdg_id()) ==11)
                        {
                            hasDaughterEl = true;
                            //It is highly unlikely a HF hadron to have 2 electron daughters. But still 
                            break;
                        }
                    }
                    if(hasDaughterEl)
                    {
                    }
                    else
                    {
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

bool SoftElectron::isBHadron(const HepMC::GenParticle* part)
{
    int mpdg = part->pdg_id();
    return  (   ( 500 < mpdg && mpdg < 599 ) ||
            ( 10500 < mpdg && mpdg < 10599 ) ||
            (  5000 < mpdg && mpdg < 5999  ) ||
            ( 20500 < mpdg && mpdg < 20599 ) );
}

bool SoftElectron::isCHadron(const HepMC::GenParticle* part)
{
    int mpdg    = part->pdg_id();
    return  (   ( 400 < mpdg && mpdg < 499 ) || 
            ( 10400 < mpdg && mpdg < 10499 ) ||
            (  4000 < mpdg && mpdg < 4999  ) ||
            ( 20400 < mpdg && mpdg < 20499 ) );
}

bool SoftElectron::isZEvent()
{
    for( unsigned int i =0 ; i < m_electronCollection->size(); ++i)
    {
        const Analysis::Electron* Electron = m_electronCollection->at(i);
        if((Electron->author(egammaParameters::AuthorSofte) && Electron->author(egammaParameters::AuthorElectron)) || Electron->author(egammaParameters::AuthorElectron))
        {
            const CaloCluster* ElCluster = Electron->cluster();
            
            float elClPt    = ElCluster->pt()/1000;
            float elClEta   = ElCluster->eta();
            float elPhi     = ElCluster->phi();
        }
    }
}

void SoftElectron::FillElectrons()
{
    MsgStream mlog(msgSvc(), name());
    std::vector<const Analysis::Electron*> SelectdElectrons;
    
    for(unsigned int i = 0; i < m_electronCollection->size(); ++i)
    {
        // initialize variables
        float elClPt            = -100;
        float elClEta           = -100;
        float elClPhi           = -100;
        float elClE             = -100;
        float elTrkEta          = -100;
        float elTrkPhi          = -100;
        float elCharge          = -100;
        float trnsE             = -100;

        float nBLayerHits       = -100;
        float nBLayerOutliers   = -100;
        float nBLayerSharedHits = -100;
        bool  expectBLayerHit   = false;

        float nPixelHits        = -100;
        float nPixelOutliers    = -100;
        float nPixelHoles       = -100;
        float nPixelSharedHits  = -100;
        float nGangedPixel      = -100;
        float nGangedFlagFakes  = -100;
        float nPixelDeadSensor  = -100;
        float nPixelSpoiltHits  = -100;
       
        float nSCTHits          = -100;
        float nSCTOutliers      = -100;
        float nSCTHoles         = -100;
        float nSCTDoubleHoles   = -100;
        float nSCTSharedHits    = -100;
        float nSCTDeadSensors   = -100;
        float nSCTSpoiltHits    = -100;
       
        float nTRTHits          = -100;
        float nTRTOutliers      = -100;
        float nTRTHoles         = -100;
        float nTRTHTHits        = -100;
        float nTRTHTOutliers    = -100;
        float nTRTDeadStraw     = -100;
        float nTRTTubeHits      = -100;

        float ethad1            = -100;
        float ethad             = -100;
        float emax              = -100;
        float emax2             = -100;
        float emin              = -100;

        float eld0              = -100;
        float elz0              = -100;
        float eld0Err           = -100;
        float elz0Err           = -100;

        bool isEmLoose          = -100;
        bool isEmLoosePP        = -100;
        bool isEmMedium         = -100;
        bool isEmMediumPP       = -100;
        bool isEmTight          = -100;
        bool isEmTightPP        = -100;
        bool isGoodOQ           = -100;
        bool isBMatch           = -100;
        bool isCMatch           = -100;
        bool isZMatch           = -100;
        bool isAuthor           = -100;
        bool isAuthorSofte      = -100;

        float f1              = -100;                          
        float f1core          = -100;    
        float emins1          = -100;    
        float fracs1          = -100;    
        float e2tsts1         = -100;    
        float weta1           = -100;    
        float wtots1          = -100;    
        float emaxs1          = -100;    
        float e233            = -100;    
        float e237            = -100;    
        float e277            = -100;    
        float weta2           = -100;    
        float f3              = -100;    
        float f3core          = -100;    
        float etcone          = -100;    
        float etcone20        = -100;    
        float etcone30        = -100;    
        float etcone40        = -100;    
        float ptcone30        = -100;    
        float deltaEta1       = -100;    
        float deltaEta2       = -100;    
        float deltaPhi2       = -100;    
        float deltaPhiRescaled= -100;    
        bool  expectHitInBLayer=false;    


        const Analysis::Electron* Electron = m_electronCollection->at(i);
        //if((Electron->author(egammaParameters::AuthorSofte) && Electron->author(egammaParameters::AuthorElectron)) || Electron->author(egammaParameters::AuthorElectron))
        m_mcTruthClassifier->particleTruthClassifier(Electron);
        const HepMC::GenParticle* elParent    = m_mcTruthClassifier->getMother();
        const CaloCluster* ElCluster = Electron->cluster();
        const EMShower* Shower = Electron->detail<EMShower>();

        if(ElCluster && Shower && elParent)
        {
            //shower
            //https://svnweb.cern.ch/trac/atlasoff/browser/Reconstruction/egamma/egammaPIDTools/trunk/src/egammaElectronCutIDTool.cxx#L593
            e237            = Shower->e237();
            e277            = Shower->e277();
            ethad1          = Shower->ethad1();
            ethad           = Shower->ethad();
            emax            = Shower->emaxs1(); //E of fist max in strip
            emax2           = Shower->e2tsts1();
            emin            = Shower->emins1();


            //track particle
            if(Electron->trackParticle())
            {
                elTrkEta     = Electron->trackParticle()->eta();
                elTrkPhi     = Electron->trackParticle()->phi();

                const  Trk::TrackSummary* summary = Electron->trackParticle()->trackSummary();
                if(summary)
                {
                    nBLayerHits       = summary->get(Trk::numberOfBLayerHits);
                    nBLayerOutliers   = summary->get(Trk::numberOfBLayerOutliers);
                    nBLayerSharedHits = summary->get(Trk::numberOfBLayerSharedHits);

                    nPixelHits        = summary->get(Trk::numberOfPixelHits);
                    nPixelOutliers    = summary->get(Trk::numberOfPixelOutliers);
                    nPixelHoles       = summary->get(Trk::numberOfPixelHoles);
                    nPixelSharedHits  = summary->get(Trk::numberOfPixelSharedHits);
                    nGangedPixel      = summary->get(Trk::numberOfGangedPixels);
                    nGangedFlagFakes  = summary->get(Trk::numberOfGangedFlaggedFakes);
                    nPixelDeadSensor  = summary->get(Trk::numberOfPixelDeadSensors);
                    nPixelSpoiltHits  = summary->get(Trk::numberOfPixelSpoiltHits);
                   
                    nSCTHits          = summary->get(Trk::numberOfSCTHits);
                    nSCTOutliers      = summary->get(Trk::numberOfSCTOutliers);
                    nSCTHoles         = summary->get(Trk::numberOfSCTHoles);
                    nSCTDoubleHoles   = summary->get(Trk::numberOfSCTDoubleHoles);
                    nSCTSharedHits    = summary->get(Trk::numberOfSCTSharedHits);
                    nSCTDeadSensors   = summary->get(Trk::numberOfSCTDeadSensors);
                    nSCTSpoiltHits    = summary->get(Trk::numberOfSCTSpoiltHits);
                   
                    nTRTHits          = summary->get(Trk::numberOfTRTHits);
                    nTRTOutliers      = summary->get(Trk::numberOfTRTOutliers);
                    nTRTHoles         = summary->get(Trk::numberOfTRTHoles);
                    nTRTHTHits        = summary->get(Trk::numberOfTRTHighThresholdHits);
                    nTRTHTOutliers    = summary->get(Trk::numberOfTRTHighThresholdOutliers);
                    nTRTDeadStraw     = summary->get(Trk::numberOfTRTDeadStraws);
                    nTRTTubeHits      = summary->get(Trk::numberOfTRTTubeHits);
                }
                /// access to the perigee parameters
                const Trk::MeasuredPerigee* perigee = Electron->trackParticle()->measuredPerigee();
                Trk::ErrorMatrix  errormatrix = perigee->localErrorMatrix();
                if (perigee) 
                {
                    HepVector parameters= perigee->parameters();
                    eld0                = parameters[Trk::d0];
                    eld0Err             = errormatrix.error(Trk::z0);
                    elz0                = parameters[Trk::z0];
                    elz0Err             = errormatrix.error(Trk::z0);
                }
            }

            //Cluster
            elClPt       = ElCluster->pt()/1000;
            elClEta      = ElCluster->eta();
            elClPhi      = ElCluster->phi();
            elClE        = ElCluster->e()/1000;
 
            //eta position in second sampling
            double eta2     = fabs(ElCluster->etaBE(2));
            // transverse energy in calorimeter (using eta position in second sampling)
            if (fabs(eta2)<999.)
                trnsE  = cosh(eta2)!=0. ? ElCluster->energy()/cosh(eta2) : 0.;


            elCharge        = Electron->charge();
            isEmLoose       = Electron->passID(egammaPID::ElectronIDLoose);
            isEmLoosePP     = Electron->passID(egammaPID::ElectronIDLoosePP);
            isEmMedium      = Electron->passID(egammaPID::ElectronIDMedium);
            isEmMediumPP    = Electron->passID(egammaPID::ElectronIDMediumPP);
            isEmTight       = Electron->passID(egammaPID::ElectronIDTight);
            isEmTightPP     = Electron->passID(egammaPID::ElectronIDTightPP);
            isAuthor        = Electron->author(egammaParameters::AuthorElectron);
            isAuthorSofte   = Electron->author(egammaParameters::AuthorSofte);

            f1              = egammaParameters::f1;
            f1core          = egammaParameters::f1core;
            emins1          = egammaParameters::emins1;
            fracs1          = egammaParameters::fracs1;
            e2tsts1         = egammaParameters::e2tsts1;
            weta1           = egammaParameters::weta1;
            wtots1          = egammaParameters::wtots1;
            emaxs1          = egammaParameters::emaxs1;
            e233            = egammaParameters::e233;
            e237            = egammaParameters::e237;
            e277            = egammaParameters::e277;
            weta2           = egammaParameters::weta2;
            f3              = egammaParameters::f3;
            f3core          = egammaParameters::f3core;
            etcone          = egammaParameters::etcone;
            etcone20        = egammaParameters::etcone20;
            etcone30        = egammaParameters::etcone30;
            etcone40        = egammaParameters::etcone40;
            ptcone30        = egammaParameters::ptcone30;
            deltaEta1       = egammaParameters::deltaEta1;
            deltaEta2       = egammaParameters::deltaEta2;
            deltaPhi2       = egammaParameters::deltaPhi2;
            deltaPhiRescaled= egammaParameters::deltaPhiRescaled;
            expectHitInBLayer= egammaParameters::expectHitInBLayer;


            isGoodOQ       = Electron->isgoodoq(egammaPID::BADCLUSELECTRON) ==0 ? true: false;

            if( isGoodOQ && 
                    (!(std::abs(elClEta) < m_elCrackEtaCutHigh && std::abs(elClEta) > m_elCrackEtaCutLow)) &&
                    std::abs(elClEta) < m_elEtaCut && elClPt > m_elPtCut )
            {
                //m_h1_histMap.find("h1_etcone30")->second->Fill(Shower->etcone30()/1000);
                
                if(this->isBHadron(elParent))
                    isBMatch = true;
                
                if(this->isCHadron(elParent))
                    isCMatch = true;
                if(elParent->pdg_id() == 23)
                    isZMatch  = true;
            }
        }
        m_numberOfbLayerHits            -> push_back(nBLayerHits);
        m_numberOfbLayerOutliers        -> push_back(nBLayerOutliers);
        m_numberOfbLayerSharedHits      -> push_back(nBLayerSharedHits);
        
        m_numberOfPixelHits             -> push_back(nPixelHits);
        m_numberOfPixelOutliers         -> push_back(nPixelOutliers);
        m_numberOfPixelHoles            -> push_back(nPixelHoles);
        m_numberOfPixelSharedHits       -> push_back(nPixelSharedHits);
        m_numberOfGangedPixels          -> push_back(nGangedPixel);
        m_numberOfGangedFlaggedFakes    -> push_back(nGangedFlagFakes);
        m_numberOfPixelDeadSensors      -> push_back(nPixelDeadSensor);
        m_numberOfPixelSpoiltHits       -> push_back(nPixelSpoiltHits);

        m_numberOfSCTHits               -> push_back(nSCTHits);
        m_numberOfSCTOutliers           -> push_back(nSCTOutliers);
        m_numberOfSCTHoles              -> push_back(nSCTHoles); 
        m_numberOfSCTDoubleHoles        -> push_back(nSCTDoubleHoles);    
        m_numberOfSCTSharedHits         -> push_back(nSCTSharedHits);    
        m_numberOfSCTDeadSensors        -> push_back(nSCTDeadSensors);    
        m_numberOfSCTSpoiltHits         -> push_back(nSCTSpoiltHits);    

        m_numberOfTRTHits               -> push_back(nTRTHits);    
        m_numberOfTRTOutliers           -> push_back(nTRTOutliers);    
        m_numberOfTRTHoles              -> push_back(nTRTHoles);    
        m_numberOfTRTHTHits             -> push_back(nTRTHTHits);    
        m_numberOfTRTHTOutliers         -> push_back(nTRTHTOutliers);    
        m_numberOfTRTDeadStraws         -> push_back(nTRTDeadStraw);    
        m_numberOfTRTTubeHits           -> push_back(nTRTTubeHits);    

        m_d0                            -> push_back(eld0);
        m_d0Err                         -> push_back(eld0Err);
        m_z0                            -> push_back(elz0);
        m_z0Err                         -> push_back(elz0Err);
        
        m_e237                          -> push_back(e237);    
        m_e277                          -> push_back(e277);   
        m_ethad                         -> push_back(ethad);
        m_ethad1                        -> push_back(ethad1); 
        m_emax                          -> push_back(emax); 
        m_emax2                         -> push_back(emax2); 
        m_emin                          -> push_back(emin); 
     
        m_isBMatch                      -> push_back(isBMatch);
        m_isCMatch                      -> push_back(isCMatch);
        m_isZMatch                      -> push_back(isZMatch);
        m_isAuthor                      -> push_back(isAuthor);
        m_isAuthorSofte                 -> push_back(isAuthorSofte);
     
        m_pt                            -> push_back(elClPt);
        m_eta                           -> push_back(elClEta);
        m_phi                           -> push_back(elClPhi);
        m_et                            -> push_back(trnsE);

        m_f1                            -> push_back(f1);                          
        m_f1core                        -> push_back(f1core);
        m_emins1                        -> push_back(emins1);
        m_fracs1                        -> push_back(fracs1);
        m_e2tsts1                       -> push_back(e2tsts1);
        m_weta1                         -> push_back(weta1);
        m_wtots1                        -> push_back(wtots1);
        m_emaxs1                        -> push_back(emaxs1);
        m_e233                          -> push_back(e233);
        m_e237                          -> push_back(e237);
        m_e277                          -> push_back(e277);
        m_weta2                         -> push_back(weta2);
        m_f3                            -> push_back(f3);
        m_f3core                        -> push_back(f3core);
        m_etcone                        -> push_back(etcone);
        m_etcone20                      -> push_back(etcone20);
        m_etcone30                      -> push_back(etcone30); 
        m_etcone40                      -> push_back(etcone40);
        m_ptcone30                      -> push_back(ptcone30);
        m_deltaEta1                     -> push_back(deltaEta1);
        m_deltaEta2                     -> push_back(deltaEta2);
        m_deltaPhi2                     -> push_back(deltaPhi2);
        m_deltaPhiRescaled              -> push_back(deltaPhiRescaled);
        m_expectHitInBLayer             -> push_back(expectHitInBLayer);
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
        mlog << MSG::ERROR <<"could not retrive PrimaryVertex container" <<endreq;
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
    m_runNumber     = 0;
    m_lumiNumber    = 0;

    m_numberOfbLayerHits            = new std::vector <float>();
    m_numberOfbLayerOutliers        = new std::vector <float>(); 
    m_numberOfbLayerSharedHits      = new std::vector <float>();
    
    m_numberOfPixelHits             = new std::vector <float>();
    m_numberOfPixelOutliers         = new std::vector <float>();
    m_numberOfPixelHoles            = new std::vector <float>();
    m_numberOfPixelSharedHits       = new std::vector <float>();
    m_numberOfGangedPixels          = new std::vector <float>();
    m_numberOfGangedFlaggedFakes    = new std::vector <float>();
    m_numberOfPixelDeadSensors      = new std::vector <float>();
    m_numberOfPixelSpoiltHits       = new std::vector <float>();

    
    m_numberOfSCTHits               = new std::vector <float>();
    m_numberOfSCTOutliers           = new std::vector <float>();
    m_numberOfSCTHoles              = new std::vector <float>(); 
    m_numberOfSCTDoubleHoles        = new std::vector <float>();    
    m_numberOfSCTSharedHits         = new std::vector <float>();    
    m_numberOfSCTDeadSensors        = new std::vector <float>();    
    m_numberOfSCTSpoiltHits         = new std::vector <float>();    

    m_d0                            = new std::vector <float>();
    m_d0Err                         = new std::vector <float>();
    m_z0                            = new std::vector <float>();
    m_z0Err                         = new std::vector <float>();

    m_numberOfTRTHits               = new std::vector <float>();    
    m_numberOfTRTOutliers           = new std::vector <float>();    
    m_numberOfTRTHoles              = new std::vector <float>();    
    m_numberOfTRTHTHits             = new std::vector <float>();    
    m_numberOfTRTHTOutliers         = new std::vector <float>();    
    m_numberOfTRTDeadStraws         = new std::vector <float>();    
    m_numberOfTRTTubeHits           = new std::vector <float>();    
    
    m_cone30                        = new std::vector <float>();
    m_ethad                         = new std::vector <float>();
    m_ethad1                        = new std::vector <float>(); 
    m_emax                          = new std::vector <float>(); 
    m_emax2                         = new std::vector <float>(); 
    m_emin                          = new std::vector <float>(); 
 
    m_isBMatch                      = new std::vector <bool>(); 
    m_isCMatch                      = new std::vector <bool>(); 
    m_isZMatch                      = new std::vector <bool>(); 
    m_isAuthor                      = new std::vector <bool>(); 
    m_isAuthorSofte                 = new std::vector <bool>(); 
 
    m_pt                            = new std::vector <float>(); 
    m_eta                           = new std::vector <float>(); 
    m_phi                           = new std::vector <float>(); 
    m_et                            = new std::vector <float>();

    m_f1                            = new std::vector <float>();
    m_f1core                        = new std::vector <float>();
    m_emins1                        = new std::vector <float>();
    m_fracs1                        = new std::vector <float>(); 
    m_e2tsts1                       = new std::vector <float>(); 
    m_weta1                         = new std::vector <float>(); 
    m_wtots1                        = new std::vector <float>(); 
    m_emaxs1                        = new std::vector <float>(); 
    m_e233                          = new std::vector <float>(); 
    m_e237                          = new std::vector <float>();    
    m_e277                          = new std::vector <float>();   
    m_weta2                         = new std::vector <float>(); 
    m_f3                            = new std::vector <float>(); 
    m_f3core                        = new std::vector <float>(); 
    m_etcone                        = new std::vector <float>(); 
    m_etcone20                      = new std::vector <float>(); 
    m_etcone30                      = new std::vector <float>(); 
    m_etcone40                      = new std::vector <float>(); 
    m_ptcone30                      = new std::vector <float>(); 
    m_deltaEta1                     = new std::vector <float>(); 
    m_deltaEta2                     = new std::vector <float>(); 
    m_deltaPhi2                     = new std::vector <float>(); 
    m_deltaPhiRescaled              = new std::vector <float>(); 
    m_expectHitInBLayer             = new std::vector <bool>(); 

}

void SoftElectron::ClearContainers()
{
    delete m_numberOfbLayerHits        ;
    delete m_numberOfbLayerOutliers    ; 
    delete m_numberOfbLayerSharedHits  ;
    delete m_numberOfPixelHits         ;
    delete m_numberOfPixelOutliers     ;
    delete m_numberOfPixelHoles        ;
    delete m_numberOfPixelSharedHits   ;
    delete m_numberOfGangedPixels      ;
    delete m_numberOfGangedFlaggedFakes;
    delete m_numberOfPixelDeadSensors  ;
    delete m_numberOfPixelSpoiltHits   ;

    delete m_numberOfSCTHits           ;
    delete m_numberOfSCTOutliers       ;
    delete m_numberOfSCTHoles          ;
    delete m_numberOfSCTDoubleHoles    ;
    delete m_numberOfSCTSharedHits     ;
    delete m_numberOfSCTDeadSensors    ;
    delete m_numberOfSCTSpoiltHits     ;
    
    delete m_d0                        ;
    delete m_d0Err                     ;
    delete m_z0                        ;
    delete m_z0Err                     ;

    delete m_numberOfTRTHits           ;
    delete m_numberOfTRTOutliers       ;
    delete m_numberOfTRTHoles          ;
    delete m_numberOfTRTHTHits         ;
    delete m_numberOfTRTHTOutliers     ;
    delete m_numberOfTRTDeadStraws     ;
    delete m_numberOfTRTTubeHits       ;
    delete m_cone30                    ;
    delete m_ethad                     ;
    delete m_ethad1                    ;
    delete m_emax                      ;
    delete m_emax2                     ;
    delete m_emin                      ;
 
    delete m_isBMatch                  ;
    delete m_isCMatch                  ;
    delete m_isZMatch                  ;
    delete m_isAuthor                  ;
    delete m_isAuthorSofte             ;
 
    delete m_pt                        ;
    delete m_eta                       ;
    delete m_phi                       ;
    delete m_et                        ;

    delete m_f1                        ;                          
    delete m_f1core                    ;    
    delete m_emins1                    ;    
    delete m_fracs1                    ;    
    delete m_e2tsts1                   ;    
    delete m_weta1                     ;    
    delete m_wtots1                    ;    
    delete m_emaxs1                    ;    
    delete m_e233                      ;    
    delete m_e237                      ;    
    delete m_e277                      ;    
    delete m_weta2                     ;    
    delete m_f3                        ;    
    delete m_f3core                    ;    
    delete m_etcone                    ;    
    delete m_etcone20                  ;    
    delete m_etcone30                  ;    
    delete m_etcone40                  ;    
    delete m_ptcone30                  ;    
    delete m_deltaEta1                 ;    
    delete m_deltaEta2                 ;    
    delete m_deltaPhi2                 ;    
    delete m_deltaPhiRescaled          ;    
    delete m_expectHitInBLayer         ;    
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
    const HepMC::GenParticle* theMother(0);

    if(!partOriVert) return theMother;

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
            theMother   = (*itrMother); 
            if(MotherPDG==partPDG) break;
        }
        itr++;
        if(itr>100)
        {
            std::cout <<"getMother:: infinite while" << std::endl; 
            break;
        }
    }while (MothOriVert !=0 &&MotherPDG==partPDG && partBarcode<200000);

    return theMother;
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
