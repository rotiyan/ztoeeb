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
#include <TLorentzVector.h>
#include <TStyle.h>
#include <TFile.h>

#include <algorithm>
#include <stdio.h>

bool sortDescend(float a, float b) 
{
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

   TString option = GetOption();
   this->BookHistograms();

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
   //Long64_t chain_entry = fChain->GetEntryNumber(entry);
   //Long64_t i_entry     = fChain->GetTree()->GetEntry(chain_entry);

   this->LoadBranches(entry);

   this->DoTruthAna();
   this->DoRecoAna();
   

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
   //
   TFile *f = new TFile("output.root","RECREATE");
   fOutput->Write();
   f->Close();


}

void ElectronID::BookHistograms()
{
    h_zInvMass              = new TH1F("zInvMass","zInvMass",200,50,150);
    fOutput->Add(h_zInvMass);

    h_zElPt                 = new TH1F("zElPt","zElPt",400,0,400);
    fOutput->Add(h_zElPt);

    h_zElEta                = new TH1F("zElEta","zElEta",500,-5,5);
    fOutput->Add(h_zElEta);
            
    h_zElPhi                = new TH1F("zElPhi","zElPhi",500,-5,5);
    fOutput->Add(h_zElPhi);

    h_elPt                  = new TH1F("elPt","elPt",500,0,200);
    fOutput->Add(h_elPt);

    h_elEta                 = new TH1F("elEta","elEta",500,-5,5);
    fOutput->Add(h_elEta);

    h_elPhi                 = new TH1F("elPhi","elPhi",500,-5,5);
    fOutput->Add(h_elPhi);

    h_elCharge              = new TH1F("elCharge","elCharge",20,-10,10);
    fOutput->Add(h_elCharge);

    h_nePlus                = new TH1F("nePlus","nePlus",50,-0.5,49.5);
    fOutput->Add(h_nePlus);

    h_neMinus               = new TH1F("neMinus","neMinus",50,-0.5,49.5);
    fOutput->Add(h_neMinus);

    //Electron ID variables
    h_trtHits               = new TProfile("trtHits","trtHits",200,0,400);
    fOutput->Add(h_trtHits);

    h_trtHtHits             = new TProfile("trtHtHits","trtHtHits",200,0,400);
    fOutput->Add(h_trtHtHits);

    h_trtHtHitRatio         = new TProfile("trtHtHitRatio","trtHtHitRatio",200,0,400);
    fOutput->Add(h_trtHtHitRatio);

    h_trtHtOutlierRatio     = new TProfile("trtHtOutlierRatio","trtHtOutlierRatio",200,0,400);
    fOutput->Add(h_trtHtOutlierRatio);
    
    h_elReta                =  new TProfile("h_elReta","h_elReta",200,0,400);
    fOutput->Add(h_elReta);

    h_elRPhi                =  new TProfile("h_elRPhi","h_elRPhi",200,0,400);
    fOutput->Add(h_elRPhi);

    h_elnBLHits             =  new TProfile("h_elnBLHits","h_elnBLHits",200,0,400);
    fOutput->Add(h_elnBLHits);

    h_elnPixHits            =  new TProfile("h_elnPixHits","h_elnPixHits",200,0,400);
    fOutput->Add(h_elnPixHits);

    h_elnSCTHits            =  new TProfile("h_elnSCTHits","h_elnSCTHits",200,0,400);
    fOutput->Add(h_elnSCTHits);

    h_elnSiHits             =  new TProfile("h_elnSiHits","h_elnSiHits",200,0,400);
    fOutput->Add(h_elnSiHits);

    h_elPixeldEdx           =  new TH2F("h_elPixeldEdx","h_elPixeldEdx;qp[GeV];dE/dx(MeVg^{-1}cm^2",600,-200,200,100,-1,10);
    fOutput->Add(h_elPixeldEdx);

    h_el_E233               =  new TProfile("h_el_E233","h_el_E233",200,0,400);
    fOutput->Add(h_el_E233);

    h_el_E237               =  new TProfile("h_el_E237","h_el_E237",200,0,400);
    fOutput->Add(h_el_E237);

    h_el_E277               =  new TProfile("h_el_E277","h_el_E277",200,0,400);
    fOutput->Add(h_el_E277);

    h_el_weta2              =  new TProfile("h_el_weta2","h_el_weta2",200,0,400);
    fOutput->Add(h_el_weta2);

    h_el_trackd0            =  new TProfile("h_el_trackd0","h_el_trackd0",200,0,400);
    fOutput->Add(h_el_trackd0);

    h_el_trackd0beam        =  new TProfile("h_el_trackd0beam","h_el_trackd0beam",200,0,400);
    fOutput->Add(h_el_trackd0beam);

    h_el_trackd0pv          =  new TProfile("h_el_trackd0pv","h_el_trackd0pv",200,0,400);
    fOutput->Add(h_el_trackd0pv);

    h_el_trackd0pvunbiased  =  new TProfile("h_el_trackd0pvunbiased","h_el_trackd0pvunbiased",200,0,400);
    fOutput->Add(h_el_trackd0pvunbiased);

    h_el_trackd0_physics    =  new TProfile("h_el_trackd0_physics","h_el_trackd0_physics",200,0,400);
    fOutput->Add(h_el_trackd0_physics);

    h_el_weight             =  new TProfile("h_el_weight","h_el_weight",200,0,400);
    fOutput->Add(h_el_weight);

    h_el_bgweight           =  new TProfile("h_el_bgweight","h_el_bgweight",200,0,400);  
    fOutput->Add(h_el_bgweight);

    h_el_softeweight        =  new TProfile("h_el_softeweight","h_el_softeweight",200,0,400);
    fOutput->Add(h_el_softeweight);

    h_el_softebgweight      =  new TProfile("h_el_softebgweight","h_el_softebgweight",200,0,400);
    fOutput->Add(h_el_softebgweight);

    h_el_likelihoodsPixeldEdx = new TProfile("h_el_likelihoodsPixeldEdx","h_el_likelihoodsPixeldEdx",200,0,400);
    fOutput->Add(h_el_likelihoodsPixeldEdx);


    //Vertex
    h_vxp_nTracks           = new TH1F("vxp_nTracks","vxp_nTracks",100,-0.5,99.5);
    fOutput->Add(h_vxp_nTracks);

    h_vxp_Tracks_n          = new TH1F("vxp_Tracks_n","vxp_Tracks_n",100,-0.5,99.5);
    fOutput->Add(h_vxp_Tracks_n);

    h_elPtVsClpt            = new TH2F("elptVsClPt","elptVsClPt",500,0,500,500,0,500);
    fOutput->Add(h_elPtVsClpt);

    //Truth Reco Energy ratio
    h_elE_trthRecoRatioVsPt= new TProfile("elETrthRecoRatioVsPt","elETrthRecoRatioVsPt; pt [GeV];trthE/RecoE",1000,0,500);
    fOutput->Add(h_elE_trthRecoRatioVsPt);

    h_elE_trthRecoRatioVsEta= new TProfile("elETrthRecoRatioVsEta","elETrthRecoRatioVsEta; #eta;trthE/RecoE ",200,-5,5);
    fOutput->Add(h_elE_trthRecoRatioVsEta);

    h_elE_trthRecoRatioVsPhi= new TProfile("elETrthRecoRatioVsPhi","elETrthRecoRatioVsPhi; #varphi;trthE/RecoE",200,-5,5);
    fOutput->Add(h_elE_trthRecoRatioVsPhi);
}

void ElectronID::LoadBranches(Long64_t entry )
{
    b_el_n->GetEntry(entry);
    b_el_cl_E->GetEntry(entry);
    b_el_pt->GetEntry(entry);
    b_el_cl_pt->GetEntry(entry);
    b_el_cl_eta->GetEntry(entry);
    b_el_cl_phi->GetEntry(entry);
    b_el_charge->GetEntry(entry);


    b_vxp_nTracks->GetEntry(entry);
    b_vxp_trk_n->GetEntry(entry);

    b_el_author->GetEntry(entry);
    b_el_mediumPP->GetEntry(entry);
    b_el_mediumPPIso->GetEntry(entry);

    b_el_nTRTHits->GetEntry(entry);
    b_el_nTRTHighTHits->GetEntry(entry);
    b_el_TRTHighTHitsRatio->GetEntry(entry);
    b_el_TRTHighTOutliersRatio->GetEntry(entry);

    b_el_reta->GetEntry(entry);
    b_el_rphi->GetEntry(entry);
    b_el_nBLHits->GetEntry(entry);
    b_el_nPixHits->GetEntry(entry);
    b_el_nSCTHits->GetEntry(entry);
    b_el_nTRTHits->GetEntry(entry);
    b_el_nTRTHighTHits->GetEntry(entry);
    b_el_nPixHoles->GetEntry(entry);
    b_el_nSCTHoles->GetEntry(entry);
    b_el_nTRTHoles->GetEntry(entry);
    b_el_nBLSharedHits->GetEntry(entry);
    b_el_nPixSharedHits->GetEntry(entry);
    b_el_nSCTSharedHits->GetEntry(entry);
    b_el_nBLayerOutliers->GetEntry(entry);
    b_el_nPixelOutliers->GetEntry(entry);
    b_el_nSCTOutliers->GetEntry(entry);
    b_el_nContribPixelLayers->GetEntry(entry);
    b_el_nGangedPixels->GetEntry(entry);
    b_el_nGangedFlaggedFakes->GetEntry(entry);
    b_el_nPixelDeadSensors->GetEntry(entry);
    b_el_nPixelSpoiltHits->GetEntry(entry);
    b_el_nSCTDoubleHoles->GetEntry(entry);
    b_el_nSCTDeadSensors->GetEntry(entry);
    b_el_nSCTSpoiltHits->GetEntry(entry);
    b_el_expectBLayerHit->GetEntry(entry);
    b_el_nSiHits->GetEntry(entry);
    b_el_pixeldEdx->GetEntry(entry);
    b_el_nGoodHitsPixeldEdx->GetEntry(entry);
    b_el_massPixeldEdx->GetEntry(entry);
    b_el_E233->GetEntry(entry);
    b_el_E237->GetEntry(entry);
    b_el_E277->GetEntry(entry);
    b_el_weta2->GetEntry(entry);
    b_el_trackd0->GetEntry(entry);
    b_el_trackd0beam->GetEntry(entry);
    b_el_trackd0pv->GetEntry(entry);
    b_el_trackd0pvunbiased->GetEntry(entry);
    b_el_trackd0_physics->GetEntry(entry);

    b_el_electronweight->GetEntry(entry);
    b_el_electronbgweight->GetEntry(entry);
    b_el_softeweight->GetEntry(entry);
    b_el_softebgweight->GetEntry(entry);
    b_el_likelihoodsPixeldEdx->GetEntry(entry);

    b_el_truth_pt->GetEntry(entry);
    b_el_truth_E->GetEntry(entry);
    b_el_truth_matched->GetEntry();
}

std::vector<float> ElectronID::getMaxPtList(std::vector<float>& vec)
{
    /*std::vector<float> myVector = vec;
    std::vector<float> returnVector;
    while(myVector.size() != 0)
    {
        float max = 0;
        for(unsigned int i =0; i < myVector.size(); i++)
        {
            if (max < myVector.at(i))
                max = myVector.at(i);
        }
        returnVector.push_back(max);
        myVector.erase(find(myVector.begin(),myVector.end(),max));
    }

    reverse(returnVector.begin(), returnVector.end());*/

    sort(vec.begin(), vec.end(),sortDescend);
    return vec;
}

bool ElectronID::IsZEvent()
{
}

void ElectronID::DoTruthAna()
{
    for(unsigned int i =0; i < el_truth_E->size(); i++)
    {
        float trthE     = el_truth_E->at(i);
        float recoE     = el_cl_E->at(i);
        float recoPt    = el_cl_pt->at(i);
        float recoEta   = el_cl_eta->at(i);
        float recoPhi   = el_cl_phi->at(i);

        if(trthE != 0) 
        {
            h_elE_trthRecoRatioVsPt->Fill(recoPt/1000,trthE/recoE);
            h_elE_trthRecoRatioVsEta->Fill(recoEta,trthE/recoE);
            h_elE_trthRecoRatioVsPhi->Fill(recoPhi,trthE/recoE);
        }
    }
}

void ElectronID::DoRecoAna()
{
   float leadPt         = -100; 
   float leadEta        = -100;
   float leadPhi        = -100;
   float leadE          = -100;
   int   leadingAuthor  = -100;

   float subleadPt      = -100;   
   float subleadEta     = -100; 
   float subleadPhi     = -100;
   float subleadE       = -100;
   int subleadingAuthor = -100;

   std::vector<float> ePlusPtVec;
   std::vector<float> eMinusPtVec;


   for(unsigned int i =0; i < el_cl_pt->size(); ++i)
   {
       float elCharge   = el_charge->at(i);
       h_elCharge->Fill(elCharge);
       if(elCharge ==1)
       {
           ePlusPtVec.push_back(elCharge);
       }
       else if(elCharge ==-1)
       {
           eMinusPtVec.push_back(elCharge);
       }
       h_nePlus->Fill(ePlusPtVec.size());
       h_neMinus->Fill(eMinusPtVec.size());
   }

   if(ePlusPtVec.size() >=1 && eMinusPtVec.size() >= 1)
   {
       std::vector<float> myelClusterPt = *el_cl_pt;
       std::vector<float> ptSortedVec   = this->getMaxPtList(myelClusterPt);
       std::vector<float> ePlusSortedVec= this->getMaxPtList(ePlusPtVec);
       std::vector<float>eMinusSortedVec= this->getMaxPtList(eMinusPtVec);
 
       leadPt           = ptSortedVec.at(0);
       subleadPt        = ptSortedVec.at(1);

       for (unsigned int i =0 ; i < el_cl_pt->size(); i++)
       {
           float myelPt     = el_cl_pt->at(i);
           float el_eta     = el_cl_eta->at(i);
           float el_phi     = el_cl_phi->at(i);
           int elAuthor     = el_author->at(i);

           if((fabs(el_eta)< 2.5) &&(!( fabs(el_eta)<1.52 && fabs(el_eta) > 1.37 )))
           {
               if(myelPt == leadPt && (elAuthor ==1 || elAuthor ==3))
               {
                   leadEta      = el_cl_eta->at(i);
                   leadPhi      = el_cl_phi->at(i);
                   leadE        = el_cl_E->at(i);
                   leadingAuthor= el_author->at(i);
               }
               else if (myelPt== subleadPt && (elAuthor ==1 || elAuthor == 3))
               {
                   subleadEta       = el_cl_eta->at(i);
                   subleadPhi       = el_cl_phi->at(i);
                   subleadE         = el_cl_E->at(i);
                   subleadingAuthor = el_author->at(i);
               }
           }
       }

       bool foundZ = false;
       if(leadPt != -100 && subleadPt !=-100)
       {
           TLorentzVector zEl1, zEl2;
           zEl1.SetPtEtaPhiE(leadPt, leadEta, leadPhi,leadE);
           zEl2.SetPtEtaPhiE(subleadPt,subleadEta, subleadPhi,subleadE);
        
           float invMass = (zEl1 + zEl2).M()/1000;
           if(invMass>66 && invMass < 116)
           {
               foundZ = true;
               h_zInvMass->Fill(invMass);
               
               h_zElPt->Fill(leadPt/1000);
               h_zElPt->Fill(subleadPt/1000);

               h_zElEta->Fill(leadEta);
               h_zElEta->Fill(subleadEta);

               h_zElPhi->Fill(leadPhi);
               h_zElPhi->Fill(subleadPhi);
           }
       }


       //if a Z Event, Fill Other Electron histograms
       if(foundZ)
       {
           for (Int_t i = 0 ; i < el_cl_pt->size(); i++)
           {
               float myelPt             = el_cl_pt->at(i);
               float elClPt             = el_cl_pt->at(i);
               float elClEta            = el_cl_eta->at(i);
               float elClPhi            = el_cl_phi->at(i);
               float elCharge           = el_charge->at(i);
               
               int trtHits              = el_nTRTHits->at(i);
               int trtHTHits            = el_nTRTHighTHits->at(i);
               float trtHTHitRatio      = el_TRTHighTHitsRatio->at(i);
               float trtHtOutlierRatio  = el_TRTHighTOutliersRatio->at(i);

               float reta               = el_reta->at(i);
               float rphi               = el_rphi->at(i);
               float nBLhits            = el_nBLHits->at(i);
               float nPixHits           = el_nPixHits->at(i);
               float nSCTHits           = el_nSCTHits->at(i);
               float nSiHits            = el_nSiHits->at(i);
               float pixeldEdx          = el_pixeldEdx->at(i);
               float e233               = el_E233->at(i);
               float e237               = el_E237->at(i);
               float e277               = el_E277->at(i);
               float weta2              = el_weta2->at(i);
               float trackd0            = el_trackd0->at(i);
               float trackd0beam        = el_trackd0beam->at(i);
               float trackd0pv          = el_trackd0pv->at(i);
               float trackd0pvunbiased  = el_trackd0pvunbiased->at(i);
               float trackd0_physics    = el_trackd0_physics->at(i);
               float elWeight           = el_electronweight->at(i);
               float elbgWeight         = el_electronbgweight->at(i);
               float elsofteWeight      = el_softeweight->at(i);
               float elsoftebgWeight    = el_softebgweight->at(i);

               std::vector<float> ellikelihooddEdx   = el_likelihoodsPixeldEdx->at(i);
               
               int  my_vxp_nTracks      = vxp_nTracks->at(0);
               int  my_vxp_Tracks_n     = vxp_trk_n->at(0);

               if((fabs(elClEta)< 2.5) &&(!( fabs(elClEta)<1.52 && fabs(elClEta) > 1.37 )) )
               {
                   //Remove the Z Electrons
                   if(myelPt!= leadPt && myelPt!= subleadPt)
                   {
                       h_elPt->Fill(myelPt/1000);
                       h_elEta->Fill(elClEta);
                       h_elPhi->Fill(elClPhi);

                       h_trtHits->Fill(myelPt/1000,trtHits);
                       h_trtHtHits->Fill(myelPt/1000,trtHTHits);
                       h_trtHtHitRatio->Fill(myelPt/1000,trtHTHitRatio);
                       h_trtHtOutlierRatio->Fill(myelPt/1000,trtHtOutlierRatio);
                       h_elReta->Fill(myelPt/1000,reta);
                       h_elRPhi->Fill(myelPt/1000,rphi);
                       h_elnBLHits->Fill(myelPt/1000,nBLhits);
                       h_elnPixHits->Fill(myelPt/1000,nPixHits);
                       h_elnSCTHits->Fill(myelPt/1000,nSCTHits);
                       h_elnSiHits->Fill(myelPt/1000,nSiHits);
                       h_elPixeldEdx->Fill(elCharge*myelPt/1000,pixeldEdx);
                       h_el_E233->Fill(myelPt/1000,e233);
                       h_el_E237->Fill(myelPt/1000,e237);
                       h_el_E277->Fill(myelPt/1000,e277);
                       h_el_weta2->Fill(myelPt/1000,weta2);
                       h_el_trackd0->Fill(myelPt/1000,trackd0);
                       h_el_trackd0beam->Fill(myelPt/1000,trackd0beam);
                       h_el_trackd0pv->Fill(myelPt/1000,trackd0pv);
                       h_el_trackd0pvunbiased->Fill(myelPt/1000,trackd0pvunbiased);
                       h_el_trackd0_physics->Fill(myelPt/1000,trackd0_physics);

                       h_el_weight->Fill(myelPt/1000,elWeight);
                       h_el_bgweight->Fill(myelPt/1000,elbgWeight);
                       h_el_softeweight->Fill(myelPt/1000, elsoftebgWeight);
                       h_el_softebgweight->Fill(myelPt/1000,elsoftebgWeight);
                       h_el_likelihoodsPixeldEdx->Fill(myelPt/1000,ellikelihooddEdx.size());

                       h_elPtVsClpt->Fill(myelPt/100,myelPt/1000,elClPt/1000);

                       h_vxp_nTracks->Fill(my_vxp_nTracks);
                       h_vxp_Tracks_n->Fill(my_vxp_nTracks);
                   }
               }
           }
       }
   }
}

