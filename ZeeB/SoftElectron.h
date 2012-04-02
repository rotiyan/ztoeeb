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

#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "HepMC/GenParticle.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "HepPDT/ParticleDataTable.hh"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronAssociation.h"

#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"

#include "TruthUtils/IHforTool.h"

namespace Rec { class TrackParticleContainer; }
namespace HepMC {class GenParticle; }
class ElectronContainer;
class IMCTruthClassifier;
class McEventCollection;
class EventInfo;
class TruthParticleContainer;
class VxContainer;
class ITHistSvc;
class TH1F;
class MsgStream;

class SoftElectron : public Algorithm 
{
    public:
        SoftElectron(const std::string& name, ISvcLocator* pSvcLocator);
        ~SoftElectron();
      
        StatusCode initialize();
        StatusCode finalize();
        StatusCode execute();
      
    
    private:
        bool isBHadron(const HepMC::GenParticle* p);
        bool isCHadron(const HepMC::GenParticle* p);
        void FindTruthParticle();
        void DoElectronMatch();
        HepMC::GenParticle* GetElectronParent(Analysis::Electron*  el);
        StatusCode LoadContainers(); //retrieves the containers from the store gate
        void ClearCounters();
        void FillCounters(std::string type, HepMC::GenParticle* parent);
        void FillHistograms();
        StatusCode BookHistograms();
    
    private:
        StoreGateSvc* m_storeGate;

        std::string m_truthParticleContainerName;
        std::string m_trackParticleContainerName;
        std::string m_primaryVertexContainerName;
        std::string m_mcEventContainerName;
        std::string m_electronContainerName;
        std::string m_muonContainerName;
        
        int m_irun;
        int m_ievt;

        int m_nBHadrons;
        int m_nCHadrons;

        int m_nHardEl;
        int m_nSoftEl;
        int m_nBEl;
        int m_nZEl;
        int m_nHardZEl;
        int m_nHardBEl;
        int m_nSoftZEl;
        int m_nSoftBEl;
      
        const TruthParticleContainer*  m_mcTruthContainer;
        const EventInfo* m_eventInfo;
        const McEventCollection* m_mcEventCollection;
        const VxContainer* m_vxContainer;
        const ElectronContainer* m_electronCollection;
        ITHistSvc* m_histos; /// histo service

        ToolHandle< IHforTool > m_hfor_tool;
        ToolHandle< IMCTruthClassifier> m_mcTruthClassifier;

        std::map<std::string,TH1F*> m_h1Hists;
        std::map<std::string,TH2F*> m_h2Hists;

        std::vector<const HepMC::GenParticle* >m_BHadronContainer;
        std::vector<const HepMC::GenParticle* >m_CHadronContainer;
        
        float m_hardElLowPtCut;
        float m_softElLowPtcut;
        float m_softElHighPtCut;

        float m_etaMax;
        float m_crackEtaMin;
        float m_crackEtaMax;
};
  

#endif // SoftElectron
