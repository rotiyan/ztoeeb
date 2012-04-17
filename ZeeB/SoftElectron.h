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
#include <TTree.h>
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

        /**
         * Book the histograms at the 
         * begining of the loop. 
         * The histograms are contained 
         * in an stl map which 
         * can be indexed by strings (names)
         */
        StatusCode BookHistograms();

        /**
         * Checks if the GenParticle is a B-hadron
         * based on the pdg id
         */
        bool isBHadron(const HepMC::GenParticle* p);

        /**
         * Checks if the Genparticle 
         * is a C-hadrons
         * based on the pdg id
         */
        bool isCHadron(const HepMC::GenParticle* p);

        /**
         * Loop through the GenEvent 
         * containers and Identifies 
         * the truth particles of interest
         */
        void FindTruthParticle();

        /**
         * Match the Reco electron with 
         * the truth electrons
         * using MCTruthClassifier and 
         * a custom class HerwigTruthclassifier
         */
        void DoElectronMatch();

        /**
         * Returns the parent of the Truth Electrons
         */
        HepMC::GenParticle* GetElectronParent(const Analysis::Electron*  el);

        /**
         * Load the AOD containers 
         * which are required for 
         * the analysis
         */
        StatusCode LoadContainers(); 

        /**
         * Clear the counters and 
         * containers at the begining of 
         * an event loop
         */
        void ClearCounters();

        /**
         * Called at the end of an 
         * event loop to fill event 
         * specific variables
         * and counters
         */
        void FillHistograms();


        /**
         * Returns the parents of 
         * the particles in GenEvent
         * container
         */
        std::vector<const HepMC::GenParticle*> GetParents(const HepMC::GenParticle* part);

        /**
         * Returns the childrens of
         * the particles in the GenEvent
         * container
         */
        std::vector<const HepMC::GenParticle*> GetChildren(const HepMC::GenParticle* part);

        /**
         * Function to count and 
         * retrieve Hadrons from 
         * the GenEvent containers.
         * Ignores hadrons which 
         * have same flavour parents. 
         */
        void GetTruthHadrons(const HepMC::GenParticle* part, std::vector<const HepMC::GenParticle*> &hdrnHolder,
                TH1F* prodHist,TH1F* semiHist, int &counter);

        /**
         * Miscellaneous function to 
         * check if a particle is in 
         * the supplied vector container
         */
        bool IsIn(const HepMC::GenParticle* part, std::vector<const HepMC::GenParticle*> vec );
    
    private:
        StoreGateSvc* m_storeGate;

        std::string m_truthParticleContainerName;
        std::string m_trackParticleContainerName;
        std::string m_primaryVertexContainerName;
        std::string m_mcEventContainerName;
        std::string m_electronContainerName;
        std::string m_muonContainerName;
        
        const TruthParticleContainer*  m_mcTruthContainer;
        const EventInfo* m_eventInfo;
        const McEventCollection* m_mcEventCollection;
        const VxContainer* m_vxContainer;
        const ElectronContainer* m_electronCollection;
        ITHistSvc* m_histos; /// histo service

        ToolHandle< IHforTool > m_hfor_tool;
        ToolHandle< IMCTruthClassifier> m_mcTruthClassifier;

        TTree* m_tree; 
        std::vector<int>*       m_elAuthor;
        std::vector<int>*       m_elAuthorSofte;
        std::vector<double>*    m_el_trk_PtBr;
        std::vector<double>*    m_el_trk_EtaBr;
        std::vector<double>*    m_el_trk_PhiBr;
        std::vector<double>*    m_el_cl_PtBr;
        std::vector<double>*    m_el_cl_EtaBr;
        std::vector<double>*    m_el_cl_PhiBr;
        std::vector<double>*    m_el_truth_PtBr;
        std::vector<double>*    m_el_truth_EtaBr;
        std::vector<double>*    m_el_truth_PhiBr;
        std::vector<int>*       m_elMtchd;
        std::vector<int>*       m_mtchdParent;
        std::vector<int>*       m_BPDG;
        std::vector<int>*       m_BStatus;
        std::vector<int>*       m_BSemiPDG;
        std::vector<int>*       m_BSemiStatus;
        std::vector<int>*       m_CPDG;
        std::vector<int>*       m_CStatus;
        std::vector<int>*       m_CSemiPDG;
        std::vector<int>*       m_CSemiStatus;

        std::vector<const HepMC::GenParticle* >m_TruthBvec;
        std::vector<const HepMC::GenParticle* >m_TruthCvec;
        
        float m_hardElLowPtCut;
        float m_softElLowPtcut;
        float m_softElHighPtCut;

        float m_etaMax;
        float m_crackEtaMin;
        float m_crackEtaMax;
};
  

#endif // SoftElectron
