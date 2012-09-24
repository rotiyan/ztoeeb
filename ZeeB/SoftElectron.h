#ifndef ZEEB_SOFTELECTRON_H
#define ZEEB_SOFTELECTRON_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <sys/types.h>
#include <TH1.h>
#include <TH2.h>
#include <THnSparse.h>
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
#include "TrigDecisionTool/TrigDecisionTool.h"


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
         * Creat Tree and branches
         * begining of the loop. 
         * The histograms are contained 
         * in an stl map which 
         * can be indexed by strings (names)
         */
        StatusCode BookTree();

        /**
         * Loop through the GenEvent 
         * containers and Identifies 
         * the truth particles of interest
         */
        void FindTruthParticle();

        /**
         * Finds the last hadron in the decay 
         * chain. Code from Georges Aaad
         */
        bool isFinalState(const HepMC::GenParticle* part, int type);

        /**
         * check if the particle is B-hadron
         */
        bool isBHadron(const HepMC::GenParticle* part);

        /**
         * check if the particle is C-hadron
         */
        bool isCHadron(const HepMC::GenParticle* part);

        /**
         * Fill Electrons container info into the 
         * ntuple
         */
        void FillElectrons();

        /**
         * Use Mc truth classifier to  find the truth electron
         */
        void DoElectronMatch();

        /**
         * Returns the parent of the Truth Electrons
         */
        const HepMC::GenParticle* GetElectronParent(const Analysis::Electron*  el);

        /**
         * Load the AOD containers 
         * which are required for 
         * the analysis
         */
        StatusCode LoadContainers();

        /**
         * Book Ntuple Containers
         */
        void BookNtupleContainers();

        /**
         * Clear the counters
         * and ntuple branch containers 
         */
        void ClearContainers();

        /**
         * Return the mother of a particle in the HepMC records
         */
        const HepMC::GenParticle* GetMother(const HepMC::GenParticle* part);

        /**
         * Returns the childrens of
         * the particle in question
         */
        std::vector<const HepMC::GenParticle*> GetChildren(const HepMC::GenParticle* part);

        /**
         * Returns the parents of 
         * the particle in question
         */
        std::vector<const HepMC::GenParticle*> GetParents(const HepMC::GenParticle* part);

        /**
         * Function to count and 
         * retrieve Hadrons from 
         * the GenEvent containers.
         * Ignores hadrons which 
         * have same flavour parents. 
         */
        void GetTruthHadrons(const HepMC::GenParticle* part, std::vector<const HepMC::GenParticle*> &hdrnHolder,
                TH1F* prodHist,TH1F* semiHist, int &counter);

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
        std::string m_hforType;
        ToolHandle< IMCTruthClassifier> m_mcTruthClassifier;
        ToolHandle< Trig::TrigDecisionTool > m_trigDec;

        //TTree
        TTree*                  m_tree;

        unsigned int            m_runNumber;
        unsigned int            m_lumiNumber;

        bool                    m_fillGenInfo;
        double                  m_elPtCut;
        double                  m_elEtaCut;
        double                  m_elCrackEtaCutLow;
        double                  m_elCrackEtaCutHigh;

        //Blayer
        std::vector<float>*     m_numberOfbLayerHits;
        std::vector<float>*     m_numberOfbLayerOutliers;
        std::vector<float>*     m_numberOfbLayerSharedHits; //Hits shared by several tracks
        std::vector<float>*     m_numberOfbLayerSplitHits;
        std::vector<float>*     m_numberOfexpectBLayerHit; //Do we expect a b-layer hit

        //Pixel
        std::vector<float>*     m_numberOfPixelHits;
        std::vector<float>*     m_numberOfPixelOutliers ;
        std::vector<float>*     m_numberOfPixelHoles;  //number of pixel layers on track with absence of hits
        std::vector<float>*     m_numberOfPixelSharedHits; //number of Pixel all-layer hits shared by several tracks.
        std::vector<float>*     m_numberOfPixelSplitHits; //number of Pixel all-layer hits split by cluster splitting
        std::vector<float>*     m_numberOfGangedPixels; //number of pixels which have a ganged ambiguity.
        std::vector<float>*     m_numberOfGangedFlaggedFakes;//number of Ganged Pixels flagged as fakes
        std::vector<float>*     m_numberOfPixelDeadSensors;//number of dead pixel sensors crossed
        std::vector<float>*     m_numberOfPixelSpoiltHits;//number of pixel hits with broad errors (width/sqrt(12))

        //SCT
        std::vector<float>*     m_numberOfSCTHits;
        std::vector<float>*     m_numberOfSCTOutliers; 
        std::vector<float>*     m_numberOfSCTHoles;
        std::vector<float>*     m_numberOfSCTDoubleHoles;  //number of Holes in both sides of a SCT module
        std::vector<float>*     m_numberOfSCTSharedHits;
        std::vector<float>*     m_numberOfSCTDeadSensors;
        std::vector<float>*     m_numberOfSCTSpoiltHits; //number of SCT hits with broad errors (width/sqrt(12))


        //TRT
        std::vector<float>*     m_numberOfTRTHits;
        std::vector<float>*     m_numberOfTRTOutliers;
        std::vector<float>*     m_numberOfTRTHoles;
        std::vector<float>*     m_numberOfTRTHTHits;
        std::vector<float>*     m_numberOfTRTHTOutliers;
        std::vector<float>*     m_numberOfTRTDeadStraws;
        std::vector<float>*     m_numberOfTRTTubeHits;

        //Shower
        std::vector<float>*     m_e237;// E(3*7) in second sampling
        std::vector<float>*     m_e277;// E(7*7) in second sampling

};
  
#endif // SoftElectron
