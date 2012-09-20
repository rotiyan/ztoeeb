#include "ZeeB/SoftElectron.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ITHistSvc.h"

//ROOT includes
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TTree.h"
#include "TMath.h"
#include "TVector3.h"

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

SoftElectron::~SoftElectron() {}

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

        this->ClearContainers(); 
    }
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
    m_h3_histMap.insert(std::pair<std::string,TH3F*>( "h3_bMatch", new TH3F("bMatch_trtHits","B electrons TRT hits;TRThits_tot;TRThits_ht; TRThits_lt",50,-0.5,45.5,50,-0.5,45.5,50,-0.5,45.5)));
    m_h3_histMap.insert(std::pair<std::string,TH3F*>( "h3_cMatch", new TH3F("cMatch_trtHits","C electrons TRT hits;TRThits_tot;TRThits_ht; TRThits_lt",50,-0.5,45.5,50,-0.5,45.5,50,-0.5,45.5)));
    m_h3_histMap.insert(std::pair<std::string,TH3F*>( "h3_unMatch",new TH3F("unMatch_trtHits","Unmatched  electrons TRT hits;TRThits_tot;TRThits_ht; TRThits_lt",50,-0.5,45.5,50,-0.5,45.5,50,-0.5,45.5)));

    //register TH1F
    for(std::map<std::string,TH1F*>::iterator iter = m_h1_histMap.begin(); iter != m_h1_histMap.end(); ++iter)
    {
        sc = m_histos->regHist("/AANT/hist/"+iter->first,iter->second);
        if (sc.isFailure())
        {
            return sc;
        }
    }

    //register TH2F
    for(std::map<std::string,TH2F*>::iterator iter = m_h2_histMap.begin(); iter != m_h2_histMap.end(); ++iter)
    {
        sc = m_histos->regHist("/AANT/"+iter->first,iter->second);
        if(sc.isFailure())
        {
            return sc;
        }
    }

    //register TH3F
    for(std::map<std::string,TH3F*>::iterator iter = m_h3_histMap.begin(); iter != m_h3_histMap.end(); ++iter)
    {
        sc = m_histos->regHist("/AANT/"+iter->first,iter->second);
        if(sc.isFailure())
        {
            return sc;
        }
    }
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

void SoftElectron::FillElectrons()
{
    MsgStream mlog(msgSvc(), name());
    std::vector<const Analysis::Electron*> SelectdElectrons;
    
    for(unsigned int i = 0; i < m_electronCollection->size(); ++i)
    {
        const Analysis::Electron* Electron = m_electronCollection->at(i);
        if((Electron->author(egammaParameters::AuthorSofte) && Electron->author(egammaParameters::AuthorElectron)) || Electron->author(egammaParameters::AuthorElectron))
        {
            m_mcTruthClassifier->particleTruthClassifier(Electron);
            const HepMC::GenParticle* elParent    = m_mcTruthClassifier->getMother();
            const CaloCluster* ElCluster = Electron->cluster();

            if(ElCluster && elParent)
            {
                double elClPt       = ElCluster->pt()/1000;
                double elClEta      = ElCluster->eta();
                double elClPhi      = ElCluster->phi();
                double elClE        = ElCluster->e()/1000;

                double elTrkEta     = -100;
                double elTrkPhi     = -100;

                if(Electron->trackParticle())
                {
                    double elTrkEta     = Electron->trackParticle()->eta();
                    double elTrkPhi     = Electron->trackParticle()->phi();
                }

                double elCharge     = Electron->charge();
                bool isEmLoose      = Electron->passID(egammaPID::ElectronIDLoose);
                bool isEmLoosePP    = Electron->passID(egammaPID::ElectronIDLoosePP);
                bool isEmMedium     = Electron->passID(egammaPID::ElectronIDMedium);
                bool isEmMediumPP   = Electron->passID(egammaPID::ElectronIDMediumPP);
                bool isEmTight      = Electron->passID(egammaPID::ElectronIDTight);
                bool isEmTightPP    = Electron->passID(egammaPID::ElectronIDTightPP);


                
                bool isGoodOQ       = Electron->isgoodoq(egammaPID::BADCLUSELECTRON) ==0 ? true: false;


                if( isGoodOQ && 
                        (!(std::abs(elClEta) < m_elCrackEtaCutHigh && std::abs(elClEta) > m_elCrackEtaCutLow)) &&
                        std::abs(elClEta) < m_elEtaCut && elClPt > m_elPtCut )
                {
                    const Rec::TrackParticle* elTrack = Electron->trackParticle();
                    if(elTrack)
                    {
                        const Trk::TrackSummary* ElSummary = elTrack->trackSummary();
                        int trtHits     = ElSummary->get(Trk::numberOfTRTHits);
                        int trtHtHits   = ElSummary->get(Trk::numberOfTRTHighThresholdHits);
                        int trtLtHits   = std::abs(trtHtHits - trtHtHits);

                        m_h3_histMap.find("h3_unMatch")->second->Fill(trtHits,trtHtHits,trtLtHits);
                        
                        if(this->isBHadron(elParent))
                        {
                            mlog <<MSG::INFO<< "INSIDE BFill " <<endreq;
                            m_h3_histMap.find("h3_bMatch")->second->Fill(trtHits,trtHtHits,trtLtHits);
                        }
                        if(this->isCHadron(elParent))
                        {
                            mlog <<MSG::INFO<< "INSIDE CFill " <<endreq;
                            m_h3_histMap.find("h3_cMatch")->second->Fill(trtHits,trtHtHits,trtLtHits);
                        }
                    }
                }
            }
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
}

void SoftElectron::ClearContainers()
{

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
