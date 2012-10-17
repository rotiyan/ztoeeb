//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Oct  8 13:10:15 2012 by ROOT version 5.28/00g
// from TTree elId/Electron ID Variables
// found on file: /tmp/narayan/process/elId/zbbnp0/user.narayan.005329.AANT._00040.root
//////////////////////////////////////////////////////////

#ifndef ElectronID_h
#define ElectronID_h

#include <TROOT.h>
#include <TH1F.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <vector>
using namespace std;

class ElectronID : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   TH1F  *h_REta;
   TH1F  *h_RPhi;
   TH1F  *h_zMass;
   TH1F  *h_nSofte;
   TH1F  *h_nAuthor;
   TH1F  *h_TRTRatio;

   TH1F  *h_numBLayerHits;
   TH1F  *h_numBLayerOutliers;
   TH1F  *h_numBLayerShared;
   TH1F  *h_numPixelHits;
   TH1F  *h_numPixelOutliers;
   TH1F  *h_numPixelHoles;
   TH1F  *h_numPixelShared;
   TH1F  *h_numGangedPixels;
   TH1F  *h_numGangedFlaggedFakes;
   TH1F  *h_numPixelDeadSensors;
   TH1F  *h_numPixelSpoiltHits;
   TH1F  *h_numSCTHits;
   TH1F  *h_numSCTOutliers;
   TH1F  *h_numSCTHoles;
   TH1F  *h_numSCTDoubleHoles;
   TH1F  *h_numSCTSharedHits;
   TH1F  *h_numSCTDeadSensors;
   TH1F  *h_numSCTSpoitHits;
   TH1F  *h_d0;
   TH1F  *h_d0Err;
   TH1F  *h_z0;
   TH1F  *h_z0Err;
   TH1F  *h_numTRTHits;
   TH1F  *h_numTRTOutliers;
   TH1F  *h_numTRTHoles;
   TH1F  *h_numTRTHtHits;
   TH1F  *h_numTRTHTOutliers;
   TH1F  *h_numTRTDeadStraw;
   TH1F  *h_numTRTTubeHits;
   TH1F  *h_cone30;
   TH1F  *h_ethad;
   TH1F  *h_ethad1;
   TH1F  *h_emax;
   TH1F  *h_emax2;
   TH1F  *h_emin;
   TH1F  *h_elPt;
   TH1F  *h_elEta;
   TH1F  *h_elPhi;
   TH1F  *h_elTrnsE;
   TH1F  *h_f1;
   TH1F  *h_f1core;
   TH1F  *h_emins1;
   TH1F  *h_fracs1;
   TH1F  *h_e2tsts1;
   TH1F  *h_weta1;
   TH1F  *h_wtots1;
   TH1F  *h_emaxs1;
   TH1F  *h_e233;
   TH1F  *h_e237;
   TH1F  *h_e277;
   TH1F  *h_weta2;
   TH1F  *h_f3;
   TH1F  *h_f3core;
   TH1F  *h_etcone;
   TH1F  *h_etcone20;
   TH1F  *h_etcone30;
   TH1F  *h_etcone40;
   TH1F  *h_ptcone30;
   TH1F  *h_deltaEta1;
   TH1F  *h_deltaEta2;
   TH1F  *h_deltaPhi2;
   TH1F  *h_deltaPhiRescaled;

   // Declaration of leaf types
   vector<float>   *numBLayerHits;
   vector<float>   *numBLayerOutliers;
   vector<float>   *numBLayerShared;
   vector<float>   *numPixelHits;
   vector<float>   *numPixelOutliers;
   vector<float>   *numPixelHoles;
   vector<float>   *numPixelShared;
   vector<float>   *numGangedPixels;
   vector<float>   *numGangedFlaggedFakes;
   vector<float>   *numPixelDeadSensors;
   vector<float>   *numPixelSpoiltHits;
   vector<float>   *numSCTHits;
   vector<float>   *numSCTOutliers;
   vector<float>   *numSCTHoles;
   vector<float>   *numSCTDoubleHoles;
   vector<float>   *numSCTSharedHits;
   vector<float>   *numSCTDeadSensors;
   vector<float>   *numSCTSpoitHits;
   vector<float>   *d0;
   vector<float>   *d0Err;
   vector<float>   *z0;
   vector<float>   *z0Err;
   vector<float>   *numTRTHits;
   vector<float>   *numTRTOutliers;
   vector<float>   *numTRTHoles;
   vector<float>   *numTRTHtHits;
   vector<float>   *numTRTHTOutliers;
   vector<float>   *numTRTDeadStraw;
   vector<float>   *numTRTTubeHits;
   vector<float>   *cone30;
   vector<float>   *ethad;
   vector<float>   *ethad1;
   vector<float>   *emax;
   vector<float>   *emax2;
   vector<float>   *emin;
   vector<bool>    *isBMatch;
   vector<bool>    *isCMatch;
   vector<bool>    *isZMatch;
   vector<bool>    *isAuthor;
   vector<bool>    *isSofte;
   vector<float>   *elPt;
   vector<float>   *elEta;
   vector<float>   *elPhi;
   vector<float>   *elTrnsE;
   vector<float>   *f1;
   vector<float>   *f1core;
   vector<float>   *emins1;
   vector<float>   *fracs1;
   vector<float>   *e2tsts1;
   vector<float>   *weta1;
   vector<float>   *wtots1;
   vector<float>   *emaxs1;
   vector<float>   *e233;
   vector<float>   *e237;
   vector<float>   *e277;
   vector<float>   *weta2;
   vector<float>   *f3;
   vector<float>   *f3core;
   vector<float>   *etcone;
   vector<float>   *etcone20;
   vector<float>   *etcone30;
   vector<float>   *etcone40;
   vector<float>   *ptcone30;
   vector<float>   *deltaEta1;
   vector<float>   *deltaEta2;
   vector<float>   *deltaPhi2;
   vector<float>   *deltaPhiRescaled;
   vector<bool>    *expectHitInBLayer;

   // List of branches
   TBranch        *b_numBLayerHits;   //!
   TBranch        *b_numBLayerOutliers;   //!
   TBranch        *b_numBLayerShared;   //!
   TBranch        *b_numPixelHits;   //!
   TBranch        *b_numPixelOutliers;   //!
   TBranch        *b_numPixelHoles;   //!
   TBranch        *b_numPixelShared;   //!
   TBranch        *b_numGangedPixels;   //!
   TBranch        *b_numGangedFlaggedFakes;   //!
   TBranch        *b_numPixelDeadSensors;   //!
   TBranch        *b_numPixelSpoiltHits;   //!
   TBranch        *b_numSCTHits;   //!
   TBranch        *b_numSCTOutliers;   //!
   TBranch        *b_numSCTHoles;   //!
   TBranch        *b_numSCTDoubleHoles;   //!
   TBranch        *b_numSCTSharedHits;   //!
   TBranch        *b_numSCTDeadSensors;   //!
   TBranch        *b_numSCTSpoitHits;   //!
   TBranch        *b_d0;   //!
   TBranch        *b_d0Err;   //!
   TBranch        *b_z0;   //!
   TBranch        *b_z0Err;   //!
   TBranch        *b_numTRTHits;   //!
   TBranch        *b_numTRTOutliers;   //!
   TBranch        *b_numTRTHoles;   //!
   TBranch        *b_numTRTHtHits;   //!
   TBranch        *b_numTRTHTOutliers;   //!
   TBranch        *b_numTRTDeadStraw;   //!
   TBranch        *b_numTRTTubeHits;   //!
   TBranch        *b_cone30;   //!
   TBranch        *b_ethad;   //!
   TBranch        *b_ethad1;   //!
   TBranch        *b_emax;   //!
   TBranch        *b_emax2;   //!
   TBranch        *b_emin;   //!
   TBranch        *b_isBMatch;   //!
   TBranch        *b_isCMatch;   //!
   TBranch        *b_isZMatch;   //!
   TBranch        *b_isAuthor;   //!
   TBranch        *b_isSofte;   //!
   TBranch        *b_elPt;   //!
   TBranch        *b_elEta;   //!
   TBranch        *b_elPhi;   //!
   TBranch        *b_elTrnsE;   //!
   TBranch        *b_f1;   //!
   TBranch        *b_f1core;   //!
   TBranch        *b_emins1;   //!
   TBranch        *b_fracs1;   //!
   TBranch        *b_e2tsts1;   //!
   TBranch        *b_weta1;   //!
   TBranch        *b_wtots1;   //!
   TBranch        *b_emaxs1;   //!
   TBranch        *b_e233;   //!
   TBranch        *b_e237;   //!
   TBranch        *b_e277;   //!
   TBranch        *b_weta2;   //!
   TBranch        *b_f3;   //!
   TBranch        *b_f3core;   //!
   TBranch        *b_etcone;   //!
   TBranch        *b_etcone20;   //!
   TBranch        *b_etcone30;   //!
   TBranch        *b_etcone40;   //!
   TBranch        *b_ptcone30;   //!
   TBranch        *b_deltaEta1;   //!
   TBranch        *b_deltaEta2;   //!
   TBranch        *b_deltaPhi2;   //!
   TBranch        *b_deltaPhiRescaled;   //!
   TBranch        *b_expectHitInBLayer;   //!

   ElectronID(TTree * /*tree*/ =0) 
   {
       h_REta                   =  0;
       h_RPhi                   =  0;
       h_zMass                  =  0;
       h_nSofte                 =  0;
       h_nAuthor                =  0;
       h_TRTRatio               =  0; 

       h_numBLayerHits	        =  0;
       h_numBLayerOutliers	    =  0;
       h_numBLayerShared	    =  0;
       h_numPixelHits	        =  0;
       h_numPixelOutliers	    =  0;
       h_numPixelHoles	        =  0;
       h_numPixelShared	        =  0;
       h_numGangedPixels	    =  0;
       h_numGangedFlaggedFakes	=  0;
       h_numPixelDeadSensors	=  0;
       h_numPixelSpoiltHits	    =  0;
       h_numSCTHits	            =  0;
       h_numSCTOutliers	        =  0;
       h_numSCTHoles	        =  0;
       h_numSCTDoubleHoles	    =  0;
       h_numSCTSharedHits	    =  0;
       h_numSCTDeadSensors	    =  0;
       h_numSCTSpoitHits	    =  0;
       h_d0	                    =  0;
       h_d0Err	                =  0;
       h_z0	                    =  0;
       h_z0Err	                =  0;
       h_numTRTHits	            =  0;
       h_numTRTOutliers         =  0;
       h_numTRTHoles            =  0;
       h_numTRTHtHits           =  0;
       h_numTRTHTOutliers       =  0;
       h_numTRTDeadStraw        =  0;
       h_numTRTTubeHits         =  0;
       h_cone30	                =  0;
       h_ethad	                =  0;  //energy leakage measure in the whole hcal
       h_ethad1	                =  0;  //energy leakage measured in the first sampling of hcal
       h_emax	                =  0;
       h_emax2	                =  0;
       h_emin                   =  0;
       h_elPt                   =  0;
       h_elEta                  =  0;
       h_elPhi                  =  0;
       h_elTrnsE                =  0;
       h_f1	                    =  0;
       h_f1core	                =  0;
       h_emins1	                =  0;
       h_fracs1	                =  0;
       h_e2tsts1	            =  0;
       h_weta1	                =  0; 
       h_wtots1	                =  0;
       h_emaxs1	                =  0;
       h_e233	                =  0; //energy deposited in the second sampling of the em call in a DeltaEta*DeltaPhi=3x3
       h_e237	                =  0; //energy deposited in the second sampling of the em cal in a DeltaEta*DeltaPhi=3x7
       h_e277	                =  0; // energy deposited in the second sampling of the em cal in a DeltaEta*DeltaPhi=7x7
       h_weta2	                =  0; //shower lateral width in the Ecal middle layer
       h_f3	                    =  0;
       h_f3core	                =  0;
       h_etcone	                =  0;
       h_etcone20               =  0;
       h_etcone30               =  0;
       h_etcone40               =  0;
       h_ptcone30               =  0;
       h_deltaEta1              =  0;
       h_deltaEta2              =  0;
       h_deltaPhi2              =  0;
       h_deltaPhiRescaled       =  0;
   }
   virtual ~ElectronID() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();
   void BookHistograms();
   std::vector<float> getMaxPtList(std::vector<float>* vec);



   ClassDef(ElectronID,0);
};

#endif

#ifdef ElectronID_cxx
void ElectronID::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   numBLayerHits = 0;
   numBLayerOutliers = 0;
   numBLayerShared = 0;
   numPixelHits = 0;
   numPixelOutliers = 0;
   numPixelHoles = 0;
   numPixelShared = 0;
   numGangedPixels = 0;
   numGangedFlaggedFakes = 0;
   numPixelDeadSensors = 0;
   numPixelSpoiltHits = 0;
   numSCTHits = 0;
   numSCTOutliers = 0;
   numSCTHoles = 0;
   numSCTDoubleHoles = 0;
   numSCTSharedHits = 0;
   numSCTDeadSensors = 0;
   numSCTSpoitHits = 0;
   d0 = 0;
   d0Err = 0;
   z0 = 0;
   z0Err = 0;
   numTRTHits = 0;
   numTRTOutliers = 0;
   numTRTHoles = 0;
   numTRTHtHits = 0;
   numTRTHTOutliers = 0;
   numTRTDeadStraw = 0;
   numTRTTubeHits = 0;
   cone30 = 0;
   ethad = 0;
   ethad1 = 0;
   emax = 0;
   emax2 = 0;
   emin = 0;
   isBMatch = 0;
   isCMatch = 0;
   isZMatch = 0;
   isAuthor = 0;
   isSofte = 0;
   elPt = 0;
   elEta = 0;
   elPhi = 0;
   elTrnsE = 0;
   f1 = 0;
   f1core = 0;
   emins1 = 0;
   fracs1 = 0;
   e2tsts1 = 0;
   weta1 = 0;
   wtots1 = 0;
   emaxs1 = 0;
   e233 = 0;
   e237 = 0;
   e277 = 0;
   weta2 = 0;
   f3 = 0;
   f3core = 0;
   etcone = 0;
   etcone20 = 0;
   etcone30 = 0;
   etcone40 = 0;
   ptcone30 = 0;
   deltaEta1 = 0;
   deltaEta2 = 0;
   deltaPhi2 = 0;
   deltaPhiRescaled = 0;
   expectHitInBLayer = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("numBLayerHits", &numBLayerHits, &b_numBLayerHits);
   fChain->SetBranchAddress("numBLayerOutliers", &numBLayerOutliers, &b_numBLayerOutliers);
   fChain->SetBranchAddress("numBLayerShared", &numBLayerShared, &b_numBLayerShared);
   fChain->SetBranchAddress("numPixelHits", &numPixelHits, &b_numPixelHits);
   fChain->SetBranchAddress("numPixelOutliers", &numPixelOutliers, &b_numPixelOutliers);
   fChain->SetBranchAddress("numPixelHoles", &numPixelHoles, &b_numPixelHoles);
   fChain->SetBranchAddress("numPixelShared", &numPixelShared, &b_numPixelShared);
   fChain->SetBranchAddress("numGangedPixels", &numGangedPixels, &b_numGangedPixels);
   fChain->SetBranchAddress("numGangedFlaggedFakes", &numGangedFlaggedFakes, &b_numGangedFlaggedFakes);
   fChain->SetBranchAddress("numPixelDeadSensors", &numPixelDeadSensors, &b_numPixelDeadSensors);
   fChain->SetBranchAddress("numPixelSpoiltHits", &numPixelSpoiltHits, &b_numPixelSpoiltHits);
   fChain->SetBranchAddress("numSCTHits", &numSCTHits, &b_numSCTHits);
   fChain->SetBranchAddress("numSCTOutliers", &numSCTOutliers, &b_numSCTOutliers);
   fChain->SetBranchAddress("numSCTHoles", &numSCTHoles, &b_numSCTHoles);
   fChain->SetBranchAddress("numSCTDoubleHoles", &numSCTDoubleHoles, &b_numSCTDoubleHoles);
   fChain->SetBranchAddress("numSCTSharedHits", &numSCTSharedHits, &b_numSCTSharedHits);
   fChain->SetBranchAddress("numSCTDeadSensors", &numSCTDeadSensors, &b_numSCTDeadSensors);
   fChain->SetBranchAddress("numSCTSpoitHits", &numSCTSpoitHits, &b_numSCTSpoitHits);
   fChain->SetBranchAddress("d0", &d0, &b_d0);
   fChain->SetBranchAddress("d0Err", &d0Err, &b_d0Err);
   fChain->SetBranchAddress("z0", &z0, &b_z0);
   fChain->SetBranchAddress("z0Err", &z0Err, &b_z0Err);
   fChain->SetBranchAddress("numTRTHits", &numTRTHits, &b_numTRTHits);
   fChain->SetBranchAddress("numTRTOutliers", &numTRTOutliers, &b_numTRTOutliers);
   fChain->SetBranchAddress("numTRTHoles", &numTRTHoles, &b_numTRTHoles);
   fChain->SetBranchAddress("numTRTHtHits", &numTRTHtHits, &b_numTRTHtHits);
   fChain->SetBranchAddress("numTRTHTOutliers", &numTRTHTOutliers, &b_numTRTHTOutliers);
   fChain->SetBranchAddress("numTRTDeadStraw", &numTRTDeadStraw, &b_numTRTDeadStraw);
   fChain->SetBranchAddress("numTRTTubeHits", &numTRTTubeHits, &b_numTRTTubeHits);
   fChain->SetBranchAddress("cone30", &cone30, &b_cone30);
   fChain->SetBranchAddress("ethad", &ethad, &b_ethad);
   fChain->SetBranchAddress("ethad1", &ethad1, &b_ethad1);
   fChain->SetBranchAddress("emax", &emax, &b_emax);
   fChain->SetBranchAddress("emax2", &emax2, &b_emax2);
   fChain->SetBranchAddress("emin", &emin, &b_emin);
   fChain->SetBranchAddress("isBMatch", &isBMatch, &b_isBMatch);
   fChain->SetBranchAddress("isCMatch", &isCMatch, &b_isCMatch);
   fChain->SetBranchAddress("isZMatch", &isZMatch, &b_isZMatch);
   fChain->SetBranchAddress("isAuthor", &isAuthor, &b_isAuthor);
   fChain->SetBranchAddress("isSofte", &isSofte, &b_isSofte);
   fChain->SetBranchAddress("elPt", &elPt, &b_elPt);
   fChain->SetBranchAddress("elEta", &elEta, &b_elEta);
   fChain->SetBranchAddress("elPhi", &elPhi, &b_elPhi);
   fChain->SetBranchAddress("elTrnsE", &elTrnsE, &b_elTrnsE);
   fChain->SetBranchAddress("f1", &f1, &b_f1);
   fChain->SetBranchAddress("f1core", &f1core, &b_f1core);
   fChain->SetBranchAddress("emins1", &emins1, &b_emins1);
   fChain->SetBranchAddress("fracs1", &fracs1, &b_fracs1);
   fChain->SetBranchAddress("e2tsts1", &e2tsts1, &b_e2tsts1);
   fChain->SetBranchAddress("weta1", &weta1, &b_weta1);
   fChain->SetBranchAddress("wtots1", &wtots1, &b_wtots1);
   fChain->SetBranchAddress("emaxs1", &emaxs1, &b_emaxs1);
   fChain->SetBranchAddress("e233", &e233, &b_e233);
   fChain->SetBranchAddress("e237", &e237, &b_e237);
   fChain->SetBranchAddress("e277", &e277, &b_e277);
   fChain->SetBranchAddress("weta2", &weta2, &b_weta2);
   fChain->SetBranchAddress("f3", &f3, &b_f3);
   fChain->SetBranchAddress("f3core", &f3core, &b_f3core);
   fChain->SetBranchAddress("etcone", &etcone, &b_etcone);
   fChain->SetBranchAddress("etcone20", &etcone20, &b_etcone20);
   fChain->SetBranchAddress("etcone30", &etcone30, &b_etcone30);
   fChain->SetBranchAddress("etcone40", &etcone40, &b_etcone40);
   fChain->SetBranchAddress("ptcone30", &ptcone30, &b_ptcone30);
   fChain->SetBranchAddress("deltaEta1", &deltaEta1, &b_deltaEta1);
   fChain->SetBranchAddress("deltaEta2", &deltaEta2, &b_deltaEta2);
   fChain->SetBranchAddress("deltaPhi2", &deltaPhi2, &b_deltaPhi2);
   fChain->SetBranchAddress("deltaPhiRescaled", &deltaPhiRescaled, &b_deltaPhiRescaled);
   fChain->SetBranchAddress("expectHitInBLayer", &expectHitInBLayer, &b_expectHitInBLayer);
}

Bool_t ElectronID::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef ElectronID_cxx
