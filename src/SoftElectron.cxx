#include "ZeeB/SoftElectron.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TMath.h"

#include "McParticleEvent/TruthParticleContainer.h"
#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#include "JetEvent/JetTagInfoBase.h"
#include "JetTagEvent/TrackAssociation.h"
#include "Particle/TrackParticleContainer.h"
#include "ParticleJetTools/IJetTruthMatching.h"
#include "ParticleJetTools/JetQuarkLabel.h"


#include "Navigation/NavigationToken.h"
#include "MuonIDEvent/MuonAssociation.h"
#include "egammaEvent/ElectronAssociation.h"


#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"

/// the Electron
#include "egammaEvent/egamma.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"


/// The Muon
#include "muonEvent/Muon.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticle/TrigEFBjetContainer.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TruthUtils/IHforTool.h"

#include <map>
#include <algorithm>

SoftElectron::SoftElectron(const std::string& name, ISvcLocator* pSvcLocator) 
    : Algorithm(name, pSvcLocator),
    m_trackTES(0),
    m_eventInfo(0),
    m_mcEventCollection(0),
    m_vxContainer(0),
    m_jetCollection(0),
    m_electronCollection(0),
    m_histos(0),
    m_jetTruthMatchTool("Analysis::JetQuarkLabel")
{
  declareProperty("JetContainer", m_particleJetContainerName = "AntiKt4TopoEMJets");
  declareProperty("MCParticleContainer",m_truthParticleContainerName = "SpclMC");
  declareProperty("TrackParticleContainer", m_trackParticleContainerName = "TrackParticleCandidate");
  declareProperty("MuonContainer",m_muonContainerName = "StacoMuonCollection");
  declareProperty("ElectronContainer", m_electronContainerName = "ElectronAODCollection");
  declareProperty("PrimaryVertexContainer", m_primaryVertexContainerName = "VxPrimaryCandidate");
  declareProperty("MCEventContainer", m_mcEventContainerName = "GEN_AOD");
  declareProperty("SelectBFromRun", m_selectBFromRun = 0);
  declareProperty("SelectUFromRun", m_selectUFromRun = 0);
  declareProperty("PurificationDeltaR", m_purificationDeltaR = 0.8);
  declareProperty("LeptonPurificationDeltaR", m_leptonPurificationDeltaR = 0.7);
  declareProperty("JetTruthMatchTool",m_jetTruthMatchTool);
}

SoftElectron::~SoftElectron() {}

StatusCode SoftElectron::initialize() 
{
    MsgStream mlog( messageService(), name() );
    StatusCode sc = service("StoreGateSvc", m_storeGate);
    
    hfor_tool = ToolHandle<IHforTool>("HforTool/hforTool", this);
    if(hfor_tool.retrieve().isFailure()){
        mlog <<MSG::ERROR 
                << ": Failed to retrieve tool "
                <<endreq;
        return StatusCode::FAILURE;
    }


    if (sc.isFailure()) 
    {
        mlog << MSG::ERROR
            << "Unable to retrieve pointer to StoreGateSvc"
            << endreq;
        return sc;
    }

    // THistSvc init:
    sc = service("THistSvc", m_histos, true);
    if ( sc.isFailure() )
    {
        mlog << MSG::WARNING << ">>> Unable to locate the Histogram service" << endreq;
    }
    
    //Jet Truth match tool 
    if ( !m_jetTruthMatchTool.empty() )
    {
        if ( m_jetTruthMatchTool.retrieve().isFailure() ) 
        {
            mlog << MSG::FATAL 
                << "#SoftElectron# Failed to retrieve tool " << m_jetTruthMatchTool << endreq;
            return StatusCode::FAILURE;
        }
    }
    // histograms:
    this->BookHistograms();
    return StatusCode::SUCCESS;
}		 

double phiCorr(double phi)
{
  double newphi = phi;
  if (phi<-M_PI) newphi += 2*M_PI;
  if (phi> M_PI) newphi -= 2*M_PI;
  return newphi;
}

StatusCode SoftElectron::finalize() 
{
  MsgStream mlog( messageService(), name() );
  return StatusCode::SUCCESS;
}

StatusCode SoftElectron::execute() 
{
    MsgStream mlog( messageService(), name() );
    mlog << MSG::INFO<< "SoftElectron::execute()" << endreq;
    StatusCode sc = StatusCode::SUCCESS;

    /** Testing HFOR **/
    /*if ( hfor_tool->execute().isSuccess() )
    {
        std::string hfor_type = hfor_tool->getDecision();
        mlog <<MSG::INFO<<"Hfortool Decision: "<<hfor_type <<endreq;
        mlog <<MSG::INFO<<"BQuarks ME: "<<hfor_tool->get_bQuarks_ME().size() <<endreq;
        mlog <<MSG::INFO<<"BQuarks GS: "<<hfor_tool->get_bQuarks_GS().size() <<endreq;
        mlog <<MSG::INFO<<"BQuarks MEin: "<<hfor_tool->get_bQuarks_MEin().size() <<endreq;
        mlog <<MSG::INFO<<"CQuarks ME:"<<hfor_tool->get_cQuarks_ME().size() <<endreq;
        mlog <<MSG::INFO<<"CQuarks GS:"<<hfor_tool->get_cQuarks_GS().size() <<endreq;
        mlog <<MSG::INFO<<"CQuarks MEin:"<<hfor_tool->get_cQuarks_MEin().size() <<endreq;
    }*/


    
    /** Retrieve Event header: */
    sc = this->LoadContainers();
    if (sc.isFailure())
    {
        mlog << MSG::FATAL <<"Could not load containers" <<endreq;
        return StatusCode::FAILURE;
    }
    m_irun = 0;
    m_ievt = 0;

    EventID* myEventID = m_eventInfo->event_ID();
    if(myEventID) 
    {
        m_irun = myEventID->run_number();  
        m_ievt = myEventID->event_number();
    }
    
    mlog << MSG::DEBUG << "--- Run " << m_irun << " Event " << m_ievt <<endreq;

    /** True primary vertex: */
    double xpvxt = 0.;
    double ypvxt = 0.;
    double zpvxt = 0.;
    
    const HepMC::GenEvent* GenEvent = *(m_mcEventCollection->begin());
    if(GenEvent) 
    {
        /** Primary Vertex **/
        HepMC::GenEvent::vertex_const_iterator vitr = GenEvent->vertices_begin();
        xpvxt = (*vitr)->position().x();
        ypvxt = (*vitr)->position().y();
        zpvxt = (*vitr)->position().z(); 
    

        // fill some information about b and c particles:
        const HepMC::GenEvent * GenEvent = *(m_mcEventCollection->begin());


        std::vector<const HepMC::GenParticle*> MEPartonVector;
        std::vector<const HepMC::GenParticle*> bcpartVector;

        HepMC::GenEvent::particle_const_iterator pitr = GenEvent->particles_begin();
        for (; pitr != GenEvent->particles_end(); ++pitr) 
        {
            int pdg = (*pitr)->pdg_id();
            int apdg = std::abs(pdg) ;
            if (apdg == 5 || apdg == 4) 
            {
                // b or c quark
                const HepMC::GenParticle * bcpart = (*pitr) ;
                mlog<<MSG::INFO<<"pdg = " << pdg << ": " << *bcpart <<endreq;
                // find the production vertex and parents
                HepMC::GenVertex * prodvtx = bcpart->production_vertex() ;
                bool hasbchadronparent(false) ;
                bool hasmpiparent(false) ;
                bool hastopparent(false) ;
                bool hasWparent(false) ;
                if ( prodvtx ) 
                {
                    mlog<<MSG::DEBUG<<"  prod vtx: " << *prodvtx <<endreq;
                    // check that there is no b/c-hadron as a parent
                    // also find mpi and top parents
                    HepMC::GenVertex::particle_iterator pin =  prodvtx->particles_begin(HepMC::parents);
                    for (; pin != prodvtx->particles_end(HepMC::parents) && !hasbchadronparent; pin++) 
                    {
                        mlog<<MSG::DEBUG<<"    incoming: " << *(*pin) <<endreq;
                        int pdgin(abs((*pin)->pdg_id())) ;
      
                        if ( (pdgin%10000)/1000 == apdg || (pdgin%1000)/100 == apdg ) 
                        hasbchadronparent = true ;
                        // also reject the c-quarks from a b-quark/hadron decay
                        if ( apdg == 4 && ( pdgin == 5 || (pdgin%10000)/1000 == 5 ||  (pdgin%1000)/100 == 5 ) )
                            hasbchadronparent = true ;
                        if ( pdgin == 0 && (*pin)->status() == 120 )
                            hasmpiparent = true ;
                        if ( pdgin == 6 ) 
                        {  
                            hastopparent = true ;
                            mlog<<MSG::DEBUG<<"  b/c parton with a top parent"<<endreq;
                        }
                        if ( pdgin == 24 ) 
                        {
                            hasWparent = true ;
                            mlog <<MSG::DEBUG <<"  b/c parton with a W parent" <<endreq;
                        }
                    }
                } 
                else 
                {
                    mlog<<MSG::DEBUG<<"  b/c parton without production vertex"<<endreq;
                }
                if ( hasbchadronparent )
                    mlog<<MSG::DEBUG<<"  b/c parton with a b/c quark/hadron parent"<<endreq;
                // find the decay vertex and children
                HepMC::GenVertex * decayvtx = bcpart->end_vertex();
                bool hasbcquarkdaughter(false) ;
                if ( !hasbchadronparent && decayvtx ) 
                {
                    mlog<<MSG::DEBUG<<"  decay vtx: " << *decayvtx  <<endreq;
                    // check whether there are only non-b/c-quark daughters
                    HepMC::GenVertex::particle_iterator pout =    decayvtx->particles_begin(HepMC::children) ;
                    for (; pout != decayvtx->particles_end(HepMC::children) && !hasbcquarkdaughter; pout++) 
                    {
                        mlog<<MSG::DEBUG <<"    outgoing: " << *(*pout) <<endreq;
                        int pdgout(abs((*pout)->pdg_id())) ;
                        if ( pdgout == apdg )
                            hasbcquarkdaughter = true ;
                    }
                }
                else if ( !decayvtx ) 
                {
                    mlog<<MSG::DEBUG <<"  b/c parton without decay vertex" <<endreq;
                }
                if (!hasbchadronparent && !hasbcquarkdaughter) 
                {
                    bcpartVector.push_back(bcpart);
                    if(bcpart->status()==123 or bcpart->status()==124)
                    {
                        mlog<<MSG::INFO <<"ME PARTON IN: "<< bcpart->pdg_id() <<endreq;
                    }
                }
            }
        }
        for(std::vector<const HepMC::GenParticle*>::const_iterator iter = bcpartVector.begin(); iter != bcpartVector.end(); ++iter)
        {
            const HepMC::GenParticle* bcpart = *iter;
            int apdg = bcpart->pdg_id();
            
            HepMC::GenVertex * prodvtx(bcpart->production_vertex()) ;

            bool isMPI(false) ;
            bool isGS(false); 
            bool isME(false) ; 
            bool isPDF(false) ; 
            bool isTopDecay(false) ; 
            bool isWDecay(false) ; // subset of top-decays, for hadronic top-decays
            bool iscquarkfromb(false) ;

           // if the final state quark is a PDF parton, ignore it 
           //(in AOD, descendants of these partons may be filtered out)
           if ( bcpart->status() == 141 || bcpart->status() == 142 ) 
           {
               isPDF = true ;
           }
           if ( !isPDF && prodvtx ) 
           {           
                //Loop throught the ancestors 
                HepMC::GenVertex::particle_iterator pin = prodvtx->particles_begin(HepMC::ancestors) ;
                for(; pin != prodvtx->particles_end(HepMC::ancestors) && !iscquarkfromb && !isPDF; ++pin)
                {
                    int apdgin = std::abs((*pin)->pdg_id()) ;
                    if (apdgin != apdg ) 
                    {
                        if ( (*pin)->status() == 121 || (*pin)->status() == 122 ) 
                        {
                            isGS = true ;
                        }
                        if ( apdgin == 0 && (*pin)->status() == 120 ) 
                        {
                            isMPI = true ;
                        }
                        // c quark from a b quark (in b-hadron decays)
                        if ( apdg == 4 && ( apdgin == 5 || (apdgin%10000)/1000 == 5 || (apdgin%1000)/100 == 5 ) ) 
                        {
                            mlog<<MSG::DEBUG <<"  c quark from b quark or b hadron" <<endreq;
                            iscquarkfromb = true ;
                        }
                        // b quark from a b-hadron decay 
                        //(b directly from b-hadron already rejected)
                        if ( apdg == 5 && ( (apdgin%10000)/1000 == 5 || (apdgin%1000)/100 == 5 ) ) 
                        {
                            mlog<<MSG::DEBUG<<"  b quark from b hadron" <<endreq;
                            iscquarkfromb = true ;
                        }
                    }
                    else
                    {
                        if((*pin)->status() == 123 || (*pin)->status() == 124 ) 
                        {
                            isME = true ;
                        }
                        if ( (*pin)->status() == 141 || (*pin)->status() == 142 ) 
                        {
                            isPDF = true ;
                        }
                    }
                }
           }
           if(!iscquarkfromb && !isPDF)
           {
               if(isME)
               {
                   mlog<<MSG::DEBUG<< "  ME !!" <<endreq;
                   mlog <<MSG::INFO << "ME Partons:" 
                       << "pdgID: " << bcpart->pdg_id() 
                       << "status: "<< bcpart->status() 
                       <<endreq;
               }
           }
        }
    }
    return sc;
}
// ============================================================
StatusCode SoftElectron::checkTrackqualforSET(Rec::TrackParticleContainer::const_iterator trackItr, double *trackpt) 
{
  //
  // check track quality for soft electrons
  // Beware that requirements could be slightly different 
  // than for other taggers (in particular for TRT)
  //
  MsgStream mlog( messageService(), name() );

  // good quality track cuts
  double cutA0       = 1;
  int cutBL          = 1;
  int cutPi          = 2;
  int cutSi          = 7;
  int cutNTRHits     = 1;
  int cutNTRTHits    = 20;
  double cutPt       = 2000.;

  int nBL   = 0;
  int nPix  = 0;
  int nSi   = 0;
  int nTR   = 0;
  int nTRT  = 0; 
    
  const Trk::TrackSummary* summary = (*trackItr)->trackSummary();
  if (summary) {
    // number of b-layer hits
    nBL = summary->get(Trk::numberOfBLayerHits);
    // number of pixel hits
    nPix = summary->get(Trk::numberOfPixelHits);
    // number of Si hits
    nSi = summary->get(Trk::numberOfPixelHits) + 
      summary->get(Trk::numberOfSCTHits);
    // number of high threshold hits in TRT (with outliers)
    nTR = summary->get(Trk::numberOfTRTHighThresholdHits)+
      summary->get(Trk::numberOfTRTHighThresholdOutliers);
    // number of TRT hits (with outliers)
    nTRT = summary->get(Trk::numberOfTRTHits)+
    summary->get(Trk::numberOfTRTOutliers);
  }
    
  // access to the perigee parameters
  const Trk::MeasuredPerigee* perigee = (*trackItr)->measuredPerigee();
    
  double A0Vert     = 0.;
  double PTInvVert  = 0.;
    
  if (perigee) 
  {
      HepVector parameters = perigee->parameters();
      // transverse impact parameter
      A0Vert  = parameters[Trk::d0];
      // pT of the track
      PTInvVert = parameters[Trk::qOverP];
      if ( PTInvVert != 0. ) 
      {
          *trackpt = 1./PTInvVert;
      } 
      else { *trackpt = 0.;}
  }
  // basic cuts on good quality tracks
  if (   A0Vert   <= cutA0       &&
          nBL      >= cutBL       &&
          nPix     >= cutPi       &&
          nSi      >= cutSi       &&
          nTR      >= cutNTRHits  &&
          nTRT     >= cutNTRTHits &&
          fabs(*trackpt) > cutPt      ) 
      return StatusCode::SUCCESS; 
  
  return StatusCode::FAILURE;  
}

void SoftElectron::BookHistograms() 
{
    MsgStream mlog( messageService(), name() );

    mlog<<MSG::INFO<<"Booking histograms" <<endreq;

    h1Hists["h1_nEvents"]     = new TH1F("nEvents","",4,0,4);
    h1Hists["h1_nBHadron"]    = new TH1F("nBHadron","",4,0,4);
    h1Hists["h1_nCHadron"]    = new TH1F("nCHadron","",4,0,4);
    h1Hists["h1_nBHadronSemilept"]    = new TH1F("nBHadronSemilept","",4,0,4);
    h1Hists["h1_nCHadronSemilept"]    = new TH1F("nCHadronSemilept","",4,0,4);
    
    h2Hists["h2_ElHardVsSoft"]= new TH2F("ElHardVsSoft",";# Hard el; #Soft el",100,0,50,100,0,50);
    h2Hists["h2_BMatching"]   = new TH2F("BMatching",";|p|[GeV];#Delta R",500,0,500,100,0,3);
    h2Hists["h2_CMatching"]   = new TH2F("CMatching",";|p|[GeV];#Delta R",500,0,500,100,0,3);
    
    h1Hists["h1_MEbQuarkDr"]  = new TH1F("MEbQuarkDeltaR","",100,0,3);
    h1Hists["h1_MEcQuarkDr"]  = new TH1F("MEcQuarkDeltaR","",100,0,3);
    h1Hists["h1_ElPt"]        = new TH1F("ElPt",";[GeV]",500,0,500);
    h1Hists["h1_ElEta"]       = new TH1F("ElEta","",200,-5,5);
    h1Hists["h1_ElPhi"]       = new TH1F("ElPhi","",200,-5,5);
    h1Hists["h1_BHadElPt"]    = new TH1F("bHadronElPt",";[GeV]",500,0,500);
    h1Hists["h1_CHadElPt"]    = new TH1F("cHadronElPt",";[GeV]",500,0,500);

    for(std::map<std::string,TH1F*>::iterator iter = h1Hists.begin(); iter != h1Hists.end(); ++iter)
        iter->second->Sumw2();

    for(std::map<std::string,TH2F*>::iterator iter = h2Hists.begin(); iter != h2Hists.end(); ++iter)
        iter->second->Sumw2();
    
}

bool SoftElectron::isBHadron(int pdg) 
{
    int mpdg = abs(pdg);
    return (   ( 500 < mpdg && mpdg < 599 )   ||
            ( 10500 < mpdg && mpdg < 10599 ) ||
            (  5000 < mpdg && mpdg < 5999  ) ||
            ( 20500 < mpdg && mpdg < 20599 ) );
}

bool SoftElectron::isDHadron(int pdg) 
{
    int mpdg = abs(pdg);
    return (   ( 400 < mpdg && mpdg < 499 )   || 
            ( 10400 < mpdg && mpdg < 10499 ) ||
            (  4000 < mpdg && mpdg < 4999  ) ||
            ( 20400 < mpdg && mpdg < 20499 ) );
}

bool SoftElectron::isLightJetIsolated(const HepLorentzVector& p) 
{
    if(0==m_mcpartTES) return false;
    TruthParticleContainer::const_iterator mcItr  = (*m_mcpartTES).begin();
    TruthParticleContainer::const_iterator mcEnd = (*m_mcpartTES).end();
    bool isol = true;
    for (; mcItr != mcEnd; ++mcItr) {
        if( p.deltaR( (*mcItr)->hlv() ) < m_purificationDeltaR ) {
            int mpdg = abs( (*mcItr)->pdgId() );
            if( this->isBHadron(mpdg) || 
                    this->isDHadron(mpdg) ||
                    5==mpdg ||
                    4==mpdg ||
                    15==mpdg ) {
                isol = false;
                break;
            }
        }
    }
    return isol;
}

// =========================================================

// ===============================================
const HepMC::GenParticle* SoftElectron::getTruth(const Rec::TrackParticle* myTrackParticle,
					      const TrackParticleTruthCollection* mcpartTES,
					      const Rec::TrackParticleContainer* trackTES)
{
  //this method returns pointer to truth particle linked to given track

 const HepMC::GenParticle* GenPart =0;

 ElementLink<Rec::TrackParticleContainer> trackPrtlink;
 trackPrtlink.setElement(const_cast<Rec::TrackParticle*>(myTrackParticle));
 trackPrtlink.setStorableObject(*trackTES);
 //typedef std::map<ElementLink<Rec::TrackParticleContainer>,TrackParticleTruth> truthMap;

 std::map<Rec::TrackParticleTruthKey,TrackParticleTruth>::const_iterator tempTrackPrtTruthItr = mcpartTES->find(trackPrtlink);

 //truthMap::const_iterator tempTrackPrtTruthItr;
 //tempTrackPrtTruthItr = mcpartTES->find(trackPrtlink);
 if (tempTrackPrtTruthItr != mcpartTES->end())
 {
   const HepMcParticleLink & temHepMcLink =
                           (*tempTrackPrtTruthItr).second.particleLink();
   long theBarcode=temHepMcLink.barcode();
   if (theBarcode!=0) GenPart = temHepMcLink.cptr();
 }
  return GenPart;
}

std::vector<const HepMC::GenParticle*> SoftElectron::GetMEPartons()
{
    std::vector<const HepMC::GenParticle*> MEPartonVector;

    return MEPartonVector;
}


////////////////////////////////////////////////////////////////////////
// Find the Heavy Flavour Quarks in this event
void SoftElectron::findHFQuarks()
////////////////////////////////////////////////////////////////////////
{
    MsgStream mlog( messageService(), name() );

  // Get the event / run number from StoreGate
  const EventInfo * currentEvent(NULL) ;
  StatusCode sc = m_storeGate->retrieve(currentEvent) ;
  if ( sc.isFailure() ) {
      mlog<<MSG::DEBUG<<"Couldnt retrieve EventInfo from StoreGateSvc"<<endreq;
    return ;
  }

  // Make sure that we haven't processed this event before
  EventType* eventType = currentEvent->event_type();
  EventID::number_type run_number(eventType->mc_channel_number()) ;
  //EventID::number_type run_number(currentEvent->event_ID()->run_number()) ;
  EventID::number_type event_number(currentEvent->event_ID()->event_number()) ;
  if ( run_number == m_prev_run_number && 
       event_number == m_prev_event_number )
    return ;

  mlog<<MSG::DEBUG<<"runnumber " << run_number << ", eventnumber " << event_number <<endreq;

  m_prev_run_number = run_number ;
  m_prev_event_number = event_number ;

  // clean up from previous event
  m_Quarks_MPI.clear() ;
  m_Quarks_GS.clear() ;
  m_Quarks_ME.clear() ;
  m_Quarks_MEin.clear() ;
  m_Quarks_unknown.clear() ;

  // vectors with the initial and final state b/c quarks; ie, initial or final 
  // in the parton shower; ignore b/c quarks from b/c-hadron decays
  std::map< int,std::vector<const HepMC::GenParticle*> > finalstate_q ;

  // get the GenEvent, assume it's the first one in the McEventCollection
  const HepMC::GenEvent* GenEvent = *(m_mcEventCollection->begin());
  HepMC::GenEvent::particle_const_iterator pitr = GenEvent->particles_begin();
  for (; pitr != GenEvent->particles_end(); ++pitr) {
    int pdg = (*pitr)->pdg_id();
    int apdg = std::abs(pdg) ;
    if (apdg == 5 || apdg == 4) { // b or c quark
      const HepMC::GenParticle * bcpart = (*pitr) ;
      mlog<<MSG::DEBUG<<"pdg = " << pdg << ": " << *bcpart<<endreq;

      // find the production vertex and parents
      HepMC::GenVertex * prodvtx = bcpart->production_vertex() ;
      bool hasbchadronparent(false) ;
      bool hasmpiparent(false) ;
      if ( prodvtx ) {
        mlog<<MSG::DEBUG<<"  prod vtx: " << *prodvtx<<endreq;

        // check that there is no b/c-hadron as a parent
        // also find mpi parent
        HepMC::GenVertex::particle_iterator pin = 
          prodvtx->particles_begin(HepMC::parents) ;
        for (; pin != prodvtx->particles_end(HepMC::parents) && !hasbchadronparent; pin++) {
          mlog<<MSG::DEBUG<<"    incoming: " << *(*pin)<<endreq;
          int pdgin(abs((*pin)->pdg_id())) ;

          if ( (pdgin%10000)/1000 == apdg || (pdgin%1000)/100 == apdg ) 
            hasbchadronparent = true ;
          // also reject the c-quarks from a b-quark/hadron decay
          if ( apdg == 4 && ( pdgin == 5 || (pdgin%10000)/1000 == 5 || 
                              (pdgin%1000)/100 == 5 ) )
            hasbchadronparent = true ;
          if ( pdgin == 0 && (*pin)->status() == 120 )
            hasmpiparent = true ;
        }
      } else {
        mlog<<MSG::DEBUG<<"  b/c parton without production vertex"<<endreq;
      }

      // find the decay vertex and children
      HepMC::GenVertex * decayvtx = bcpart->end_vertex() ;
      bool hasbcquarkdaughter(false) ;
      if ( decayvtx ) {
        mlog<<MSG::DEBUG<<"  decay vtx: " << *decayvtx<<endreq;
        
        // check whether there are only non-b/c-quark daughters
        HepMC::GenVertex::particle_iterator pout =
          decayvtx->particles_begin(HepMC::children) ;
        for (; pout != decayvtx->particles_end(HepMC::children) && !hasbcquarkdaughter; pout++) {
          mlog<<MSG::DEBUG<<"    outgoing: " << *(*pout)<<endreq;
          int pdgout(abs((*pout)->pdg_id())) ;
          if ( pdgout == apdg )
            hasbcquarkdaughter = true ;
        }
      } else {
        mlog<<MSG::DEBUG<<"  b/c parton without decay vertex"<<endreq;
      }
      
      // if no b/c-hadron parent and no b/c-quark daughter, keep it!
      if (!hasbchadronparent && !hasbcquarkdaughter) {
        mlog<<MSG::DEBUG<<"  final state b/c-quark, barcode = "<< bcpart->barcode()<<endreq;
        finalstate_q[apdg].push_back(bcpart) ;
      }

      // if no b/c-hadron parent, check it to see whether it comes from the ME
      // but ignore the ones with an MPI parent
      if (!hasbchadronparent && !hasmpiparent && 
          ( bcpart->status() == 123 || bcpart->status() == 124 ) ) {
        mlog<<MSG::DEBUG<<"  b/c-quark from ME"<<endreq;
        m_Quarks_MEin[apdg].push_back(bcpart) ;
      }

    } // particle is a b or a c quark
    
  } // loop over all particles in the GenEvent

  // loop over all the final state b/c-quarks and find out where they come from
  // first loop over quarks flavours that were stored (b,c)
  for ( std::map< int,std::vector<const HepMC::GenParticle*> >::const_iterator ipdg = finalstate_q.begin() ;
        ipdg != finalstate_q.end(); ipdg++ ) {
    int apdg(ipdg->first) ;
    msg<<MSG::DEBUG<<"looking for ancestors of pdg " << apdg<<endreq;

    // second loop over the final state quarks
    for ( std::vector<const HepMC::GenParticle*>::const_iterator ibcpart = finalstate_q[apdg].begin() ;
          ibcpart != finalstate_q[apdg].end(); ibcpart++ ) {
      const HepMC::GenParticle * bcpart(*(ibcpart)) ;
      mlog<<MSG::DEBUG<<"final state b/c " << *bcpart<<endreq;
      HepMC::GenVertex * prodvtx(bcpart->production_vertex()) ;
      bool isMPI(false) ;
      bool isGS(false) ;
      bool isME(false) ;
      bool isPDF(false) ;
      bool iscquarkfromb(false) ;
      // if the final state quark is a PDF parton, ignore it 
      // (in AOD, descendants of these partons may be filtered out)
      if ( bcpart->status() == 141 || bcpart->status() == 142 ) {
        mlog<<MSG::DEBUG<<"PDF !!"<<endreq;
        isPDF = true ;
      }
      if ( !isPDF && prodvtx ) {
        HepMC::GenVertex::particle_iterator pin = prodvtx->particles_begin(HepMC::ancestors) ;
        for ( ; pin != prodvtx->particles_end(HepMC::ancestors) && !iscquarkfromb && !isPDF ; pin++ ) {
          int apdgin = std::abs((*pin)->pdg_id()) ;
          if (apdgin != apdg ) {
            mlog<<MSG::DEBUG<<"  non b/c parent " << *(*pin)<<endreq;
            // if MPI as a non-b parent, label it
            if ( apdgin == 0 && (*pin)->status() == 120 ) {
              mlog<<MSG::DEBUG<<"  MPI !!"<<endreq;
              isMPI = true ;
            }
            // gluon splitting or ME origin: in evgen files, 
            // proton (id 2212) seems to be saved in all events; not so in 
            // AOD files... Thus look for non-HF origin with status 121 or 122
            if ( (*pin)->status() == 121 || (*pin)->status() == 122 ) {
              mlog<<MSG::DEBUG<<"  GS !!"<<endreq;
              isGS = true ;
            }     
            // c quark from a b quark (in b-hadron decays)
            if ( apdg == 4 && ( apdgin == 5 || (apdgin%10000)/1000 == 5 ||
                                (apdgin%1000)/100 == 5 ) ) {
              mlog<<MSG::DEBUG<<"  c quark from b quark"<<endreq;
              iscquarkfromb = true ;
            }
          } else {
            mlog<<MSG::DEBUG<<"  b/c parent " << *(*pin)<<endreq;
            // if the status of a b-quark is 123 or 124, then it is a ME b-quark
            if ( (*pin)->status() == 123 || (*pin)->status() == 124 ) {
              mlog<<MSG::DEBUG<<"  ME !!"<<endreq;
              isME = true ;
            }
            // if status 141 or 142 then it came from the PDF, ignore those!!
            if ( (*pin)->status() == 141 || (*pin)->status() == 142 ) {
              mlog<<MSG::DEBUG<<"  PDF !!"<<endreq;
              isPDF = true ;
            }
          } // b/c or non-b/c quark
        } // loop over all ancestors
      } // final state b/c-quark with a production vertex
      
      // MPI output is also status 123,124 so MPI comes before anything else
      // ME parents have status 121 or 122, so ME comes before GS
      if ( !iscquarkfromb && !isPDF ) {
        if ( isMPI )
          m_Quarks_MPI[apdg].push_back( bcpart) ;
        else if ( isME )
          m_Quarks_ME[apdg].push_back( bcpart ) ;
        else if ( isGS ) {
          // in AOD, incoming ME partons may look like GS partons if
          // their descendants are filtered out
          if ( ! (bcpart->status() == 123 || bcpart->status() == 124) )
            m_Quarks_GS[apdg].push_back( bcpart ) ;
          else
            mlog<<MSG::DEBUG<<"ME b/c-quark identified as GS"<<endreq;
        }
        else {
          mlog<<MSG::DEBUG<<"Unidentified b/c-quark"<<endreq;
          m_Quarks_unknown[apdg].push_back( bcpart-> ) ;
        }
      } // not a c-quark from a b decay or a PDF c-quark
      
    } // loop over final state b/c-quarks
  }   // loop over quark flavours

} // findHFQuarks()

StatusCode SoftElectron::LoadContainers()
{
    MsgStream mlog( messageService(), name() );

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
 
    sc=m_storeGate->retrieve(m_mcpartTES, m_truthParticleContainerName);
    if(sc.isFailure())
    {
        mlog << MSG::ERROR << "could not retrive TruthParticlecontainer" <<endreq;
        return sc;
    }
    
    sc=m_storeGate->retrieve( m_jetCollection, m_particleJetContainerName);
    if(sc.isFailure())
    {
        mlog << MSG::ERROR <<"could not retireve JetContainer " <<endreq;
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
