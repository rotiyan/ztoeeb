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
#include <TLorentzVector.h>
#include <algorithm>


bool sortDescend(float a, float b) {
    return a > b;
}

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
   this->BookHistograms();

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
   Long64_t chain_entry = fChain->GetEntryNumber(entry);
   Long64_t i_entry     = fChain->GetTree()->GetEntry(chain_entry);
   //b_numBLayerHits->GetEntry(chain_entry);
   

   bool isZEvent    = false;
   //check if the event is a Z-Event
   std::vector<float> maxPtList  = this->getMaxPtList(elPt);
   float leadPt     = 0;
   float subleadPt  = 0;
   float leadEta    = 0;
   float leadPhi    = 0;
   float elMass     = 0.000511;
   float subleadEta = 0;
   float subleadPhi = 0;

   if(maxPtList.size() >1)
   {
       leadPt           = maxPtList.at(0);
       subleadPt        = maxPtList.at(1);
   }

   for (unsigned i =0; i < numBLayerHits->size(); i++)
   {

       float pt     = elPt->at(i);
       float eta    = elEta->at(i);
       float phi    = elPhi->at(i);
       if(pt == leadPt)
       {
           leadEta  = eta;
           leadPhi  = phi;
       }
       if(pt == subleadPt)
       {
           subleadEta = eta;
           subleadPhi = phi;
       }
   }
   TLorentzVector el1Vec,el2Vec;
   el1Vec.SetPtEtaPhiM(leadPt,leadEta,leadPhi,elMass);
   el2Vec.SetPtEtaPhiM(subleadPt,subleadEta,subleadPhi,elMass);
   float invmass = ( (el1Vec + el2Vec ).M());
   if(invmass > 66 && invmass < 116)
   {
       h_zMass->Fill(invmass);
       isZEvent = true;
   }

   //plot other electrons
   if(isZEvent)
   {
       float nSofte             = 0;
       float ntruthMatchSofte   = 0;
       float nAuthor            = 0;
       float ntruthMatchAuthor  = 0;
       for(unsigned int i =0; i < numBLayerHits->size();i++)
       {
           float pt     = elPt->at(i);
           //Exclude Z Electrons
           if(!(pt == leadPt || pt == subleadPt))
           {
               //E Ratio's 
               if(e233->at(i) != -100)
               {
                   h_e233->Fill(e233->at(i));
                   if(e237->at(i) !=0)
                       h_RPhi->Fill((e233->at(i))/(e237->at(i)));
               }
               if(e237->at(i) != -100)
                   h_e237->Fill(e237->at(i));

               if(e277->at(i) !=-100)
               {
                   h_e277->Fill(e277->at(i));   
                   if(e277->at(i) !=0)
                       h_REta->Fill((e237->at(i))/e237->at(i));
               }
               if(isSofte->at(i))
               {
                   nSofte++;
               }
               if(isAuthor->at(i)) 
               {
                   nAuthor++;
               }

               //TRT Ratio
               float trtHits        = numTRTHits->at(i);
               float trtHtHits      = numTRTHtHits->at(i);
               float trtOutliers    = numTRTOutliers->at(i);
               float trtHtOutliers  = numTRTHTOutliers->at(i);

               if( (trtHits + trtOutliers) !=0  && trtHtHits !=-100 && trtHtOutliers !=-100)
               {
                   h_TRTRatio->Fill((trtHtHits + trtHtOutliers)/(trtHits + trtOutliers));
                   if(isBMatch->at(i) == true)
                   {
                       if(isSofte->at(i)==true)
                       {
                           ntruthMatchSofte++;
                       }
                       if(isAuthor->at(i)==true)
                       {
                           ntruthMatchAuthor++;
                       }

                       h_truthMatchTRTRatio->Fill( (trtHtHits + trtHtOutliers)/(trtHits + trtOutliers) );
                       h_truthMatchElPt->Fill(elPt->at(i));
                       h_truthMatchd0->Fill(d0->at(i));
                   }
               }
               this->FillTrivialHists(i);
           }
       }
       h_nSofte->Fill(nSofte);
       h_truthMatchNSofte->Fill(ntruthMatchSofte);
       h_nAuthor->Fill(nAuthor);
       h_truthMatchNAuthor->Fill(ntruthMatchAuthor);
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
    h_truthRecoPt           = new TH1F("h_truthRecoPt"              , "h_truthRecoPt"           ,600,0,300);
    h_truthRecoEta          = new TH1F("h_truthRecoEta"             , "h_truthRecoEta"          ,100,-5,5);
    h_truthRecoPhi          = new TH1F("h_truthRecoPhi"             , "h_truthRecoPhi"          ,100,-5,5);
    h_truthRecoEnrgy        = new TH1F("h_truthRecoEnrgy"           , "h_truthRecoEnrgy"        ,600,0,300);

    h_REta                  =  new TH1F("h_REta"                    ,  "h_REta"                 ,100,0,10);
    h_RPhi                  =  new TH1F("h_RPhi"                    ,  "h_RPhi"                 ,100,0,10);
    h_zMass                 =  new TH1F("h_zMass"                   ,  "h_zMass"                ,400,0,200);
    h_nSofte                =  new TH1F("h_nSofte"                  ,  "h_nSofte"               ,20,-0.5,19.5);
    h_truthMatchNSofte      =  new TH1F("h_truthMatchNSofte"        ,  "h_truthMatchNSofte"     ,20,-0.5,19.5);
    h_nAuthor               =  new TH1F("h_nAuthor"                 ,  "h_nAuthor"              ,20,-0.5,19.5);
    h_truthMatchNAuthor     =  new TH1F("h_truthMatchNAuthor"       ,  "h_truthMatchNAuthor"    ,20,-0.5,19.5);
    h_TRTRatio              =  new TH1F("h_TRTRatio"                ,  "h_TRTRatio"             ,20,0,1);
    h_truthMatchTRTRatio    =  new TH1F("h_truthMatchTRTRatio"      ,  "h_truthMatchTRTRatio"   ,20,0,1);

    h_numBLayerHits	        =  new TH1F("h_numBLayerHits"           ,  "h_numBLayerHit"         ,50,-0.5,49.5);
    h_numBLayerOutliers	    =  new TH1F("h_numBLayerOutliers"       ,  "h_numBLayerOutliers"    ,50,-0.5,49.5);
    h_numBLayerShared	    =  new TH1F("h_numBLayerShared"         ,  "h_numBLayerShared"      ,50,-0.5,49.5);
    h_numPixelHits	        =  new TH1F("h_numPixelHits"            ,  "h_numPixelHits"         ,50,-0.5,49.5);                                                          
    h_numPixelOutliers	    =  new TH1F("h_numPixelOutliers"        ,  "h_numPixelOutliers"     ,50,-0.5,49.5);   
    h_numPixelHoles	        =  new TH1F("h_numPixelHoles"           ,  "h_numPixelHoles"        ,50,-0.5,49.5);  
    h_numPixelShared	    =  new TH1F("h_numPixelShared"          ,  "h_numPixelShared"       ,50,-0.5,49.5);  
    h_numGangedPixels	    =  new TH1F("h_numGangedPixels"         ,  "h_numGangedPixels"      ,50,-0.5,49.5); 
    h_numGangedFlaggedFakes	=  new TH1F("h_numGangedFlaggedFakes"   ,  "h_numGangedFlaggedFak"  ,50,-0.5,49.5);  
    h_numPixelDeadSensors	=  new TH1F("h_numPixelDeadSensors"     ,  "h_numPixelDeadSensors"  ,50,-0.5,49.5);
    h_numPixelSpoiltHits	=  new TH1F("h_numPixelSpoiltHits"      ,  "h_numPixelSpoiltHits"   ,50,-0.5,49.5);
    h_numSCTHits	        =  new TH1F("h_numSCTHits"              ,  "h_numSCTHits"           ,50,-0.5,49.5);
    h_numSCTOutliers	    =  new TH1F("h_numSCTOutliers"          ,  "h_numSCTOutliers"       ,50,-0.5,49.5);                      
    h_numSCTHoles	        =  new TH1F("h_numSCTHoles"             ,  "h_numSCTHoles"          ,50,-0.5,49.5);
    h_numSCTDoubleHoles	    =  new TH1F("h_numSCTDoubleHoles"       ,  "h_numSCTDoubleHoles"    ,50,-0.5,49.5);
    h_numSCTSharedHits	    =  new TH1F("h_numSCTSharedHits"        ,  "h_numSCTSharedHits"     ,50,-0.5,49.5);
    h_numSCTDeadSensors	    =  new TH1F("h_numSCTDeadSensors"       ,  "h_numSCTDeadSensors"    ,50,-0.5,49.5);
    h_numSCTSpoitHits	    =  new TH1F("h_numSCTSpoitHits"         ,  "h_numSCTSpoitHits"      ,50,-0.5,49.5);
    h_numTRTHits	        =  new TH1F("h_numTRTHits"              ,  "h_numTRTHits"           ,50,-0.5,49.5);
    h_numTRTOutliers        =  new TH1F("h_numTRTOutliers"          ,  "h_numTRTOutliers"       ,50,-0.5,49.5);
    h_numTRTHoles           =  new TH1F("h_numTRTHoles"             ,  "h_numTRTHoles"          ,50,-0.5,49.5);
    h_numTRTHtHits          =  new TH1F("h_numTRTHtHits"            ,  "h_numTRTHtHits"         ,50,-0.5,49.5);
    h_numTRTHTOutliers      =  new TH1F("h_numTRTHTOutliers"        ,  "h_numTRTHTOutliers"     ,50,-0.5,49.5);
    h_numTRTDeadStraw       =  new TH1F("h_numTRTDeadStraw"         ,  "h_numTRTDeadStraw"      ,50,-0.5,49.5);
    h_numTRTTubeHits        =  new TH1F("h_numTRTTubeHits"          ,  "h_numTRTTubeHits"       ,50,-0.5,49.5);
    h_d0	                =  new TH1F("h_d0"                      ,  "h_d0"                   ,40,-10,10);
    h_truthMatchd0          =  new TH1F("h_truthMatchd0"            ,  "h_truthMatchd0"         ,40,-10,10);
    h_d0Err	                =  new TH1F("h_d0Err"                   ,  "h_d0Err"                ,1000,0,100);
    h_z0	                =  new TH1F("h_z0"                      ,  "h_z0"                   ,1000,0,100);
    h_z0Err	                =  new TH1F("h_z0Err"                   ,  "h_z0Err"                ,1000,0,100);
    h_cone30	            =  new TH1F("h_cone30"                  ,  "h_cone30"               ,1000,0,1000);
    h_ethad	                =  new TH1F("h_ethad"                   ,  "h_ethad"                ,1000,0,1000);
    h_ethad1	            =  new TH1F("h_ethad1"                  ,  "h_ethad1"               ,1000,0,1000);
    h_emax	                =  new TH1F("h_ema"                     ,  "h_emax"                 ,1000,0,1000);
    h_emax2	                =  new TH1F("h_emax2"                   ,  "h_emax2"                ,1000,0,1000);
    h_emin                  =  new TH1F("h_emin"                    ,  "h_emin"                 ,1000,0,1000);
    h_elPt                  =  new TH1F("h_elPt"                    ,  "h_elPt"                 ,300,0,300);
    h_truthMatchElPt        =  new TH1F("h_truthMatchElPt"          ,  "h_truthMatchElPt"       ,300,0,300);
    h_elEta                 =  new TH1F("h_elEta"                   ,  "h_elEta"                ,1000,-5,5);
    h_elPhi                 =  new TH1F("h_elPhi"                   ,  "h_elPhi"                ,1000,-5,5);                                                
    h_elTrnsE               =  new TH1F("h_elTrnsE"                 ,  "h_elTrnsE"              ,1000,0,1000);                                   
    h_f1	                =  new TH1F("h_f1"                      ,  "h_f1"                   ,1000,0,10);                        
    h_f1core	            =  new TH1F("h_f1core"                  ,  "h_f1core"               ,1000,0,10);           
    h_emins1	            =  new TH1F("h_emins1"                  ,  "h_emins1"               ,1000,0,1000);
    h_fracs1	            =  new TH1F("h_fracs1"                  ,  "h_fracs1"               ,1000,0,1000);
    h_e2tsts1	            =  new TH1F("h_e2tsts1"                 ,  "h_e2tsts1"              ,1000,0,1000);
    h_weta1	                =  new TH1F("h_weta1"                   ,  "h_weta1"                ,1000,0,1000);
    h_wtots1	            =  new TH1F("h_wtots1"                  ,  "h_wtots1"               ,1000,0,1000);
    h_emaxs1	            =  new TH1F("h_emaxs1"                  ,  "h_emaxs1"               ,1000,0,1000);
    h_e233	                =  new TH1F("h_e233"                    ,  "h_e233"                 ,1000,0,10);
    h_e237	                =  new TH1F("h_e237"                    ,  "h_e237"                 ,1000,0,10);
    h_e277	                =  new TH1F("h_e277"                    ,  "h_e277"                 ,1000,0,10);
    h_weta2	                =  new TH1F("h_weta2"                   ,  "h_weta2"                ,1000,0,1000);
    h_f3	                =  new TH1F("h_f3"                      ,  "h_f3"                   ,1000,0,1000);
    h_f3core	            =  new TH1F("h_f3core"                  ,  "h_f3core"               ,1000,0,1000);
    h_etcone	            =  new TH1F("h_etcone"                  ,  "h_etcone"               ,1000,0,1000);
    h_etcone20              =  new TH1F("h_etcone20"                ,  "h_etcone20"             ,1000,0,1000);
    h_etcone30              =  new TH1F("h_etcone30"                ,  "h_etcone30"             ,1000,0,1000);
    h_etcone40              =  new TH1F("h_etcone40"                ,  "h_etcone40"             ,1000,0,1000);
    h_ptcone30              =  new TH1F("h_ptcone30"                ,  "h_ptcone30"             ,1000,0,1000);
    h_deltaEta1             =  new TH1F("h_deltaEta1"               ,  "h_deltaEta1"            ,1000,0,1000);
    h_deltaEta2             =  new TH1F("h_deltaEta2"               ,  "h_deltaEta2"            ,1000,0,1000);
    h_deltaPhi2             =  new TH1F("h_deltaPhi2"               ,  "h_deltaPhi2"            ,1000,0,1000);
    h_deltaPhiRescaled      =  new TH1F("h_deltaPhiRescaled"        ,  "h_deltaPhiRescaled"     ,1000,0,1000);      

    std::vector<TH1F*> histVector;

    histVector.push_back(h_truthRecoPt);
    histVector.push_back(h_truthRecoEnrgy);

    histVector.push_back(h_REta);
    histVector.push_back(h_RPhi);
    histVector.push_back(h_zMass);
    histVector.push_back(h_nSofte);
    histVector.push_back(h_truthMatchNSofte);
    histVector.push_back(h_nAuthor);
    histVector.push_back(h_truthMatchNAuthor);
    histVector.push_back(h_TRTRatio);
    histVector.push_back(h_truthMatchTRTRatio);

    histVector.push_back(h_numBLayerHits);    
    histVector.push_back(h_numBLayerOutliers);    
    histVector.push_back(h_numBLayerShared);
    histVector.push_back(h_numPixelHits);
    histVector.push_back(h_numPixelOutliers);
    histVector.push_back(h_numPixelHoles);    
    histVector.push_back(h_numPixelShared);
    histVector.push_back(h_numGangedPixels);
    histVector.push_back(h_numGangedFlaggedFakes);	
    histVector.push_back(h_numPixelDeadSensors);
    histVector.push_back(h_numPixelSpoiltHits);
    histVector.push_back(h_numSCTHits);
    histVector.push_back(h_numSCTOutliers);
    histVector.push_back(h_numSCTHoles);
    histVector.push_back(h_numSCTDoubleHoles);    
    histVector.push_back(h_numSCTSharedHits);
    histVector.push_back(h_numSCTDeadSensors);    
    histVector.push_back(h_numSCTSpoitHits);
    histVector.push_back(h_numTRTHits);
    histVector.push_back(h_numTRTOutliers); 
    histVector.push_back(h_numTRTHoles); 
    histVector.push_back(h_numTRTHtHits); 
    histVector.push_back(h_numTRTHTOutliers); 
    histVector.push_back(h_numTRTDeadStraw); 
    histVector.push_back(h_numTRTTubeHits); 
    histVector.push_back(h_d0);
    histVector.push_back(h_truthMatchd0);
    histVector.push_back(h_d0Err);    
    histVector.push_back(h_z0);
    histVector.push_back(h_z0Err);    
    histVector.push_back(h_cone30);
    histVector.push_back(h_ethad);    
    histVector.push_back(h_ethad1);
    histVector.push_back(h_emax);
    histVector.push_back(h_emax2);    
    histVector.push_back(h_emin); 
    histVector.push_back(h_elPt); 
    histVector.push_back(h_truthMatchElPt);
    histVector.push_back(h_elEta); 
    histVector.push_back(h_elPhi); 
    histVector.push_back(h_elTrnsE); 
    histVector.push_back(h_f1);
    histVector.push_back(h_f1core);
    histVector.push_back(h_emins1);
    histVector.push_back(h_fracs1);
    histVector.push_back(h_e2tsts1);
    histVector.push_back(h_weta1);    
    histVector.push_back(h_wtots1);
    histVector.push_back(h_emaxs1);
    histVector.push_back(h_e233);
    histVector.push_back(h_e237);
    histVector.push_back(h_e277);
    histVector.push_back(h_weta2);    
    histVector.push_back(h_f3);
    histVector.push_back(h_f3core);
    histVector.push_back(h_etcone);
    histVector.push_back(h_etcone20); 
    histVector.push_back(h_etcone30); 
    histVector.push_back(h_etcone40); 
    histVector.push_back(h_ptcone30); 
    histVector.push_back(h_deltaEta1); 
    histVector.push_back(h_deltaEta2); 
    histVector.push_back(h_deltaPhi2); 
    histVector.push_back(h_deltaPhiRescaled); 

    for(std::vector<TH1F*>::iterator iter = histVector.begin(); iter != histVector.end(); ++iter)
    {
        fOutput->Add((*iter));
    }
}

//Returns the ptlist in descending order
std::vector<float> ElectronID::getMaxPtList(std::vector<float>* vec)
{
    std::vector<float> ptlist = *vec;
    sort(ptlist.begin(), ptlist.end(),sortDescend);
    return ptlist;
}


void ElectronID::FillTrivialHists(int i)
{
    h_numBLayerHits      ->Fill(numBLayerHits->at(i));
    h_numBLayerOutliers	->Fill(numBLayerOutliers->at(i));
    h_numBLayerShared	->Fill(numBLayerShared->at(i));
    h_numPixelHits	    ->Fill(numPixelHits->at(i));
    h_numPixelOutliers	->Fill(numPixelOutliers->at(i));
    h_numPixelHoles	    ->Fill(numPixelHoles->at(i));
    h_numPixelShared	    ->Fill(numPixelShared->at(i));
    h_numGangedPixels	->Fill(numGangedPixels->at(i));
    h_numGangedFlaggedFakes->Fill(numGangedFlaggedFakes->at(i));
    h_numPixelDeadSensors->Fill(numPixelDeadSensors->at(i));
    h_numPixelSpoiltHits ->Fill(numPixelSpoiltHits->at(i));
    h_numSCTHits	        ->Fill(numSCTHits->at(i));
    h_numSCTOutliers	    ->Fill(numSCTOutliers->at(i));
    h_numSCTHoles	    ->Fill(numSCTHoles->at(i));
    h_numSCTDoubleHoles  ->Fill(numSCTDoubleHoles ->at(i));
    h_numSCTSharedHits   ->Fill(numSCTSharedHits->at(i));
    h_numSCTDeadSensors  ->Fill(numSCTDeadSensors->at(i));
    h_numSCTSpoitHits    ->Fill(numSCTSpoitHits->at(i));
    h_numTRTHits	        ->Fill(numTRTHits->at(i));
    h_numTRTOutliers     ->Fill(numTRTOutliers->at(i));
    h_numTRTHoles        ->Fill(numTRTHoles->at(i));
    h_numTRTHtHits       ->Fill(numTRTHtHits->at(i));
    h_numTRTHTOutliers   ->Fill(numTRTHTOutliers->at(i));
    h_numTRTDeadStraw    ->Fill(numTRTDeadStraw->at(i));
    h_numTRTTubeHits     ->Fill(numTRTTubeHits->at(i));
    h_d0	                ->Fill(d0->at(i));
    h_d0Err	            ->Fill(d0Err->at(i));
    h_z0	                ->Fill(z0->at(i));
    //h_z0Err	            ->Fill(z0Err->at(i));
    //h_cone30	            ->Fill(cone30->at(i));
    h_ethad	            ->Fill(ethad->at(i));
    h_ethad1	            ->Fill(ethad1->at(i));
    h_emax	            ->Fill(emax	 ->at(i));
    h_emax2	            ->Fill(emax2->at(i));
    h_emin               ->Fill(emin ->at(i));

    h_elPt               ->Fill(elPt->at(i));
    h_elEta              ->Fill(elEta->at(i));
    h_elPhi              ->Fill(elPhi->at(i));
    h_elTrnsE            ->Fill(elTrnsE->at(i));
    h_f1	                ->Fill(f1->at(i));
    h_f1core	            ->Fill(f1core->at(i));
    h_emins1	            ->Fill(emins1->at(i));
    h_fracs1	            ->Fill(fracs1->at(i));
    h_e2tsts1	        ->Fill(e2tsts1->at(i));
    h_weta1	            ->Fill(weta1->at(i));
    h_wtots1	            ->Fill(wtots1->at(i));
    h_emaxs1	            ->Fill(emaxs1->at(i));
    h_weta2	            ->Fill(weta2->at(i));
    h_f3	                ->Fill(f3->at(i));
    h_f3core	            ->Fill(f3core->at(i));
    h_etcone	            ->Fill(etcone->at(i));
    //h_etcone20           ->Fill(etcone20->at(i));
    //h_etcone30           ->Fill(etcone30->at(i));
    //h_etcone40           ->Fill(etcone40->at(i));
    //h_ptcone30           ->Fill(ptcone30->at(i));
    h_deltaEta1          ->Fill(deltaEta1->at(i));
    h_deltaEta2          ->Fill(deltaEta2->at(i));
    h_deltaPhi2          ->Fill(deltaPhi2->at(i));
    h_deltaPhiRescaled   ->Fill(deltaPhiRescaled ->at(i));
}
