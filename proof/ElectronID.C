#define ElectronID_cxx
// The class definition in ElectronID.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("ElectronID.C")
// Root > T->Process("ElectronID.C","some options")
// Root > T->Process("ElectronID.C+")
//

#include "ElectronID.h"
#include <TH2.h>
#include <TStyle.h>


void ElectronID::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

void ElectronID::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

    BookHistograms();

   TString option = GetOption();

}

Bool_t ElectronID::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either ElectronID::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.
   //

   Long64_t chain_entry = fChain->GetEntryNumber(entry);

   b_elPt                   ->GetEntry(chain_entry);
   b_elPhi                  ->GetEntry(chain_entry);
   b_elTrnsE                ->GetEntry(chain_entry);
   
   b_numBLayerHits          ->GetEntry(chain_entry);
   b_numBLayerOutliers      ->GetEntry(chain_entry);
   b_numBLayerShared        ->GetEntry(chain_entry);
   b_numBLayerExpect        ->GetEntry(chain_entry);

   b_numPixelHits           ->GetEntry(chain_entry);
   b_numPixelOutliers       ->GetEntry(chain_entry);
   b_numPixelShared         ->GetEntry(chain_entry);

   b_numSCTHits             ->GetEntry(chain_entry);
   b_numSCTOutliers         ->GetEntry(chain_entry);
   b_numSCTSharedHits       ->GetEntry(chain_entry);

   b_numTRTHits             ->GetEntry(chain_entry);
   b_numTRTOutliers         ->GetEntry(chain_entry);
   b_numTRTHtHits           ->GetEntry(chain_entry);
   b_numTRTHTOutliers       ->GetEntry(chain_entry);

   b_e237                   ->GetEntry(chain_entry);
   b_e277                   ->GetEntry(chain_entry);

   b_isAuthor               ->GetEntry(chain_entry);
   b_isSofte                ->GetEntry(chain_entry);
   b_isBMatch               ->GetEntry(chain_entry);
   b_isCMatch               ->GetEntry(chain_entry);
   b_isZMatch               ->GetEntry(chain_entry);

   b_ethad                  ->GetEntry(chain_entry);
   b_ethad1                 ->GetEntry(chain_entry);

   b_emax                   ->GetEntry(chain_entry);
   b_emax2                  ->GetEntry(chain_entry);

   b_elPt                   ->GetEntry(chain_entry);
   b_elEta                  ->GetEntry(chain_entry);
   b_elPhi                  ->GetEntry(chain_entry);
   b_elTrnsE                ->GetEntry(chain_entry);

   //Fill the histograms
   for(unsigned int i =0; i < numBLayerHits->size(); ++i)
   {
       if(!(isZMatch->at(i) && isBMatch && isCMatch))
       {
           //Author
           Int_t author         = 0;
           if(isAuthor->at(i))
               author =   1;
           if(isSofte->at(i))
               author =   2;
           if(isAuthor->at(i) && isSofte->at(i))
               author =   3;
           h1_elAuthor          ->Fill(author);

           //Kinematics
           h1_elPt              ->Fill(elPt->at(i));
           h1_elEta             ->Fill(elEta->at(i));
           h1_elPhi             ->Fill(elPhi->at(i));
           h1_elEt              ->Fill(elTrnsE->at(i)/1000);
           
           h1_nBLayerHits       ->Fill(numBLayerHits->at(i) + numBLayerOutliers->at(i));
           h1_nPixelHits        ->Fill(numPixelHits->at(i)  + numPixelOutliers->at(i));
           h1_nSCTHits          ->Fill(numSCTHits->at(i) + numSCTOutliers->at(i));
           h1_nTRTHits          ->Fill(numTRTHits->at(i) + numTRTOutliers->at(i));
           h1_nTRTHtHits        ->Fill(numTRTHtHits->at(i) + numTRTHTOutliers->at(i));
           h1_trtHTHitFraction  ->Fill( (numTRTHtHits->at(i) + numTRTHTOutliers->at(i))/(numTRTHits->at(i) + numTRTOutliers->at(i)));

           h1_hadLeak           ->Fill(ethad->at(i)/elTrnsE->at(i)/1000);
           h1_hadLeak1          ->Fill(ethad1->at(i)/elTrnsE->at(i)/1000);
           h1_reta              ->Fill(e237->at(i)/e277->at(i));
           h1_eRatio            ->Fill(fabs(emax->at(i) - emax2->at(i))/(emax->at(i)+emax2->at(i)));
       }
       if(isBMatch->at(i))
       {
           //Author
           Int_t author         = 0;
           if(isAuthor->at(i))
               author =   1;
           if(isSofte->at(i))
               author =   2;
           if(isAuthor->at(i) && isSofte->at(i))
               author =   3;
           h1_b_elAuthor          ->Fill(author);

           //Kinematics
           h1_b_elPt              ->Fill(elPt->at(i));
           h1_b_elEta             ->Fill(elEta->at(i));
           h1_b_elPhi             ->Fill(elPhi->at(i));
           h1_b_elEt              ->Fill(elTrnsE->at(i)/1000);
           
           h1_b_nBLayerHits       ->Fill(numBLayerHits->at(i) + numBLayerOutliers->at(i));
           h1_b_nPixelHits        ->Fill(numPixelHits->at(i)  + numPixelOutliers->at(i));
           h1_b_nSCTHits          ->Fill(numSCTHits->at(i) + numSCTOutliers->at(i));
           h1_b_nTRTHits          ->Fill(numTRTHits->at(i) + numTRTOutliers->at(i));
           h1_b_nTRTHtHits        ->Fill(numTRTHtHits->at(i) + numTRTHTOutliers->at(i));
           h1_b_trtHTHitFraction  ->Fill( (numTRTHtHits->at(i) + numTRTHTOutliers->at(i))/(numTRTHits->at(i) + numTRTOutliers->at(i)));

           h1_b_hadLeak           ->Fill(ethad->at(i)/elTrnsE->at(i)/1000);
           h1_b_hadLeak1          ->Fill(ethad1->at(i)/elTrnsE->at(i)/1000);
           h1_b_reta              ->Fill(e237->at(i)/e277->at(i));
           h1_b_eRatio            ->Fill(fabs(emax->at(i) - emax2->at(i))/(emax->at(i)+emax2->at(i)));
       }
       if(isCMatch->at(i))
       {
           //Author
           Int_t author         = 0;
           if(isAuthor->at(i))
               author =   1;
           if(isSofte->at(i))
               author =   2;
           if(isAuthor->at(i) && isSofte->at(i))
               author =   3;
           h1_c_elAuthor          ->Fill(author);

           //Kinematics
           h1_c_elPt              ->Fill(elPt->at(i));
           h1_c_elEta             ->Fill(elEta->at(i));
           h1_c_elPhi             ->Fill(elPhi->at(i));
           h1_c_elEt              ->Fill(elTrnsE->at(i)/1000);
           
           h1_c_nBLayerHits       ->Fill(numBLayerHits->at(i) + numBLayerOutliers->at(i));
           h1_c_nPixelHits        ->Fill(numPixelHits->at(i)  + numPixelOutliers->at(i));
           h1_c_nSCTHits          ->Fill(numSCTHits->at(i) + numSCTOutliers->at(i));
           h1_c_nTRTHits          ->Fill(numTRTHits->at(i) + numTRTOutliers->at(i));
           h1_c_nTRTHtHits        ->Fill(numTRTHtHits->at(i) + numTRTHTOutliers->at(i));
           h1_c_trtHTHitFraction  ->Fill( (numTRTHtHits->at(i) + numTRTHTOutliers->at(i))/(numTRTHits->at(i) + numTRTOutliers->at(i)));

           h1_c_hadLeak           ->Fill(ethad->at(i)/elTrnsE->at(i)/1000);
           h1_c_hadLeak1          ->Fill(ethad1->at(i)/elTrnsE->at(i)/1000);
           h1_c_reta              ->Fill(e237->at(i)/e277->at(i));
           h1_c_eRatio            ->Fill(fabs(emax->at(i) - emax2->at(i))/(emax->at(i)+emax2->at(i)));
       }

       if(isZMatch->at(i))
       {
           //Author
           Int_t author         = 0;
           if(isAuthor->at(i))
               author =   1;
           if(isSofte->at(i))
               author =   2;
           if(isAuthor->at(i) && isSofte->at(i))
               author =   3;
           h1_z_elAuthor          ->Fill(author);

           //Kinematics
           h1_z_elPt              ->Fill(elPt->at(i));
           h1_z_elEta             ->Fill(elEta->at(i));
           h1_z_elPhi             ->Fill(elPhi->at(i));
           h1_z_elEt              ->Fill(elTrnsE->at(i)/1000);
           
           h1_z_nBLayerHits       ->Fill(numBLayerHits->at(i) + numBLayerOutliers->at(i));
           h1_z_nPixelHits        ->Fill(numPixelHits->at(i)  + numPixelOutliers->at(i));
           h1_z_nSCTHits          ->Fill(numSCTHits->at(i) + numSCTOutliers->at(i));
           h1_z_nTRTHits          ->Fill(numTRTHits->at(i) + numTRTOutliers->at(i));
           h1_z_nTRTHtHits        ->Fill(numTRTHtHits->at(i) + numTRTHTOutliers->at(i));
           h1_z_trtHTHitFraction  ->Fill( (numTRTHtHits->at(i) + numTRTHTOutliers->at(i))/(numTRTHits->at(i) + numTRTOutliers->at(i)));

           h1_z_hadLeak           ->Fill(ethad->at(i)/elTrnsE->at(i)/1000);
           h1_z_hadLeak1          ->Fill(ethad1->at(i)/elTrnsE->at(i)/1000);
           h1_z_reta              ->Fill(e237->at(i)/e277->at(i));
           h1_z_eRatio            ->Fill(fabs(emax->at(i) - emax2->at(i))/(emax->at(i)+emax2->at(i)));
       }
   }
   return kTRUE;
}

void ElectronID::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void ElectronID::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

    TFile * f = new TFile("output.root","RECREATE");
    fOutput->Write();
    f->Close();

}

void ElectronID::BookHistograms()
{

    //unmatched
    h1_nBLayerHits          = new TH1F("nBLayerHits","Number of B-layer hits;#bLayerhits",50,-0.5,49.5);
    fOutput->Add(h1_nBLayerHits);
    h1_nPixelHits           = new TH1F("nPixelHits","Number of Pixel hits;#pixelhits",50,-0.5,49.5);
    fOutput->Add(h1_nPixelHits);
    h1_nSCTHits             = new TH1F("nSCTHits","Number of SCT hits; #SCT hits",50,-0.5,49.5);
    fOutput->Add(h1_nSCTHits);
    h1_nTRTHits             = new TH1F("nTRTHits","Number of TRT hits; #TRT hits",50,-0.5,49.5);
    fOutput->Add(h1_nTRTHits);
    h1_nTRTHtHits           = new TH1F("nTRTHtHits","Number of TRT high threshold hits;#TRT ht",50,-0.5,49.5);
    fOutput->Add(h1_nTRTHtHits);
    h1_trtHTHitFraction     = new TH1F("trtHtHitFraction","TRT High threshold hit fraction;TRT-HT/TRT-All",150,0,1.5);
    fOutput->Add(h1_trtHTHitFraction);
    h1_reta                 = new TH1F("reta","Reta(e237/e277)",150,0,1.5);
    fOutput->Add(h1_reta);
    h1_elAuthor             = new TH1F("elAuthor","Electron Author",10,-0.5,9.5);
    fOutput->Add(h1_elAuthor);
    h1_hadLeak              = new TH1F("hadLeak","Hadronic leak var (ethad/et)",500,-1,1);
    fOutput->Add(h1_hadLeak);
    h1_hadLeak1             = new TH1F("hadLeak1","Hadronic leak var (ethad1/et)",500,-1,1);
    fOutput->Add(h1_hadLeak1);
    h1_eRatio               = new TH1F("eRatio","(E1-E2)/(E1+E2) in strip layer",100,0,1);
    fOutput->Add(h1_eRatio);
    h1_elPt                 = new TH1F("elPt","Electron pt;[GeV]",500,0,500);
    fOutput->Add(h1_elPt);
    h1_elEta                = new TH1F("elEta","Electron Eta;[GeV]",500,-5,5);
    fOutput->Add(h1_elEta);
    h1_elPhi                = new TH1F("elPhi","Electron Phi",1400,-7,7);
    fOutput->Add(h1_elPhi);
    h1_elEt                 = new TH1F("elEt","Electron Transverse Energy;[GeV]",500,0,500);
    fOutput->Add(h1_elEt);

    //b
    h1_b_nBLayerHits          = new TH1F("b_nBLayerHits","Number of B-layer hits;#bLayerhits",50,-0.5,49.5);
    fOutput->Add(h1_b_nBLayerHits);
    h1_b_nPixelHits           = new TH1F("b_nPixelHits","Number of Pixel hits;#pixelhits",50,-0.5,49.5);
    fOutput->Add(h1_b_nPixelHits);
    h1_b_nSCTHits             = new TH1F("b_nSCTHits","Number of SCT hits; #SCT hits",50,-0.5,49.5);
    fOutput->Add(h1_b_nSCTHits);
    h1_b_nTRTHits             = new TH1F("b_nTRTHits","Number of TRT hits; #TRT hits",50,-0.5,49.5);
    fOutput->Add(h1_b_nTRTHits);
    h1_b_nTRTHtHits           = new TH1F("b_nTRTHtHits","Number of TRT high threshold hits;#TRT ht",50,-0.5,49.5);
    fOutput->Add(h1_b_nTRTHtHits);
    h1_b_trtHTHitFraction     = new TH1F("b_trtHtHitFraction","TRT High threshold hit fraction;TRT-HT/TRT-All",150,0,1.5);
    fOutput->Add(h1_b_trtHTHitFraction);
    h1_b_reta                 = new TH1F("b_reta","Reta(e237/e277)",150,0,1.5);
    fOutput->Add(h1_b_reta);
    h1_b_elAuthor             = new TH1F("b_elAuthor","Electron Author",10,-0.5,9.5);
    fOutput->Add(h1_b_elAuthor);
    h1_b_hadLeak              = new TH1F("b_hadLeak","Hadronic leak var (ethad/et)",500,-1,1);
    fOutput->Add(h1_b_hadLeak);
    h1_b_hadLeak1             = new TH1F("b_hadLeak1","Hadronic leak var (ethad1/et)",500,-1,1);
    fOutput->Add(h1_b_hadLeak1);
    h1_b_eRatio               = new TH1F("b_eRatio","(E1-E2)/(E1+E2) in strip layer",100,0,1);
    fOutput->Add(h1_b_eRatio);
    h1_b_elPt                 = new TH1F("b_elPt","Electron pt;[GeV]",500,0,500);
    fOutput->Add(h1_b_elPt);
    h1_b_elEta                = new TH1F("b_elEta","Electron Eta;[GeV]",500,-5,5);
    fOutput->Add(h1_b_elEta);
    h1_b_elPhi                = new TH1F("b_elPhi","Electron Phi",1400,-7,7);
    fOutput->Add(h1_b_elPhi);
    h1_b_elEt                 = new TH1F("b_elEt","Electron Transverse Energy;[GeV]",500,0,500);
    fOutput->Add(h1_b_elEt);

    //c
    h1_c_nBLayerHits          = new TH1F("c_nBLayerHits","Number of B-layer hits;#bLayerhits",50,-0.5,49.5);
    fOutput->Add(h1_c_nBLayerHits);
    h1_c_nPixelHits           = new TH1F("c_nPixelHits","Number of Pixel hits;#pixelhits",50,-0.5,49.5);
    fOutput->Add(h1_c_nPixelHits);
    h1_c_nSCTHits             = new TH1F("c_nSCTHits","Number of SCT hits; #SCT hits",50,-0.5,49.5);
    fOutput->Add(h1_c_nSCTHits);
    h1_c_nTRTHits             = new TH1F("c_nTRTHits","Number of TRT hits; #TRT hits",50,-0.5,49.5);
    fOutput->Add(h1_c_nTRTHits);
    h1_c_nTRTHtHits           = new TH1F("c_nTRTHtHits","Number of TRT high threshold hits;#TRT ht",50,-0.5,49.5);
    fOutput->Add(h1_c_nTRTHtHits);
    h1_c_trtHTHitFraction     = new TH1F("c_trtHtHitFraction","TRT High threshold hit fraction;TRT-HT/TRT-All",150,0,1.5);
    fOutput->Add(h1_c_trtHTHitFraction);
    h1_c_reta                 = new TH1F("c_reta","Reta(e237/e277)",150,0,1.5);
    fOutput->Add(h1_c_reta);
    h1_c_elAuthor             = new TH1F("c_elAuthor","Electron Author",10,-0.5,9.5);
    fOutput->Add(h1_c_elAuthor);
    h1_c_hadLeak              = new TH1F("c_hadLeak","Hadronic leak var (ethad/et)",500,-1,1);
    fOutput->Add(h1_c_hadLeak);
    h1_c_hadLeak1             = new TH1F("c_hadLeak1","Hadronic leak var (ethad1/et)",500,-1,1);
    fOutput->Add(h1_c_hadLeak1);
    h1_c_eRatio               = new TH1F("c_eRatio","(E1-E2)/(E1+E2) in strip layer",100,0,1);
    fOutput->Add(h1_c_eRatio);
    h1_c_elPt                 = new TH1F("c_elPt","Electron pt;[GeV]",500,0,500);
    fOutput->Add(h1_c_elPt);
    h1_c_elEta                = new TH1F("c_elEta","Electron Eta;[GeV]",500,-5,5);
    fOutput->Add(h1_c_elEta);
    h1_c_elPhi                = new TH1F("c_elPhi","Electron Phi",1400,-7,7);
    fOutput->Add(h1_c_elPhi);
    h1_c_elEt                 = new TH1F("c_elEt","Electron Transverse Energy;[GeV]",500,0,500);
    fOutput->Add(h1_c_elEt);

    //z
    h1_z_nBLayerHits          = new TH1F("z_nBLayerHits","Number of B-layer hits;#bLayerhits",50,-0.5,49.5);
    fOutput->Add(h1_z_nBLayerHits);
    h1_z_nPixelHits           = new TH1F("z_nPixelHits","Number of Pixel hits;#pixelhits",50,-0.5,49.5);
    fOutput->Add(h1_z_nPixelHits);
    h1_z_nSCTHits             = new TH1F("z_nSCTHits","Number of SCT hits; #SCT hits",50,-0.5,49.5);
    fOutput->Add(h1_z_nSCTHits);
    h1_z_nTRTHits             = new TH1F("z_nTRTHits","Number of TRT hits; #TRT hits",50,-0.5,49.5);
    fOutput->Add(h1_z_nTRTHits);
    h1_z_nTRTHtHits           = new TH1F("z_nTRTHtHits","Number of TRT high threshold hits;#TRT ht",50,-0.5,49.5);
    fOutput->Add(h1_z_nTRTHtHits);
    h1_z_trtHTHitFraction     = new TH1F("z_trtHtHitFraction","TRT High threshold hit fraction;TRT-HT/TRT-All",150,0,1.5);
    fOutput->Add(h1_z_trtHTHitFraction);
    h1_z_reta                 = new TH1F("z_reta","Reta(e237/e277)",150,0,1.5);
    fOutput->Add(h1_z_reta);
    h1_z_elAuthor             = new TH1F("z_elAuthor","Electron Author",10,-0.5,9.5);
    fOutput->Add(h1_z_elAuthor);
    h1_z_hadLeak              = new TH1F("z_hadLeak","Hadronic leak var (ethad/et)",500,-1,1);
    fOutput->Add(h1_z_hadLeak);
    h1_z_hadLeak1             = new TH1F("z_hadLeak1","Hadronic leak var (ethad1/et)",500,-1,1);
    fOutput->Add(h1_z_hadLeak1);
    h1_z_eRatio               = new TH1F("z_eRatio","(E1-E2)/(E1+E2) in strip layer",100,0,1);
    fOutput->Add(h1_z_eRatio);
    h1_z_elPt                 = new TH1F("z_elPt","Electron pt;[GeV]",500,0,500);
    fOutput->Add(h1_z_elPt);
    h1_z_elEta                = new TH1F("z_elEta","Electron Eta;[GeV]",500,-5,5);
    fOutput->Add(h1_z_elEta);
    h1_z_elPhi                = new TH1F("z_elPhi","Electron Phi",1400,-7,7);
    fOutput->Add(h1_z_elPhi);
    h1_z_elEt                 = new TH1F("z_elEt","Electron Transverse Energy;[GeV]",500,0,500);
    fOutput->Add(h1_z_elEt);
}
