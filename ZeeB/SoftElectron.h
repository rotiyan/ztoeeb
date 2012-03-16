#ifndef ZEEB_SOFTELECTRON_H
#define ZEEB_SOFTELECTRON_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <string>
#include <map>
#include <vector>
#include <sys/types.h>
#include <TH1.h>
#include <TH2.h>
#include "GaudiKernel/ITHistSvc.h"

#include "JetEvent/JetTagInfoBase.h"
#include "JetTagInfo/TruthInfo.h"
#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "HepMC/GenParticle.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "HepPDT/ParticleDataTable.hh"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronAssociation.h"

#include "TruthUtils/IHforTool.h"

namespace Analysis { class JetTagInfoBase; }
namespace Rec { class TrackParticleContainer; }
namespace Analysis 
{
    class MuonContainer; 
    class IJetTruthMatching;
}
class ElectronContainer;
class McEventCollection;
class EventInfo;
class TruthParticleContainer;
class VxContainer;
class JetCollection;
class ITHistSvc;
class TH1F;
class Jet;

class SoftElectron : public Algorithm 
{
    public:
        SoftElectron(const std::string& name, ISvcLocator* pSvcLocator);
        ~SoftElectron();
      
        StatusCode initialize();
        StatusCode finalize();
        StatusCode execute();
      
        // helper methods:
        void BookHistograms();

        /* Track quality for electrons **/
        StatusCode checkTrackqualforSET(Rec::TrackParticleContainer::const_iterator trackItr,double *pT);
        
    private:
        bool isBHadron(int pdg);
        bool isDHadron(int pdg);
        StatusCode LoadContainers(); //retrieves the containers from the store gate
        int  GetMEPartonBarcode(); //Get the barcode of matrix elecment(hard process ) b
        bool isLightJetIsolated(const HepLorentzVector& jet);
    
    private:
        StoreGateSvc* m_storeGate;
      
        std::string m_particleJetContainerName;
        std::string m_truthParticleContainerName;
        std::string m_trackParticleContainerName;
        std::string m_primaryVertexContainerName;
        std::string m_mcEventContainerName;
        std::string m_electronContainerName;
        std::string m_muonContainerName;
        
        int m_selectBFromRun; // use only b-jets from this run number
        int m_selectUFromRun; // ditto for light jets
        double m_purificationDeltaR;       // light jets closer than this dR to b,c,tau are ignored
        double m_leptonPurificationDeltaR; // jets closer than this dR to "high-pt" leptons are ignored
      
        int m_irun;
        int m_ievt;
      
        const Rec::TrackParticleContainer*  m_trackTES;
        const TruthParticleContainer*  m_mcpartTES;
        const EventInfo* m_eventInfo;
        const McEventCollection* m_mcEventCollection;
        const VxContainer* m_vxContainer;
        const JetCollection* m_jetCollection;
        const ElectronContainer* m_electronCollection;
        ITHistSvc* m_histos; /// histo service

        ToolHandle< Analysis::IJetTruthMatching > m_jetTruthMatchTool;
        ToolHandle< IHforTool > hfor_tool;

        
        TH1F* m_h1_mcn;

        TH1F* m_h1_nvtx;
        TH1F* m_h1_pvx_x;
        TH1F* m_h1_pvx_y;
        TH1F* m_h1_pvx_z;
        TH1F* m_h1_primvtxresx;
        TH1F* m_h1_primvtxresy;
        TH1F* m_h1_primvtxresz;

        TH1F* m_h1_nJets;
        TH1F* m_h1_JetPt;
        TH1F* m_h1_JetEta;
        TH1F* m_h1_JetPhi;
        TH1F* m_h1_nBjets;
        TH1F* m_h1_bJetPt;
        TH1F* m_h1_bJetEta;
        TH1F* m_h1_bJetPhi;
        TH1F* m_h1_nCjets;
        TH1F* m_h1_cJetPt;
        TH1F* m_h1_cJetEta;
        TH1F* m_h1_cJetPhi;
        TH1F* m_h1_nLightJets;
        TH1F* m_h1_lJetPt;
        TH1F* m_h1_lJetEta;
        TH1F* m_h1_lJetPhi;
        TH1F* m_h1_NElectrons;
   
        TH1F* m_h1_nSoftEl;
        TH1F* m_h1_SoftElPt;
        TH1F* m_h1_SoftElEta;
        TH1F* m_h1_SoftElPhi;
        TH1F* m_h1_ThreeProngNSoftEl;
        TH1F* m_h1_ThreeProngSoftElPt;
        TH1F* m_h1_ThreeProngSoftElEta;
        TH1F* m_h1_ThreeProngSoftElPhi;
        TH1F* m_h1_FourProngNSoftEl;
        TH1F* m_h1_FourProngSoftElPt;
        TH1F* m_h1_FourProngSoftElEta;
        TH1F* m_h1_FourProngSoftElPhi;

        TH1F* m_h1_nHardEl;
        TH1F* m_h1_HardElPt;
        TH1F* m_h1_HardElEta;
        TH1F* m_h1_HardElPhi;
        TH1F* m_h1_ThreeProngNHardEl;
        TH1F* m_h1_ThreeProngHardElPt;
        TH1F* m_h1_ThreeProngHardElEta;
        TH1F* m_h1_ThreeProngHardElPhi;
        TH1F* m_h1_FourProngNHardEl;
        TH1F* m_h1_FourProngHardElPt;
        TH1F* m_h1_FourProngHardElEta;
        TH1F* m_h1_FourProngHardElPhi;

        TH1F* m_h1_ThreeProngNBjets;
        TH1F* m_h1_ThreeProngBjetPt;
        TH1F* m_h1_ThreeProngBjetEta;
        TH1F* m_h1_ThreeProngBjetPhi;
        TH1F* m_h1_ThreeProngNCjets;
        TH1F* m_h1_ThreeProngCjetPt;
        TH1F* m_h1_ThreeProngCjetEta;
        TH1F* m_h1_ThreeProngCjetPhi;
        TH1F* m_h1_ThreeProngNLjets;
        TH1F* m_h1_ThreeProngLjetPt;
        TH1F* m_h1_ThreeProngLjetEta;
        TH1F* m_h1_ThreeProngLjetPhi;

        TH1F* m_h1_FourProngNBjets;
        TH1F* m_h1_FourProngBjetPt;
        TH1F* m_h1_FourProngBjetEta;
        TH1F* m_h1_FourProngBjetPhi;
        TH1F* m_h1_FourProngNCjets;
        TH1F* m_h1_FourProngCjetPt;
        TH1F* m_h1_FourProngCjetEta;
        TH1F* m_h1_FourProngCjetPhi;
        TH1F* m_h1_FourProngNLjets;
        TH1F* m_h1_FourProngLjetPt;
        TH1F* m_h1_FourProngLjetEta;
        TH1F* m_h1_FourProngLjetPhi;

        TH1F* m_h1_bQuarkPt;
        TH1F* m_h1_bQuarkEta;
        TH1F* m_h1_bQuarkPhi;

        TH1F* m_h1_bHadronPdgId;

        std::string m_TPTruthContainer;
        int m_eventNumber;
        int m_runNumber;

        int m_nJets;
        int m_nBjets;
        int m_nBjetsThreeProng;
        int m_nBjetsFourProng;
        int m_nCjets;
        int m_nCjetsThreeProng;
        int m_nCjetsFourProng;
        int m_nLightJets;
        int m_nLightJetsThreeProng;
        int m_nLightJetsFourProng;
        int m_nSoftEl;
        int m_nSoftElThreeProng;
        int m_nSoftElFourProng;
        int m_nHardEl;
        int m_nHardElThreeProng;
        int m_nHardElFourProng;
        int m_nElectrons;

        const HepMC::GenParticle* getTruth(const Rec::TrackParticle* myTrackParticle,const TrackParticleTruthCollection* mcpartTES,const Rec::TrackParticleContainer* trackTES);
  
        const HepPDT::ParticleDataTable* m_particleTable;
};

#endif // SoftElectron
