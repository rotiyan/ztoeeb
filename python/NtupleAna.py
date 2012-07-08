import abc
from NtupleAnaBase import NtupleAnaBase
from array import array

import ROOT, sys,os,glob 
from multiprocessing import Process,Queue,current_process


class NtupleAna(NtupleAnaBase):
    def __init__(self,tree,outFileName, scaleFactor = 1, hists = {}):
        self.hists  = hists
        self.myTree   = tree

        '''Histogram Scale Factor (Lumi Scale Factor)'''
        self.scaleFactor    = scaleFactor

        self.outFileName    = outFileName
    
    def initialize(self):
        #GRL Shared object
        #ROOT.gSystem.Load("libGoodRunsListsLib.so")
        #ROOT.DQ.SetXMLFile("../share/data12_7TeV.periodAllYear_DetStatus-v36-pro10_CoolRunQuery-00-04-08_WZjets_allchannels.xml")

        #Used in DoAuthorEl()
        self.regPtHist("ElAuthorPt")
        self.regEtaHist("ElAuthorEta")
        self.regPhiHist("ElAuthorPhi")
        self.regTH1("ElAuthorMultplcty","",20,-0.5,19.5)

        self.regTHnSparse("ZCandKine","",nbins=7,\
                bins=[1000,100,500, 1000, 100, 500,400],\
                fmin=[0  , -5, -5, 0   , -5 , -5 ,0 ],\
                fmax=[500, 5 ,  5, 500 , 5  , 5  ,200])

        '''Truth'''
        self.regTH1("Bhadrons")
        self.regTH1("BHdrnpt","",500,0,500)
        self.regTH1("BHdrneta","",100,-5,5)
        self.regTH2("BHdrnBElPtCorr","",500,0,500,100,0,100)
        self.regTH2("BHdrnEvents","",35,0.5,35.5,10,-0.5,9.5)
        self.regTH2("BElEvents","",35,0.5,35.5,10,-0.5,9.5)
        self.regProfHist2D("BElEvntEff","",35,0.5,35.5,10,-0.5,9.5)

        self.regTH1("Chadrons")
        self.regTH2("CHdrnCElPtCorr","",500,0,500,100,0,100)
        self.regPtHist("Cpt")
        self.regEtaHist("Ceta")
        self.regPtHist("CscdPt")
        self.regEtaHist("CscdEta")
        self.regTH2("CHdrnEvents","",35,0.5,35.5,10,-0.5,9.5)
        self.regTH2("CElEvents","",35,0.5,35.5,10,-0.5,9.5)

        self.regTH2("hardVsSoftEl","",10,0.5,10.5,10,0.5,10.5)

        '''SemiElectron decay '''
        #ptcut,nBEl,nCEl
        self.regTHnSparse("BSemiElectron","",nbins=3,\
                bins=[35  ,11  ,11],\
                fmin=[0.5 ,-0.5,-0.5  ],\
                fmax=[35.5,10.5,10.5 ])

        self.regTHnSparse("CSemiElectron","",nbins=3,\
                bins=[35  ,11  ,10],\
                fmin=[0.5 ,-0.5,0.5  ],\
                fmax=[35.5,10.5,10.5 ])


        '''ZB electron'''
        self.regTHnSparse("ZBEl","",nbins=5,\
                bins=[200,200,100,100,10],\
                fmin=[0  ,0  ,-5 , -5,0.5 ],\
                fmax=[200,200,5  ,  5,10.5])

        '''bquark correlation'''
        self.regPtHist("bQuarkPt")

        self.regTH3("bquarkPtDeltaR",";b1;b2",200,0,200,200,0,200,100,0,2)
        self.regTHnSparse("bquarkKinematics",nbins=7,\
                bins=[200,200,100,100,100,100,100],\
                fmin=[0  ,0  ,0  ,-5 ,-5 ,-5 ,-4 ],\
                fmax=[100,100,100,5  , 5 ,5  , 4 ])

        self.regTH1("nbQuark","",10,0.5,10.5)
        
        '''Z boson decay electrons'''
        self.regPtHist("ZElPt")
        self.regEtaHist("ZElEta")
        self.regPhiHist("ZElPhi")


    def execute(self):
        #Check if the event passes GRL
        #if(ROOT.DQ.PassRunLB(self.getCurrentValue("RunNumber"),self.getCurrentValue("LumiblockNumber"))):
        #Analyse the author electrons
        self.DoAuthorEl()
        
    def finalize(self):
        #Save histograms to disk
        histlist = self.gethistList()
        fname = self.outFileName+current_process().name
        f = ROOT.TFile(fname,"RECREATE")
        for h in histlist:
            h.Scale(self.scaleFactor)
            h.Write()
        f.Close()

    ''''''''''''''''''''''''''''''''''''''''''''
    '''Functions to be called inside execute'''
    ''''''''''''''''''''''''''''''''''''''''''''

    def doZElectron(self):
        ZElPtVec    = self.getCurrentValue("ZElPt")
        ZElEtaVec   = self.getCurrentValue("ZElEta")
        ZElPhiVec   = self.getCurrentValue("ZElPhi")

        for i in xrange(ZElEtaVec.size()):
            eta = ZElEtaVec.at(i)
            if(abs(eta) < 2.5):
                self.gethist("ZElPt").Fill(ZElPtVec.at(i))
                self.gethist("ZElEta").Fill(ZElEtaVec.at(i))
                self.gethist("ZElPhi").Fill(ZElPhiVec.at(i))

    def hadronElectronMatching(self):
        BPtVec      = self.getCurrentValue("BPt")
        BPhiVec     = self.getCurrentValue("BPhi")

        BSemiElPtVec= self.getCurrentValue("BsemiElPt")
        BSemiElEtaVec=self.getCurrentValue("BsemiElEta")
        BSemiElPhiVec=self.getCurrentValue("BsemiElPhi")
        BisSemiVec  = self.getCurrentValue("BisSemiElectron")

        CSemiElPtVec= self.getCurrentValue("CsemiElPt")
        CSemiElEtaVec=self.getCurrentValue("CsemiElEta")
        CSemiElPhiVec=self.getCurrentValue("CsemiElPhi")
        CisSemiVec  = self.getCurrentValue("CisSemiElectron")

        ptcutlist = [2,5,10,15,20,25,30]

        ZElPt       = self.getCurrentValue("ZElPt")
        ZElEta      = self.getCurrentValue("ZElEta")

        for ptcut in ptcutlist:
            nBHadrons   = self.getBMultplcty(ptcut,2.5)
            nBEl        = self.getBElMultplcty(ptcut,2.5)

            x = [ptcut,nBEl,nBHadrons]
            self.gethist("BSemiElectron").Fill(array("d",x))

            self.gethist("BHdrnEvents").Fill(ptcut,nBHadrons)
            isSemiB  = 0
            if(nBEl):
                isSemiB = 1
                self.gethist("BElEvents").Fill(ptcut,nBHadrons)

            self.gethist("BElEvntEff").Fill(ptcut,nBHadrons,isSemiB)

            nCHadrons   = self.getCMultplcty(ptcut,2.5)
            nCEl        = self.getCElMultplcty(ptcut,2.5)

            x = [ptcut,nCEl,nCHadrons]
            self.gethist("CSemiElectron").Fill(array("d",x))

            if(nCEl):
                self.gethist("CElEvents").Fill(ptcut,nCHadrons)


    def makeDeltaRPlots(self):

        bQuarkME_pt     = self.getCurrentValue("bQuarkME_pt")
        bQuarkME_eta    = self.getCurrentValue("bQuarkME_eta")
        bQuarkME_phi    = self.getCurrentValue("bQuarkME_phi")

        BPtVec      = self.getCurrentValue("BPt")
        BEtaVec     = self.getCurrentValue("BEta")
        BPhiVec     = self.getCurrentValue("BPhi")

        CPtVec      = self.getCurrentValue("CPt")
        CEtaVec     = self.getCurrentValue("CEta")
        CPhiVec     = self.getCurrentValue("CPhi")

        BBC         = self.getCurrentValue("BBC")
        CparentBC   = self.getCurrentValue("CParentBC")

        '''bquark correlation'''
        if(len(bQuarkME_pt) ==2):
            x = [bQuarkME_pt[0],bQuarkME_pt[1],bQuarkME_eta[0],bQuarkME_eta[1],bQuarkME_phi[0],bQuarkME_phi[1]]
            self.gethist("bquarkKinematics").Fill(array("d",x))
            self.gethist("nbQuark").Fill(len(bQuarkME_pt))
            
            bPtEtaPhi = zip(bQuarkME_pt,bQuarkME_eta,bQuarkME_phi)
            self.gethist("bquarkPtDeltaR").Fill(bQuarkME_pt[0],bQuarkME_pt[1],self.deltaR(bQuarkME_eta[0],bQuarkME_phi[0],bQuarkME_eta[1],bQuarkME_phi[1]))

    def getBMultplcty(self,ptcut,etacut):
        BPtVec      = self.getCurrentValue("BPt")
        BEtaVec     = self.getCurrentValue("BEta")

        nBHadrons = 0
        for i in xrange(BPtVec.size()):
            BHdrnPt = BPtVec.at(i)
            BHdrnEta= BEtaVec.at(i)

            if(BHdrnPt >ptcut and abs(BHdrnEta) < etacut):
                nBHadrons +=1

        return nBHadrons

    def getBElMultplcty(self,ptcut,etacut):
        BPtVec      = self.getCurrentValue("BPt")
        BEtaVec     = self.getCurrentValue("BEta")
        BElPtVec    = self.getCurrentValue("BsemiElPt")
        BElEtaVec   = self.getCurrentValue("BsemiElEta")

        isSemiElVec = self.getCurrentValue("BisSemiElectron")

        nBEl    = 0
        for i in xrange(BPtVec.size()):
            BHdrnPt = BPtVec.at(i)
            BHdrnEta= BEtaVec.at(i)
            SemiEl  = isSemiElVec.at(i)

            if(BHdrnPt > ptcut and abs(BHdrnEta) < etacut):
                if(SemiEl ==1):

                    elPass = False
                    for k in xrange(BElPtVec.size()):
                        BElPt   = BElPtVec.at(k)
                        BElEta  = BElEtaVec.at(k)
                        '''10 GeV Soft El cut'''
                        if(BElPt> 10 and abs(BElEta) < etacut):
                            elPass = True

                    if(elPass):
                        nBEl +=1

        return nBEl

    def fillBKinematics(self,ptcut,etacut):
        BPtVec      = self.getCurrentValue("BPt")
        BEtaVec     = self.getCurrentValue("BEta")
        BPhiVec     = self.getCurrentValue("BPhi")
        BsemiElPtVec= self.getCurrentValue("BsemiElPt")

        isSemiElVec = self.getCurrentValue("BisSemiElectron")
        
        nBHadrons = 0
        for i in xrange(BPtVec.size()):
            BHdrnPt = BPtVec.at(i)
            BHdrnEta= BEtaVec.at(i)

            if(BHdrnPt >ptcut and abs(BHdrnEta) < etacut):
                self.gethist("Bhadrons").Fill("Bhadrons",1)
                self.gethist("BHdrnpt").Fill(BHdrnPt)
                self.gethist("BHdrneta").Fill(BHdrnEta)
                nBHadrons +=1
                
                if(isSemiElVec.at(i)==1):
                    self.gethist("Bhadrons").Fill("Bsemi",1)
                    
                    for j in xrange(BsemiElPtVec.size()):
                        self.gethist("BHdrnBElPtCorr").Fill(BHdrnPt,BsemiElPtVec.at(j))

    def fillCKinematics(self,ptcut,etacut):
        CPtVec      = self.getCurrentValue("CPt")
        CEtaVec     = self.getCurrentValue("CEta")
        CSemiElPtVec= self.getCurrentValue("CsemiElPt")
        isSemiElVec = self.getCurrentValue("CisSemiElectron")
        CsemiElPtVec= self.getCurrentValue("CsemiElPt")
        BBCVec      = self.getCurrentValue("BBC") #BBarcode Vector
        CParentBCVec= self.getCurrentValue("CParentBC") #CParent Barcode
        
        nCHadrons = 0
        for i in xrange(CPtVec.size()):
            CHdrnPt = CPtVec.at(i)
            CHdrnEta= CEtaVec.at(i)

            if(CHdrnPt >ptcut and abs(CHdrnEta) < etacut):
                "fix the ntuple branch"
                #if (CParentBCVec.at(i) in BBCVec):
                #    self.gethist("CscdPt").Fill(CHdrnPt)
                #    self.gethist("CscdEta").Fill(CHdrnEta)

                self.gethist("Chadrons").Fill("Chadrons",1)
                self.gethist("Cpt").Fill(CHdrnPt)
                self.gethist("Ceta").Fill(CHdrnEta)
                nCHadrons +=1
                if(isSemiElVec.at(i)==1):
                    self.gethist("Chadrons").Fill("Csemi",1)

                    #if(CParentBCVec.at(i) not in BBCVec):
                    for j in xrange(CsemiElPtVec.size()):
                        self.gethist("CHdrnCElPtCorr").Fill(CHdrnPt,CSemiElPtVec.at(j))




    def getCMultplcty(self,ptcut,etacut):
        CPtVec      = self.getCurrentValue("CPt")
        CEtaVec     = self.getCurrentValue("CEta")
        isSemiElVec = self.getCurrentValue("CisSemiElectron")
        
        nCHadrons = 0
        for i in xrange(CPtVec.size()):
            CHdrnPt = CPtVec.at(i)
            CHdrnEta= CEtaVec.at(i)

            if(CHdrnPt >ptcut and abs(CHdrnEta) < etacut):
                nCHadrons +=1

        return nCHadrons

    '''# Soft Electrons in C-hadron PtCut,etacut bins'''
    def getCElMultplcty(self,ptcut,etacut):
        CPtVec      = self.getCurrentValue("CPt")
        CEtaVec     = self.getCurrentValue("CEta")
        CElPtVec    = self.getCurrentValue("CsemiElPt")
        CElEtaVec   = self.getCurrentValue("CsemiElEta")

        isSemiElVec = self.getCurrentValue("CisSemiElectron")

        nCEl    = 0
        for i in xrange(CPtVec.size()):
            CHdrnPt = CPtVec.at(i)
            CHdrnEta= CEtaVec.at(i)
            SemiEl  = isSemiElVec.at(i)

            if(CHdrnPt > ptcut and abs(CHdrnEta) < etacut):
                if(SemiEl ==1):

                    elPass = False
                    for k in xrange(CElPtVec.size()):
                        CElPt   = CElPtVec.at(k)
                        CElEta  = CElEtaVec.at(k)
                        '''Soft Electron cut 10 GeV'''
                        if(CElPt> 10 and abs(CElEta) < etacut):
                            elPass = True

                    if(elPass):
                        nCEl +=1

        return nCEl

    '''# Soft Electrons in C SemiElectron Ptcut, etacut'''
    def getNCEl(self,elptcut,eletacut):
        CElEtaVec   = self.getCurrentValue("CsemiElEta")
        CElPtVec    = self.getCurrentValue("CsemiElPt")
        isSemiElVec = self.getCurrentValue("CisSemiElectron")

        nCEl = 0
        for i in xrange(CElPtVec.size()):
            CElPt   = CElPtVec.at(i)
            CElEta  = CElEtaVec.at(i)
            if(CElPt > elptcut  and abs(CElEta) < etacut):
                nCEl +=1

        return nCEl


    '''The Author Electron is an electron with track pointing to the cluster identified'''
    def DoAuthorEl(self):
        clptVec     = self.getCurrentValue("el_cl_Pt")
        cletaVec    = self.getCurrentValue("el_cl_Eta")
        clphiVec    = self.getCurrentValue("el_cl_Phi")

        softeVec    = self.getCurrentValue("elAuthorSofte")
        authorVec   = self.getCurrentValue("elAuthor")

        chrgVec     = self.getCurrentValue("el_charge")
        medIdVec    = self.getCurrentValue("el_medium")
        medPPIdVec  = self.getCurrentValue("el_mediumPP")
        tightPPIdVec= self.getCurrentValue("el_tightPP")


        kineList = []

        for i in xrange(cletaVec.size()):
            clEta   = cletaVec.at(i)
            clPt    = clptVec.at(i)
            clPhi   = clphiVec.at(i)

            author  = authorVec.at(i)
            softe   = softeVec.at(i)
            clChrg    = chrgVec.at(i)
            medId   = medIdVec.at(i)
            medPPId = medPPIdVec.at(i)
            tightPPid=tightPPIdVec.at(i)

            if(medPPId==True and (author == True or(author==True and softe==True))):
 
                kineList +=[(clPt,clEta,clPhi,clChrg)]

                self.gethist("ElAuthorEta").Fill(clEta)
                self.gethist("ElAuthorPt").Fill(clPt)
                self.gethist("ElAuthorPhi").Fill(clPhi)

        self.gethist("ElAuthorMultplcty").Fill(len(kineList)) 
        
        sortList = sorted(kineList)
        sortList.reverse()
        
        pt  = []
        eta = []
        phi = []
        chrg= []
        if(len(sortList)>1):
            el1 = sortList[0]
            el2 = sortList[1]

            pt = [el1[0],el2[0]]
            eta= [el1[1],el2[1]]
            phi= [el1[2],el2[2]]
            chrg=[el1[3],el2[3]]

            self.FillZCandKinematics(pt,eta,phi,chrg)


    '''Get Invariant mass list of OS electrons'''
    '''DEBUG:The el mass should be removed'''
    def FillZCandKinematics(self,pt,eta,phi,chrg):
        negIdx = [chrg.index(x) for x in chrg if x <0]
        plusIdx= [chrg.index(x) for x in chrg if x >0]

        ptPair = [(pt[ePidx],pt[eMidx]) for eMidx in negIdx for ePidx in plusIdx]
        etaPair = [(eta[ePidx],eta[eMidx]) for eMidx in negIdx for ePidx in plusIdx]
        phiPair = [(phi[ePidx],phi[eMidx]) for eMidx in negIdx for ePidx in plusIdx]

        invmass = []
        for i in xrange(len(ptPair)):
            ePlus = ROOT.TLorentzVector()
            eMinus= ROOT.TLorentzVector()

            ePlus.SetPtEtaPhiM(float(ptPair[i][0]),float(etaPair[i][0]),float(phiPair[i][0]),0.000511)
            eMinus.SetPtEtaPhiM(float(ptPair[i][1]),float(etaPair[i][1]),float(phiPair[i][1]),0.000511)

            m = (ePlus + eMinus).M()

            if(m > 20):
                zCandKine= [ptPair[i][0],etaPair[i][0], phiPair[i][0], ptPair[i][1],etaPair[i][1], phiPair[i][0], m]
                self.gethist("ZCandKine").Fill(array("d",zCandKine))


    ''''''''''''''''''''''''''''''''
    '''Function to add histograms to the analysis chain'''
    ''''''''''''''''''''''''''''''''
    def regPtHist(self,name):
        self.regTH1(name,";[GeV]",500,0,500)


    def regEtaHist(self,name):
        self.regTH1(name,";#eta",100,-5,5)

    def regPhiHist(self,name):
        self.regTH1(name,"",100,-4,4)


    ''''''''''''''''''''''''
    ''''Utility functions'''
    ''''''''''''''''''''''''

    '''Checks if the supplied pdg id is that of  a B hadron'''
    def isBHadron(self,pdg):
        mpdg = abs(pdg)
        if( ( 500 < mpdg and mpdg < 599 )    or \
                ( 10500 < mpdg and mpdg < 10599 )  or \
                (  5000 < mpdg and mpdg < 5999  )  or \
                ( 20500 < mpdg and mpdg < 20599 ) ):
                return True
        else:
            return False

    '''checks if the supplied pdg id is that of a C hadron'''
    def isCHadron(self,pdg):
        mpdg = abs(pdg)
        if( ( 400  < mpdg and mpdg < 499 )   or \
                ( 10400 < mpdg and mpdg < 10499 ) or \
                (  4000 < mpdg and mpdg < 4999  ) or \
                ( 20400 < mpdg and mpdg < 20499 ) ):
            return True
        else:
            return False

    def deltaR(self,eta1,phi1,eta2,phi2):
        import math
        return math.sqrt( (eta1 - eta2)**2 + (phi1 - phi2)**2 )

    '''Compare delta R separation between ME quark 
    and the heavy flavor hadrons'''
    def compareBbDeltaR(self,ptcut,etacut):

        h3_BbdrMin  = self.gethist("BbdeltaRMin")
        h3_BbdrMax  = self.gethist("BbdeltaRMax")

        h3_BbdrMin.GetXaxis().SetRangeUser(ptcut,ptcut +1)
        h3_BbdrMax.GetXaxis().SetRangeUser(ptcut,ptcut +1)
        
        h3_BbdrMin.GetYaxis().SetRangeUser(0,etacut)
        h3_BbdrMax.GetYaxis().SetRangeUser(0,etacut)

        h_rmin  = h3_BbdrMin.Project3D("z")
        h_rmax  = h3_BbdrMax.Project3D("z")

        h_rmin.GetXaxis().SetRangeUser(0,1)
        h_rmin.GetXaxis().SetTitle("#Delta R")
        h_rmax.GetXaxis().SetRangeUser(0,1)

        h_rmin.SetName("BbdeltaRMin_"+str(ptcut)+"_"+str(int(etacut*10)))
        h_rmax.SetName("BbdeltaRMax_"+str(ptcut)+"_"+str(int(etacut*10)))
        
        self.addtohistList(h_rmin)
        self.addtohistList(h_rmax)

    def compareCbDeltaR(self,ptcut,etacut):
        h3_CbdrMin  = self.gethist("CbdeltaRMin")
        h3_CbdrMax  = self.gethist("CbdeltaRMax")

        h3_CbdrMin.GetXaxis().SetRangeUser(ptcut,ptcut +1)
        h3_CbdrMax.GetXaxis().SetRangeUser(ptcut,ptcut +1)
        
        h3_CbdrMin.GetYaxis().SetRangeUser(0,etacut)
        h3_CbdrMax.GetYaxis().SetRangeUser(0,etacut)

        h_rmin  = h3_CbdrMin.Project3D("z")
        h_rmax  = h3_CbdrMax.Project3D("z")

        h_rmin.GetXaxis().SetRangeUser(0,1)
        h_rmin.GetXaxis().SetTitle("#Delta R")
        h_rmax.GetXaxis().SetRangeUser(0,1)

        h_rmin.SetName("CbdeltaRMin_"+str(ptcut)+"_"+str(int(etacut*10)))
        h_rmax.SetName("CbdeltaRMax_"+str(ptcut)+"_"+str(int(etacut*10)))
        
        self.addtohistList(h_rmin)
        self.addtohistList(h_rmax)

        '''cascaded decay b->c'''
        h3_Cscd_drMin   = self.gethist("CascadedRMin")
        h3_Cscd_drMax   = self.gethist("CascadedRMax")

        h3_Cscd_drMin.GetXaxis().SetRangeUser(ptcut,ptcut +1)
        h3_Cscd_drMax.GetXaxis().SetRangeUser(ptcut,ptcut +1)

        h3_Cscd_drMin.GetYaxis().SetRangeUser(0,etacut)
        h3_Cscd_drMax.GetYaxis().SetRangeUser(0,etacut)

        h_Cscd_rmin     = h3_Cscd_drMin.Project3D("z")
        h_Cscd_rmax     = h3_Cscd_drMax.Project3D("z")

        h_Cscd_rmin.GetXaxis().SetRangeUser(0,1)
        h_Cscd_rmin.GetXaxis().SetTitle("#Delta R")
        h_Cscd_rmax.GetXaxis().SetRangeUser(0,1)
        h_Cscd_rmax.GetXaxis().SetTitle("#Delta R")

        h_Cscd_rmin.SetName("Cscd_drMin_"+str(ptcut)+"_"+str(int(etacut*10)))
        h_Cscd_rmax.SetName("Cscd_drMax_"+str(ptcut)+"_"+str(int(etacut*10)))

        self.addtohistList(h_Cscd_rmin)
        self.addtohistList(h_Cscd_rmax)

#end of NtupleAna 

'''''''''''''''''''''''''''''''''''''''''''''''
PlotScript:
    A Class to make post processed final histograms. 
    When the constructor of the class is called,
    all the public methods of the class are executed. 
    All utility methods in this class should
    be private methods. The public methods should be 
    written only to make final histograms. 
'''''''''''''''''''''''''''''''''''''''''''''''

#A dummy to invoke rootlogon.py
ROOT.gStyle.SetOptStat(0)
class plotscript:
    def __init__(self,histFile):
        self.inHistFile     = histFile 
        self.outHistList    = []

        '''Make output pdf directory'''
        self.pdfPath= "pdf"
        os.system("mkdir -p "+ self.pdfPath)

        '''Plot the histograms'''
        self.__run()


    def __run(self):
        import inspect
        '''get a list of methods of this class'''
        funcList = [ x[0] for x in inspect.getmembers(self,inspect.ismethod) if '__' not in x[0] ]
        '''execute all those functions in the funclist'''
        for funcName in funcList:
            method = getattr(self,funcName)
            method()

        #Save histograms
        f = ROOT.TFile("procHist.root","RECREATE")
        #print self.outHistList
        for hist in self.outHistList:
            hist.Write()

        f.Close()

    def __getInHist(self,name):
        try:
            return self.inHistFile.Get(name)
        except:
            print "could not find hist: ",name

    def __getOutHist(self,name):
        try: 
            return self.outHistList[name]
        except:
            print "could not find hist: ",name

    def __saveHist(self,hist):
        self.outHistList += [hist]


    ''' Create functions to make the final histograms here.
    Functions with '__' will not be executed while creating
    final histograms'''


    def MakeRecoHists(self):
        self.__saveHist(self.__getInHist("ElAuthorPt"))
        self.__saveHist(self.__getInHist("ElAuthorEta"))
        self.__saveHist(self.__getInHist("ElAuthorPhi"))

        h_nElAuthor = self.__getInHist("ElAuthorMultplcty")
        h_nElAuthor.GetXaxis().SetTitle("#")
        self.__saveHist(h_nElAuthor)

        ###################################################
        #ZBoson kinematics from THnSparse.
        h_ZCandSparse    = self.__getInHist("ZCandKine")

        if(h_ZCandSparse):
            h_ZBosonEplusPt  = h_ZCandSparse.Projection(0)
            h_ZBosonEplusPt.SetName("Boson Candidate Eplus pt")
            h_ZBosonEplusPt.GetXaxis().SetTitle("[GeV]")
            self.__saveHist(h_ZBosonEplusPt)

            h_ZBosonEMinusPt = h_ZCandSparse.Projection(3)
            h_ZBosonEMinusPt.SetName("Boson Candidate EMinus pt")
            h_ZBosonEMinusPt.GetXaxis().SetTitle("[GeV]")
            self.__saveHist(h_ZBosonEMinusPt)

            h_ZBosonMass    = h_ZCandSparse.Projection(6)
            h_ZBosonMass.SetName("Z Boson Mass")
            h_ZBosonMass.GetXaxis().SetTitle("[GeV]")
            self.__saveHist(h_ZBosonMass)

    
    def MakeHistBSemiElectron(self):
        #Retrieve the THnSparse
        BSparse         = self.__getInHist("BSemiElectron")

        if(BSparse):
            #B Multiplicity as a function of B-hadron pt cut
            h_mulVsPtCut    = BSparse.Projection(2,0)
            h_mulVsPtCut.SetName("MultVsPtCut")
            h_mulVsPtCut.GetXaxis().SetTitle("pt cut")
            h_mulVsPtCut.GetYaxis().SetTitle("# B-hadrons")
            self.__saveHist(h_mulVsPtCut)

            #BSemiElectron multiplicity as a function of B-hadron pt cut
            h_BElVsPtCut    = BSparse.Projection(1,0)
            h_BElVsPtCut.SetName("BElVsPtCut")
            h_BElVsPtCut.GetXaxis().SetTitle("pt cut")
            h_BElVsPtCut.GetYaxis().SetTitle("# B-el")
            self.__saveHist(h_BElVsPtCut)

            #Multiplicity of B Electrons as a function of B- multiplicity
            h_BElVsBMul     = BSparse.Projection(1,2)
            h_BElVsBMul.SetName("BElVsBMul")
            h_BElVsBMul.GetXaxis().SetTitle("nBHadrons")
            h_BElVsBMul.GetYaxis().SetRangeUser(0,8)
            h_BElVsBMul.GetYaxis().SetTitle("# B-el")
            self.__saveHist(h_BElVsBMul)

            #Multiplicity of B-hadrons
            BSparse.GetAxis(0).SetRangeUser(5,6)
            h_BMult_5       = BSparse.Projection(2)
            h_BMult_5.SetMarkerColor(ROOT.kRed+1)
            h_BMult_5.SetName("BMult_5")
            self.__saveHist(h_BMult_5)

            BSparse.GetAxis(0).SetRangeUser(10,11)
            h_BMult_10      = BSparse.Projection(2)
            h_BMult_10.SetMarkerColor(ROOT.kGreen+1)
            h_BMult_10.SetName("BMult_10")
            self.__saveHist(h_BMult_10)

            BSparse.GetAxis(0).SetRangeUser(15,16)
            h_BMult_15      = BSparse.Projection(2)
            h_BMult_15.SetMarkerColor(ROOT.kBlue+1)
            h_BMult_15.SetName("BMult_15")
            self.__saveHist(h_BMult_15)

            BSparse.GetAxis(0).SetRangeUser(20,21)
            h_BMult_20      = BSparse.Projection(2)
            h_BMult_20.SetMarkerColor(ROOT.kYellow+1)
            h_BMult_20.SetName("BMult_20")
            self.__saveHist(h_BMult_20)

            BSparse.GetAxis(0).SetRangeUser(25,26)
            h_BMult_25      = BSparse.Projection(2)
            h_BMult_25.SetMarkerColor(ROOT.kViolet+1)
            h_BMult_25.SetName("BMult_25 ")
            self.__saveHist(h_BMult_25)

            BSparse.GetAxis(0).SetRangeUser(30,31)
            h_BMult_30      = BSparse.Projection(2)
            h_BMult_30.SetMarkerColor(ROOT.kMagenta+1)
            h_BMult_30.SetName("BMult_30")
            self.__saveHist(h_BMult_30)

            #B Semi electron events as a function of B ptcut and B multiplicity
            h_BElEvents     = self.__getInHist("BElEvents")
            h_BElEvents.GetXaxis().SetTitle("B-hadron pt cut [GeV]")
            h_BElEvents.GetYaxis().SetTitle("# B-hadrons")
            self.__saveHist(h_BElEvents)

    def MakeHistBKinematics(self):
        #Retrieve the kinematics histograms
        h_BhdrnBElCorrPt    = self.__getInHist("BHdrnBElPtCorr")

        if(h_BhdrnBElCorrPt):
            h_BhdrnBElCorrPt.GetXaxis().SetRangeUser(0,100)
            h_BhdrnBElCorrPt.GetYaxis().SetRangeUser(0,50)
            h_BhdrnBElCorrPt.GetXaxis().SetTitle("B-hadron pt [GeV]")
            h_BhdrnBElCorrPt.GetYaxis().SetTitle("BEl pt [GeV]")
            self.__saveHist(h_BhdrnBElCorrPt)

            h_BElPt = self.__getInHist("BHdrnBElPtCorr").ProjectionY()
            h_BElPt.SetName("BElPt")
            h_BElPt.GetXaxis().SetTitle("B- el pt [GeV]")
            self.__saveHist(h_BElPt)

            h_BElEff    = self.__getInHist("BElEvntEff")
            h_BElEff.GetXaxis().SetTitle("B-hadron pt-cut [GeV]")
            h_BElEff.GetYaxis().SetTitle("# B-hadrons")
            self.__saveHist(h_BElEff)

            h_BHdrnEvnt = self.__getInHist("BHdrnEvents")
            h_BHdrnEvnt.GetXaxis().SetTitle("B-hadron pt-cut [GeV]")
            h_BHdrnEvnt.GetYaxis().SetTitle("# B-hadrons")
            self.__saveHist(h_BHdrnEvnt)

    def MakeHistCKinematics(self):
        #Retrieve the kinematics histograms
        h_ChdrnCElCorrPt    = self.__getInHist("CHdrnCElPtCorr")
        if(h_ChdrnCElCorrPt):
            h_ChdrnCElCorrPt.GetXaxis().SetRangeUser(0,100)
            h_ChdrnCElCorrPt.GetYaxis().SetRangeUser(0,50)
            h_ChdrnCElCorrPt.GetXaxis().SetTitle("C-hadron pt [GeV]")
            h_ChdrnCElCorrPt.GetYaxis().SetTitle("CEl pt [GeV]")
            self.__saveHist(h_ChdrnCElCorrPt)

            h_CElPt = self.__getInHist("CHdrnCElPtCorr").ProjectionY()
            h_CElPt.SetName("CElPt")
            h_CElPt.GetXaxis().SetTitle("C- el pt [GeV]")
            h_CElPt.GetYaxis().SetTitle("#C-hadrons")
            self.__saveHist(h_CElPt)

    def MakeHistCSemiElectron(self):
        #Retrieve THnSparse
        CSparse         = self.__getInHist("CSemiElectron")
        if(CSparse):
            h_mulVsPtCut    = CSparse.Projection(2,0)
            h_mulVsPtCut.SetName("MultVsPtCut")
            h_mulVsPtCut.GetXaxis().SetTitle("pt cut")
            h_mulVsPtCut.GetYaxis().SetTitle("# C-hadrons")
            self.__saveHist(h_mulVsPtCut)

            h_CElVsPtCut    = CSparse.Projection(1,0)
            h_CElVsPtCut.SetName("CElVsPtCut")
            h_CElVsPtCut.GetXaxis().SetTitle("pt cut")
            h_CElVsPtCut.GetYaxis().SetTitle("# C-el")
            self.__saveHist(h_CElVsPtCut)

            h_CElVsCMul     = CSparse.Projection(1,2)
            h_CElVsCMul.SetName("CElVsCMult")
            h_CElVsCMul.GetXaxis().SetTitle("# C-hadrons")
            h_CElVsCMul.GetYaxis().SetTitle("# C-el")
            h_CElVsCMul.GetYaxis().SetRangeUser(0,8)
            self.__saveHist(h_CElVsCMul)

            #Multiplicity of C-hadrons
            CSparse.GetAxis(0).SetRangeUser(5,6)
            h_CMult_5       = CSparse.Projection(2)
            h_CMult_5.SetMarkerColor(ROOT.kRed+1)
            h_CMult_5.SetName("CMult_5")
            self.__saveHist(h_CMult_5)

            CSparse.GetAxis(0).SetRangeUser(10,11)
            h_CMult_10      = CSparse.Projection(2)
            h_CMult_10.SetMarkerColor(ROOT.kGreen+1)
            h_CMult_10.SetName("CMult_10")
            self.__saveHist(h_CMult_10)

            CSparse.GetAxis(0).SetRangeUser(15,16)
            h_CMult_15      = CSparse.Projection(2)
            h_CMult_15.SetMarkerColor(ROOT.kBlue+1)
            h_CMult_15.SetName("CMult_15")
            self.__saveHist(h_CMult_15)

            CSparse.GetAxis(0).SetRangeUser(20,21)
            h_CMult_20      = CSparse.Projection(2)
            h_CMult_20.SetMarkerColor(ROOT.kYellow+1)
            h_CMult_20.SetName("CMult_20")
            self.__saveHist(h_CMult_20)

            CSparse.GetAxis(0).SetRangeUser(25,26)
            h_CMult_25      = CSparse.Projection(2)
            h_CMult_25.SetMarkerColor(ROOT.kViolet+1)
            h_CMult_25.SetName("CMult_25")
            self.__saveHist(h_CMult_25)

            CSparse.GetAxis(0).SetRangeUser(30,31)
            h_CMult_30      = CSparse.Projection(2)
            h_CMult_30.SetMarkerColor(ROOT.kMagenta+1)
            h_CMult_30.SetName("CMult_30")
            self.__saveHist(h_CMult_30)

            #C Semi electron events as a function of C ptcut and C multiplicity
            h_CElEvents     = self.__getInHist("CElEvents")
            h_CElEvents.GetXaxis().SetTitle("C-hadron pt cut [GeV]")
            h_CElEvents.GetYaxis().SetTitle("# C-hadrons")
            self.__saveHist(h_CElEvents)

#End of class plotscript
