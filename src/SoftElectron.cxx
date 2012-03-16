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
    mlog << MSG::DEBUG << "SoftElectron::execute()" << endreq;
    StatusCode sc = StatusCode::SUCCESS;

    /** Testing HFOR **/
    if ( hfor_tool->execute().isSuccess() )
    {
        std::string hfor_type = hfor_tool->getDecision();
        mlog <<MSG::INFO<<"Hfortool Decision: "<<hfor_type <<endreq;
        mlog <<MSG::INFO<<"BQuarks ME: "<<hfor_tool->get_bQuarks_ME().size() <<endreq;
        mlog <<MSG::INFO<<"BQuarks GS: "<<hfor_tool->get_bQuarks_GS().size() <<endreq;
        mlog <<MSG::INFO<<"BQuarks MEin: "<<hfor_tool->get_bQuarks_MEin().size() <<endreq;
        mlog <<MSG::INFO<<"CQuarks ME:"<<hfor_tool->get_cQuarks_ME().size() <<endreq;
        mlog <<MSG::INFO<<"CQuarks GS:"<<hfor_tool->get_cQuarks_GS().size() <<endreq;
        mlog <<MSG::INFO<<"CQuarks MEin:"<<hfor_tool->get_cQuarks_MEin().size() <<endreq;
    }


    
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


    /** Primary vertex: */
    int npvx = m_vxContainer->size();
    VxContainer::const_iterator fz = m_vxContainer->begin();
    const Trk::RecVertex& primaryVertex = (*fz)->recVertex();
    double pvx = primaryVertex.position().x();
    double pvy = primaryVertex.position().y();
    double pvz = primaryVertex.position().z();
    
    m_h1_nvtx->Fill((float)npvx);
    m_h1_pvx_x->Fill(pvx);
    m_h1_pvx_y->Fill(pvy);
    m_h1_pvx_z->Fill(pvz);

    mlog << MSG::VERBOSE << "--- Primary vertex: " << pvx << " " << pvy << " " << pvz << endreq;
    // Check for undefined PV (when no PV is reconstructed, a dummy one is stored):
    if( pvx == 0 && pvy == 0 && pvz == 0 ) 
    {
        mlog << MSG::WARNING << "Primary Vertex is (0,0,0): skipping event." << endreq;
        return StatusCode::SUCCESS;
    }

    /** True primary vertex: */
    double xpvxt = 0.;
    double ypvxt = 0.;
    double zpvxt = 0.;
    
    const HepMC::GenEvent* genEvent = *(m_mcEventCollection->begin());
    if(genEvent) 
    {
        /** Primary Vertex **/
        HepMC::GenEvent::vertex_const_iterator vitr = genEvent->vertices_begin();
        xpvxt = (*vitr)->position().x();
        ypvxt = (*vitr)->position().y();
        zpvxt = (*vitr)->position().z(); 

        m_h1_primvtxresx->Fill(pvx-xpvxt);
        m_h1_primvtxresy->Fill(pvy-ypvxt);
        m_h1_primvtxresz->Fill(pvz-zpvxt);

        /** Particle **/
    }
    
    /** MC Truth container: */
    int nmcp = 0;
    nmcp = (*m_mcpartTES).size();
    
    // fill some information about b and c particles:
    TruthParticleContainer::const_iterator mcItr  = (*m_mcpartTES).begin();
    TruthParticleContainer::const_iterator mcEnd = (*m_mcpartTES).end();
    int nbbq = 0;
    int nbcq = 0;
    int nbbh = 0;
    int nbch = 0;
    for (; mcItr != mcEnd; ++mcItr) 
    {
        int mpdg = (*mcItr)->pdgId();    
        double pt = (*mcItr)->pt();
        double eta = (*mcItr)->eta();
        if(5==abs(mpdg)) 
        {
            nbbq++;
        }
        if(4==abs(mpdg)) 
        {
            nbcq++;
        }
        if(this->isBHadron(mpdg)) {
            nbbh++;
        }
        if(this->isDHadron(mpdg)) {
            nbch++;
        }
    }
 
    //The Signal.. 
    // The signal event requires atleast a single b-quark in the hard event (ME b-quark)
    int MEPartonBarcode= this->GetMEPartonBarcode();

    if(MEPartonBarcode)
    {
        /** Jet container: */
        int njtag = (*m_jetCollection).size();

        // Iterate of the electron container
        ElectronContainer::const_iterator elecItr  = m_electronCollection->begin();
        ElectronContainer::const_iterator elecItrE = m_electronCollection->end();

        int nElTot  = 0;
        int nElSoft = 0;
        int nElHard = 0;

        for(; elecItr != elecItrE; ++elecItr) 
        {
        }

        // --- iterate over the Jet container 
        JetCollection::const_iterator jetItr = (*m_jetCollection).begin();
        JetCollection::const_iterator jetEnd = (*m_jetCollection).end();
      
        int ntotal = 0;
        
        int nlabelb = 0;
        int nlabelc = 0;
        int nlabelt = 0;
        int icount =0;

  
        for (; jetItr != jetEnd; ++jetItr) 
        {
            HepLorentzVector p4( (*jetItr)->px(),(*jetItr)->py(),(*jetItr)->pz(),(*jetItr)->e());
            int ntag = (*jetItr)->jetTagInfoVector().size();
            ntotal++;
  
            //Jet TruthMatching 
            std::string label("N/A");
            const Analysis::TruthInfo* mcinfo = (*jetItr)->tagInfo<Analysis::TruthInfo>("TruthInfo");
            if(mcinfo) 
            {
                mlog <<MSG::INFO <<" Got MC INfo " << endreq;
                label = mcinfo->jetTruthLabel();
                if(label=="B")
                {
                    nlabelb++; }
                if(label=="C") {nlabelc++; }
                if(label=="T") {nlabelt++; }  
                // --- get jet basic kinematics:
                mlog << MSG::INFO << "BJet # " << icount << " Eta= " << p4.pseudoRapidity()
                    << " Phi= " << p4.phi() << " pT= " << p4.perp()
                    << "  #Tags= " << ntag << " MCLabel= " << label 
                    << " Barcode : "<< MEPartonBarcode 
                    << endreq;
            }
            else 
            {
                mlog << MSG::VERBOSE << "could not find TruthInfo for matching jet" << endreq;
            }
            icount++;
        } // end loop jets
        m_h1_nBjets->Fill(nlabelb);
        m_h1_nCjets->Fill(nlabelc);
        m_h1_nLightJets->Fill(ntotal - (nlabelb + nlabelc + nlabelt) );
    }
    return StatusCode::SUCCESS;
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

    m_h1_mcn                   = new TH1F("mcn","MonteCarlo mc_n distribution",20000,0,10000);
    m_h1_nvtx                  = new TH1F("nvtx","Number of primary vertices; nvtx",200,0,20);
    m_h1_pvx_x                 = new TH1F("pvx_x","Primary Vertex X; [mm]",200,-10,10);
    m_h1_pvx_y                 = new TH1F("pvx_y","Primary Vertex Y; [mm]",200,-10,10);
    m_h1_pvx_z                 = new TH1F("pvx_z","Primary Vertex Z; [mm]",200,-250,250);
    m_h1_primvtxresx           = new TH1F("pvx_xRes","Primary Vertex X Resolution; [mm]",200,-10,10);
    m_h1_primvtxresy           = new TH1F("pvx_yRes","Primary Vertex X Resolution; [mm]",200,-10,10);
    m_h1_primvtxresz           = new TH1F("pvx_zRes","Primary Vertex X Resolution; [mm]",200,-10,10);

    m_h1_nJets                 = new TH1F("nJets"," # jets",100,0,100);                                    
    m_h1_nBjets                = new TH1F("nBjets"," # b-jets",10,0,10);                                   
    m_h1_bJetPt                = new TH1F("bjetPt", " b-jet pt;[GeV]",1000,0,500);                         
    m_h1_bJetEta               = new TH1F("bjetEta", "#eta b-jet",1000,-5,5);                              
    m_h1_bJetPhi               = new TH1F("bjetPhi", " #phi b-jet",800,-4,4);                              
    m_h1_nCjets                = new TH1F("nCjets","# c-jets",10,0,10);                                    
    m_h1_cJetPt                = new TH1F("cjetPt", " b-jet pt;[GeV]",1000,0,500);                         
    m_h1_cJetEta               = new TH1F("cjetEta", "#eta b-jet",1000,-5,5);                              
    m_h1_cJetPhi               = new TH1F("cjetPhi", " #phi b-jet",800,-4,4);                              
    m_h1_nLightJets            = new TH1F("nLightJets","# l-jet",70,0,70);                                 
    m_h1_lJetPt                = new TH1F("ljetPt", " b-jet pt;[GeV]",1000,0,500);                         
    m_h1_lJetEta               = new TH1F("ljetEta", "#eta b-jet",1000,-5,5);                              
    m_h1_lJetPhi               = new TH1F("ljetPhi", " #phi b-jet",800,-4,4);                              

    m_h1_NElectrons            = new TH1F("NElectrons","# Electrons", 50,0,50);                            
    
    m_h1_nSoftEl               = new TH1F("nSoftEl","# soft el",10,0,10);                                  
    m_h1_SoftElPt              = new TH1F("SoftElPt"," Soft El pt; [GeV]",100,0,25);                       
    m_h1_SoftElEta             = new TH1F("SoftElEta"," Soft El Eta",1000,-5,5);                           
    m_h1_SoftElPhi             = new TH1F("SoftElPhi"," Soft El Phi",800,-4,4);                            
    m_h1_ThreeProngNSoftEl     = new TH1F("ThreeProngNSoftEl"," # soft el; (3 prong)", 10,0,10);           
    m_h1_ThreeProngSoftElPt    = new TH1F("ThreeProngSoftElPt"," Soft El pt;  [GeV] (3 prong)",100,0,25);  
    m_h1_ThreeProngSoftElEta   = new TH1F("ThreeProngSoftElEta"," SOft El Eta; (3 prong) ",1000,-5,5);     
    m_h1_ThreeProngSoftElPhi   = new TH1F("ThreeProngSoftElPhi","Soft El phi; (3 prong)",800,-4,4);        
    m_h1_FourProngNSoftEl      = new TH1F("FourProngNSoftEl"," # soft el; (4 prong)", 10,0,10);            
    m_h1_FourProngSoftElPt     = new TH1F("FourProngSoftElPt"," Soft El pt;  [GeV] (4 prong)",100,0,25);   
    m_h1_FourProngSoftElEta    = new TH1F("FourProngSoftElEta"," SOft El Eta ; (4 prong)",1000,-5,5);      
    m_h1_FourProngSoftElPhi    = new TH1F("FourProngSoftElPhi","Soft El phi; (4 prong)",800,-4,4);         

    m_h1_nHardEl               = new TH1F("nHardEl","# Hard El",10,0,10);                                  
    m_h1_HardElPt              = new TH1F("HardElPt","Hard El Pt ; [GeV]",1000,0,500);                     
    m_h1_HardElEta             = new TH1F("HardElEta","Hard El Eta",1000,-5,5);                            
    m_h1_HardElPhi             = new TH1F("HardElPhi","Hard El Phi",800,-4,4);                             
    m_h1_ThreeProngNHardEl     = new TH1F("ThreeProngNHardEl","# Hard Electrons ; (3 prong )",10,0,10);    
    m_h1_ThreeProngHardElPt    = new TH1F("ThreeProngHardElPt","Hard Electron pt;[GeV] (3 prong)",1000,0,500);
    m_h1_ThreeProngHardElEta   = new TH1F("ThreeProngHardElEta","Hard Electron Eta; (3 prong)",1000,-5,5); 
    m_h1_ThreeProngHardElPhi   = new TH1F("ThreePRongHardElPhi","Hard Electron Phi; (3 prong)",800,-4,4);  
    m_h1_FourProngNHardEl      = new TH1F("FourProngNHardEl","# Hard Electrons ; (4 prong )",10,0,10);     
    m_h1_FourProngHardElPt     = new TH1F("FourProngHardElPt","Hard Electron pt ; [GeV] (4 prong)",1000,0,500); 
    m_h1_FourProngHardElEta    = new TH1F("FourProngHardElEta","Hard Electron Eta; (4 prong)",1000,-5,5);  
    m_h1_FourProngHardElPhi    = new TH1F("FourPRongHardElPhi","Hard Electron Phi; (4 prong)",800,-4,4);   

    m_h1_ThreeProngNBjets      = new TH1F("ThreeProngNBjets"," # b-jets ; (3 prong)", 10,0,10);            
    m_h1_ThreeProngBjetPt      = new TH1F("ThreeProngBjetPt","b-jet pt; p_{T} [GeV] (3 prong)",1000,0,500);
    m_h1_ThreeProngBjetEta     = new TH1F("ThreeProngBjetEta","b-jet #eta; (3 prong)", 1000,-5,5);         
    m_h1_ThreeProngBjetPhi     = new TH1F("ThreeProngBjetPhi","b-jet #phi; (3 prong)", 800,-4,4);          
    m_h1_ThreeProngNCjets      = new TH1F("ThreeProngNCjets","# c-jets; (3 prong)", 10,0,10);              
    m_h1_ThreeProngCjetPt      = new TH1F("ThreeProngCjetPt","c-jet pt; p_{T} [GeV](3 prong)",1000,0,500); 
    m_h1_ThreeProngCjetEta     = new TH1F("ThreeProngCjetEta","c-jet #eta; (3 prong)",1000,-5,5);          
    m_h1_ThreeProngCjetPhi     = new TH1F("ThreeProngCjetPhi","c-jet #phi ; (3 prong)",10,0,10);           
    m_h1_ThreeProngNLjets      = new TH1F("ThreeProngNLjets","# l-jets;(3 prong)",10,0,10);                
    m_h1_ThreeProngLjetPt      = new TH1F("ThreeProngLjetPt","l-jet pt; p_{T} [GeV] (3 prong)",1000,0,500);
    m_h1_ThreeProngLjetEta     = new TH1F("ThreeProngLjetEta","l-jet #eta; (3 prong)",1000,-5,5);          
    m_h1_ThreeProngLjetPhi     = new TH1F("ThreeProngLjetPhi","l-jet #phi; (3 prong)",800,-4,4);           

    m_h1_FourProngNBjets       = new TH1F("FourProngNBjets"," # b-jets ; ( 4 prong)", 10,0,10);            
    m_h1_FourProngBjetPt       = new TH1F("FourProngBjetPt","b-jet pt; p_{T} [GeV]( 4 prong)",1000,0,500); 
    m_h1_FourProngBjetEta      = new TH1F("FourProngBjetEta","b-jet #eta; ( 4 prong)", 1000,-5,5);         
    m_h1_FourProngBjetPhi      = new TH1F("FourProngBjetPhi","b-jet #phi; ( 4 prong)", 800,-4,4);          
    m_h1_FourProngNCjets       = new TH1F("FourProngNCjets","# c-jets; ( 4 prong)", 10,0,10);              
    m_h1_FourProngCjetPt       = new TH1F("FourProngCjetPt","c-jet pt; p_{T} [GeV] ( 4 prong)",1000,0,500);
    m_h1_FourProngCjetEta      = new TH1F("FourProngCjetEta","c-jet #eta; ( 4 prong)",1000,-5,5);          
    m_h1_FourProngCjetPhi      = new TH1F("FourProngCjetPhi","c-jet #phi ; ( 4 prong)",10,0,10);           
    m_h1_FourProngNLjets       = new TH1F("FourProngNLjets","# l-jets;( 4 prong)",10,0,10);                
    m_h1_FourProngLjetPt       = new TH1F("FourProngLjetPt","l-jet pt; p_{T} [GeV] ( 4 prong)",1000,0,500);
    m_h1_FourProngLjetEta      = new TH1F("FourProngLjetEta","l-jet #eta; ( 4 prong)",1000,-5,5);          
    m_h1_FourProngLjetPhi      = new TH1F("FourProngLjetPhi","l-jet #phi; ( 4 prong)",800,-4,4);           


    m_h1_JetPt                 = new TH1F("JetPt"," jet pt ;[GeV]",1000,0,500);                            
    m_h1_JetEta                = new TH1F("JetEta"," jet Eta ",1000,-5,5);                                 
    m_h1_JetPhi                = new TH1F("JetPhi"," jet Phi ",800,-4,4);                                  
    
    m_h1_bQuarkPt              = new TH1F("bQuarkPt","p_{T} of b-quark before hadronization",1000,0,500);  
    m_h1_bQuarkEta             = new TH1F("bQuarkEta","#eta of b-quark before hadronization",1000,-5,5);   
    m_h1_bQuarkPhi             = new TH1F("bQuarkPhi","#phi of b-quark before hadronization",800,-4,4);    

    m_h1_bHadronPdgId          = new TH1F("bHadronPdgId","pdg id of the b-hadron",100,0,100);

    //register histograms 
    StatusCode sc; 
    std::string histDir("/SoftElectron/");

    sc = m_histos->regHist(histDir + "mcn",m_h1_mcn);
   
    sc = m_histos->regHist(histDir + "nvtx", m_h1_nvtx);
    sc = m_histos->regHist(histDir + "pvx_x", m_h1_pvx_x);
    sc = m_histos->regHist(histDir + "pvx_y",m_h1_pvx_y);
    sc = m_histos->regHist(histDir + "pvx_z",m_h1_pvx_z);
    sc = m_histos->regHist(histDir + "pvx_xRes",m_h1_primvtxresx);
    sc = m_histos->regHist(histDir + "pvx_yRes",m_h1_primvtxresy);
    sc = m_histos->regHist(histDir + "pvx_zRes",m_h1_primvtxresz);
    
    sc = m_histos->regHist(histDir + "nJets",m_h1_nJets);
    sc = m_histos->regHist(histDir + "nBjets",m_h1_nBjets);
    sc = m_histos->regHist(histDir + "bjetPt",m_h1_bJetPt);
    sc = m_histos->regHist(histDir + "bjetEta",m_h1_bJetEta);
    sc = m_histos->regHist(histDir + "bjetPhi",m_h1_bJetPhi);
    sc = m_histos->regHist(histDir + "nCjets",m_h1_nCjets);
    sc = m_histos->regHist(histDir + "cjetPt",m_h1_cJetPt);
    sc = m_histos->regHist(histDir + "cjetEta",m_h1_cJetEta);
    sc = m_histos->regHist(histDir + "cjetPhi",m_h1_cJetPhi);
    sc = m_histos->regHist(histDir + "nLightJets",m_h1_nLightJets);
    sc = m_histos->regHist(histDir + "ljetPt",m_h1_lJetPt);
    sc = m_histos->regHist(histDir + "ljetEta",m_h1_lJetEta);
    sc = m_histos->regHist(histDir + "ljetPhi",m_h1_lJetPhi);
    
    sc = m_histos->regHist(histDir + "NElectrons",m_h1_NElectrons);
    
    sc = m_histos->regHist(histDir + "nSoftEl",m_h1_nSoftEl);
    sc = m_histos->regHist(histDir + "SoftElPt",m_h1_SoftElPt);
    sc = m_histos->regHist(histDir + "SoftElEta",m_h1_SoftElEta);
    sc = m_histos->regHist(histDir + "SoftElPhi",m_h1_SoftElPhi);
    sc = m_histos->regHist(histDir + "ThreeProngNSoftEl",m_h1_ThreeProngNSoftEl);
    sc = m_histos->regHist(histDir + "ThreeProngSoftElPt",m_h1_ThreeProngSoftElPt);
    sc = m_histos->regHist(histDir + "ThreeProngSoftElEta",m_h1_ThreeProngSoftElEta);
    sc = m_histos->regHist(histDir + "ThreeProngSoftElPhi",m_h1_ThreeProngSoftElPhi);
    sc = m_histos->regHist(histDir + "FourProngNSoftEl", m_h1_FourProngNSoftEl );
    sc = m_histos->regHist(histDir + "FourProngSoftElPt", m_h1_FourProngSoftElPt);
    sc = m_histos->regHist(histDir + "FourProngSoftElEta", m_h1_FourProngSoftElEta);
    sc = m_histos->regHist(histDir + "FourProngSoftElPhi",m_h1_FourProngSoftElPhi);
    
    sc = m_histos->regHist(histDir + "nHardEl",m_h1_nHardEl);
    sc = m_histos->regHist(histDir + "HardElPt",m_h1_HardElPt);
    sc = m_histos->regHist(histDir + "HardElEta", m_h1_HardElEta);
    sc = m_histos->regHist(histDir + "HardElPhi", m_h1_HardElPhi);
    sc = m_histos->regHist(histDir + "ThreeProngNHardEl", m_h1_ThreeProngNHardEl);
    sc = m_histos->regHist(histDir + "ThreeProngHardElPt", m_h1_ThreeProngHardElPt);
    sc = m_histos->regHist(histDir + "ThreeProngHardElEta", m_h1_ThreeProngHardElEta);
    sc = m_histos->regHist(histDir + "ThreePRongHardElPhi", m_h1_ThreeProngHardElPhi);
    sc = m_histos->regHist(histDir + "FourProngNHardEl", m_h1_FourProngNHardEl );
    sc = m_histos->regHist(histDir + "FourProngHardElPt", m_h1_FourProngHardElPt);
    sc = m_histos->regHist(histDir + "FourProngHardElEta", m_h1_FourProngHardElEta);
    sc = m_histos->regHist(histDir + "FourPRongHardElPhi", m_h1_FourProngHardElPhi);
    
    sc = m_histos->regHist(histDir + "ThreeProngNBjets", m_h1_ThreeProngNBjets );
    sc = m_histos->regHist(histDir + "ThreeProngBjetPt", m_h1_ThreeProngBjetPt );
    sc = m_histos->regHist(histDir + "ThreeProngBjetEta", m_h1_ThreeProngBjetEta);
    sc = m_histos->regHist(histDir + "ThreeProngBjetPhi", m_h1_ThreeProngBjetPhi);
    sc = m_histos->regHist(histDir + "ThreeProngNCjets", m_h1_ThreeProngNCjets );
    sc = m_histos->regHist(histDir + "ThreeProngCjetPt", m_h1_ThreeProngCjetPt );
    sc = m_histos->regHist(histDir + "ThreeProngCjetEta", m_h1_ThreeProngCjetEta);
    sc = m_histos->regHist(histDir + "ThreeProngCjetPhi",m_h1_ThreeProngCjetPhi);
    sc = m_histos->regHist(histDir + "ThreeProngNLjets",  m_h1_ThreeProngNLjets );
    sc = m_histos->regHist(histDir + "ThreeProngLjetPt",  m_h1_ThreeProngLjetPt );
    sc = m_histos->regHist(histDir + "ThreeProngLjetEta", m_h1_ThreeProngLjetEta);
    sc = m_histos->regHist(histDir + "ThreeProngLjetPhi",  m_h1_ThreeProngLjetPhi);
    
    sc = m_histos->regHist(histDir + "FourProngNBjets",   m_h1_FourProngNBjets  );
    sc = m_histos->regHist(histDir + "FourProngBjetPt",   m_h1_FourProngBjetPt  );
    sc = m_histos->regHist(histDir + "FourProngBjetEta", m_h1_FourProngBjetEta );
    sc = m_histos->regHist(histDir + "FourProngBjetPhi",m_h1_FourProngBjetPhi );
    sc = m_histos->regHist(histDir + "FourProngNCjets", m_h1_FourProngNCjets  );
    sc = m_histos->regHist(histDir + "FourProngCjetPt",   m_h1_FourProngCjetPt  );
    sc = m_histos->regHist(histDir + "FourProngCjetEta", m_h1_FourProngCjetEta );
    sc = m_histos->regHist(histDir + "FourProngCjetPhi",m_h1_FourProngCjetPhi );
    sc = m_histos->regHist(histDir + "FourProngNLjets",   m_h1_FourProngNLjets  );
    sc = m_histos->regHist(histDir + "FourProngLjetPt",  m_h1_FourProngLjetPt  );
    sc = m_histos->regHist(histDir + "FourProngLjetEta", m_h1_FourProngLjetEta );
    sc = m_histos->regHist(histDir + "FourProngLjetPhi",   m_h1_FourProngLjetPhi );
    
    sc = m_histos->regHist(histDir + "JetPt",m_h1_JetPt );
    sc = m_histos->regHist(histDir + "JetEta",m_h1_JetEta );
    sc = m_histos->regHist(histDir + "JetPhi", m_h1_JetPhi );
    
    sc = m_histos->regHist(histDir + "bQuarkPt", m_h1_bQuarkPt );
    sc = m_histos->regHist(histDir + "bQuarkEta",m_h1_bQuarkEta );
    sc = m_histos->regHist(histDir + "bQuarkPhi",m_h1_bQuarkPhi );
    sc = m_histos->regHist(histDir + "bHadronPdgId", m_h1_bHadronPdgId );

    //Sumw2()

    if (sc.isFailure() ) {}
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

int SoftElectron::GetMEPartonBarcode()
{
    int bQuarkBarcode  = -1;
    
    const HepMC::GenEvent* GenEvent = *(m_mcEventCollection->begin());
    HepMC::GenEvent::particle_const_iterator pitr = GenEvent->particles_begin();

    for (; pitr != GenEvent->particles_end(); ++pitr) 
    {
        int pdg = (*pitr)->pdg_id();
        int apdg = std::abs(pdg) ;
        
        if (apdg == 5 || apdg == 4 || apdg == 3 || apdg == 2 || apdg == 1)
        {
            // b quark
            const HepMC::GenParticle * bpart = (*pitr) ;
           
            // find the production vertex and parents
            HepMC::GenVertex * prodvtx = bpart->production_vertex() ;
            bool hasbhadronparent(false) ;
            bool hasmpiparent(false) ;
            bool isME(false);

            if ( prodvtx ) 
            {
                HepMC::GenVertex::particle_iterator pin = prodvtx->particles_begin(HepMC::parents) ;
                for (; pin != prodvtx->particles_end(HepMC::parents) && !hasbhadronparent; pin++) 
                {
                   // if the status of a b-quark is 123 or 124, then it is a ME b-quark
                   if ( (*pin)->status() == 123 || (*pin)->status() == 124 ) 
                   {
                       isME = true ;
                       bQuarkBarcode  = (*pin)->barcode();
                   }
               }
            }
        } 
    }
    return bQuarkBarcode;
}

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
