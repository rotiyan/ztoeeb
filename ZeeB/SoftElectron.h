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

        /**
         * Miscellaneous function to 
         * check if a particle is in 
         * the supplied vector container
         */
    
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

        TTree* m_tree; 
        std::vector<double>*    m_ZElPt;
        std::vector<double>*    m_ZElEta;
        std::vector<double>*    m_ZElPhi;

        std::vector<int>*       m_el_charge;
        std::vector<bool>*       m_elAuthor;
        std::vector<bool>*       m_elAuthorSofte;
        std::vector<double>*    m_el_trk_Pt;
        std::vector<double>*    m_el_trk_Eta;
        std::vector<double>*    m_el_trk_Phi;
        std::vector<double>*    m_el_cl_Pt;
        std::vector<double>*    m_el_cl_Eta;
        std::vector<double>*    m_el_cl_Phi;

        std::vector<bool>*      m_el_id_loose;
        std::vector<bool>*      m_el_id_loosepp;
        std::vector<bool>*      m_el_id_medium;
        std::vector<bool>*      m_el_id_mediumpp;
        std::vector<bool>*      m_el_id_tight;
        std::vector<bool>*      m_el_id_tightpp;

        std::vector<double>*    m_el_truth_Pt;
        std::vector<double>*    m_el_truth_Eta;
        std::vector<double>*    m_el_truth_Phi;
        std::vector<int>*       m_elMtchd;
        std::vector<int>*       m_mtchdParent;
        std::vector<int>*       m_mtchdGrndParent;

        std::vector<int>*       m_BPDG;
        std::vector<int>*       m_BStatus;
        std::vector<double>*    m_BPt;
        std::vector<double>*    m_BEta;
        std::vector<double>*    m_BPhi;
        std::vector<int>*       m_BBC;
        std::vector<int>*       m_BisSemiElectron;
        std::vector<double>*    m_BsemiElPt;
        std::vector<double>*    m_BsemiElEta;
        std::vector<double>*    m_BsemiElPhi;
        std::vector<int>*       m_CPDG;
        std::vector<int>*       m_CStatus;
        std::vector<double>*    m_CPt;
        std::vector<double>*    m_CEta;
        std::vector<double>*    m_CPhi;
        std::vector<int>*       m_CisSemiElectron;
        std::vector<int>*       m_CparentBC;
        std::vector<int>*       m_CgrndParentBC;
        std::vector<double>*    m_CsemiElPt;
        std::vector<double>*    m_CsemiElEta;
        std::vector<double>*    m_CsemiElPhi;

        std::vector<double>*    m_bQuarkME_pt;
        std::vector<double>*    m_bQuarkME_eta;
        std::vector<double>*    m_bQuarkME_phi;
        std::vector<int>*       m_bQuarkME_pdg;

        bool                    m_fillGenInfo;
        bool                    m_doTruthMatching;

        double                  m_elPtCut;
        double                  m_elEtaCut;
        double                  m_elCrackEtaCutLow;
        double                  m_elCrackEtaCutHigh;
};
  

#endif // SoftElectron
