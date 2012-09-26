//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Sep 24 21:34:09 2012 by ROOT version 5.28/00g
// from TTree elId/Electron ID Variables
// found on file: ../share/softElectron.root
//////////////////////////////////////////////////////////

#ifndef ElectronID_h
#define ElectronID_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH1F.h>
#include <TH2F.h>

class ElectronID : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   TH1F*    h1_elPt;
   TH1F*    h1_elEta;
   TH1F*    h1_elPhi;
   TH1F*    h1_elEt;
   TH1F*    h1_nBLayerHits;
   TH1F*    h1_nPixelHits;
   TH1F*    h1_nSCTHits;
   TH1F*    h1_nTRTHits;
   TH1F*    h1_nTRTHtHits;
   TH1F*    h1_trtHTHitFraction;
   TH1F*    h1_reta;
   TH1F*    h1_elAuthor;
   TH1F*    h1_hadLeak;
   TH1F*    h1_hadLeak1;
   TH1F*    h1_eRatio;

   TH1F*    h1_b_elPt;
   TH1F*    h1_b_elEta;
   TH1F*    h1_b_elPhi;
   TH1F*    h1_b_elEt;
   TH1F*    h1_b_nBLayerHits;
   TH1F*    h1_b_nPixelHits;
   TH1F*    h1_b_nSCTHits;
   TH1F*    h1_b_nTRTHits;
   TH1F*    h1_b_nTRTHtHits;
   TH1F*    h1_b_trtHTHitFraction;
   TH1F*    h1_b_reta;
   TH1F*    h1_b_elAuthor;
   TH1F*    h1_b_hadLeak;
   TH1F*    h1_b_hadLeak1;
   TH1F*    h1_b_eRatio;

   TH1F*    h1_c_elPt;
   TH1F*    h1_c_elEta;
   TH1F*    h1_c_elPhi;
   TH1F*    h1_c_elEt;
   TH1F*    h1_c_nBLayerHits;
   TH1F*    h1_c_nPixelHits;
   TH1F*    h1_c_nSCTHits;
   TH1F*    h1_c_nTRTHits;
   TH1F*    h1_c_nTRTHtHits;
   TH1F*    h1_c_trtHTHitFraction;
   TH1F*    h1_c_reta;
   TH1F*    h1_c_elAuthor;
   TH1F*    h1_c_hadLeak;
   TH1F*    h1_c_hadLeak1;
   TH1F*    h1_c_eRatio;

   TH1F*    h1_z_elPt;
   TH1F*    h1_z_elEta;
   TH1F*    h1_z_elPhi;
   TH1F*    h1_z_elEt;
   TH1F*    h1_z_nBLayerHits;
   TH1F*    h1_z_nPixelHits;
   TH1F*    h1_z_nSCTHits;
   TH1F*    h1_z_nTRTHits;
   TH1F*    h1_z_nTRTHtHits;
   TH1F*    h1_z_trtHTHitFraction;
   TH1F*    h1_z_reta;
   TH1F*    h1_z_elAuthor;
   TH1F*    h1_z_hadLeak;
   TH1F*    h1_z_hadLeak1;
   TH1F*    h1_z_eRatio;


   // Declaration of leaf types
   vector<float>   *numBLayerHits;
   vector<float>   *numBLayerOutliers;
   vector<float>   *numBLayerShared;
   vector<float>   *numBLayerExpect;
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
   vector<float>   *numTRTHits;
   vector<float>   *numTRTOutliers;
   vector<float>   *numTRTHoles;
   vector<float>   *numTRTHtHits;
   vector<float>   *numTRTHTOutliers;
   vector<float>   *numTRTDeadStraw;
   vector<float>   *numTRTTubeHits;
   vector<float>   *e237;
   vector<float>   *e277;
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

   // List of branches
   TBranch        *b_numBLayerHits;   //!
   TBranch        *b_numBLayerOutliers;   //!
   TBranch        *b_numBLayerShared;   //!
   TBranch        *b_numBLayerExpect;   //!
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
   TBranch        *b_numTRTHits;   //!
   TBranch        *b_numTRTOutliers;   //!
   TBranch        *b_numTRTHoles;   //!
   TBranch        *b_numTRTHtHits;   //!
   TBranch        *b_numTRTHTOutliers;   //!
   TBranch        *b_numTRTDeadStraw;   //!
   TBranch        *b_numTRTTubeHits;   //!
   TBranch        *b_e237;   //!
   TBranch        *b_e277;   //!
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

   ElectronID(TTree * /*tree*/ =0) 
   {
       h1_elPt              = 0;
       h1_elEta             = 0;
       h1_elPhi             = 0;
       h1_elEt              = 0;
       h1_nBLayerHits       = 0;
       h1_nPixelHits        = 0;
       h1_nSCTHits          = 0;
       h1_nTRTHits          = 0; 
       h1_nTRTHtHits        = 0;
       h1_trtHTHitFraction  = 0;
       h1_reta              = 0;
       h1_elAuthor          = 0;
       h1_hadLeak           = 0;
       h1_hadLeak1          = 0;
       h1_eRatio            = 0;

       h1_b_elPt              = 0;
       h1_b_elEta             = 0;
       h1_b_elPhi             = 0;
       h1_b_elEt              = 0;
       h1_b_nBLayerHits       = 0;
       h1_b_nPixelHits        = 0;
       h1_b_nSCTHits          = 0;
       h1_b_nTRTHits          = 0; 
       h1_b_nTRTHtHits        = 0;
       h1_b_trtHTHitFraction  = 0;
       h1_b_reta              = 0;
       h1_b_elAuthor          = 0;
       h1_b_hadLeak           = 0;
       h1_b_hadLeak1          = 0;
       h1_b_eRatio            = 0;

       h1_c_elPt              = 0;
       h1_c_elEta             = 0;
       h1_c_elPhi             = 0;
       h1_c_elEt              = 0;
       h1_c_nBLayerHits       = 0;
       h1_c_nPixelHits        = 0;
       h1_c_nSCTHits          = 0;
       h1_c_nTRTHits          = 0; 
       h1_c_nTRTHtHits        = 0;
       h1_c_trtHTHitFraction  = 0;
       h1_c_reta              = 0;
       h1_c_elAuthor          = 0;
       h1_c_hadLeak           = 0;
       h1_c_hadLeak1          = 0;
       h1_c_eRatio            = 0;

       h1_z_elPt              = 0;
       h1_z_elEta             = 0;
       h1_z_elPhi             = 0;
       h1_z_elEt              = 0;
       h1_z_nBLayerHits       = 0;
       h1_z_nPixelHits        = 0;
       h1_z_nSCTHits          = 0;
       h1_z_nTRTHits          = 0; 
       h1_z_nTRTHtHits        = 0;
       h1_z_trtHTHitFraction  = 0;
       h1_z_reta              = 0;
       h1_z_elAuthor          = 0;
       h1_z_hadLeak           = 0;
       h1_z_hadLeak1          = 0;
       h1_z_eRatio            = 0;


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

   void    BookHistograms();

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
   numBLayerExpect = 0;
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
   numTRTHits = 0;
   numTRTOutliers = 0;
   numTRTHoles = 0;
   numTRTHtHits = 0;
   numTRTHTOutliers = 0;
   numTRTDeadStraw = 0;
   numTRTTubeHits = 0;
   e237 = 0;
   e277 = 0;
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
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("numBLayerHits", &numBLayerHits, &b_numBLayerHits);
   fChain->SetBranchAddress("numBLayerOutliers", &numBLayerOutliers, &b_numBLayerOutliers);
   fChain->SetBranchAddress("numBLayerShared", &numBLayerShared, &b_numBLayerShared);
   fChain->SetBranchAddress("numBLayerExpect", &numBLayerExpect, &b_numBLayerExpect);
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
   fChain->SetBranchAddress("numTRTHits", &numTRTHits, &b_numTRTHits);
   fChain->SetBranchAddress("numTRTOutliers", &numTRTOutliers, &b_numTRTOutliers);
   fChain->SetBranchAddress("numTRTHoles", &numTRTHoles, &b_numTRTHoles);
   fChain->SetBranchAddress("numTRTHtHits", &numTRTHtHits, &b_numTRTHtHits);
   fChain->SetBranchAddress("numTRTHTOutliers", &numTRTHTOutliers, &b_numTRTHTOutliers);
   fChain->SetBranchAddress("numTRTDeadStraw", &numTRTDeadStraw, &b_numTRTDeadStraw);
   fChain->SetBranchAddress("numTRTTubeHits", &numTRTTubeHits, &b_numTRTTubeHits);
   fChain->SetBranchAddress("e237", &e237, &b_e237);
   fChain->SetBranchAddress("e277", &e277, &b_e277);
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
