import abc
from NtupleAnaBase import NtupleAnaBase
from ZeeBParticle import ZeeBElectron
from array import array
import math

import ROOT, sys,os,glob 
from multiprocessing import Process,Queue,current_process

from PyAnalysisUtils import PDG


class NtupleAna(NtupleAnaBase):
    def __init__(self,tree,outFileName, scaleFactor = 1, hists = {}):
        self.hists  = hists
        self.myTree   = tree

        '''Histogram Scale Factor (Lumi Scale Factor)'''
        self.scaleFactor    = scaleFactor

        self.outFileName    = outFileName
    
    def initialize(self):
        #Used in doAuthorEl()
        self.regPtHist("ElPt")
        self.regEtaHist("ElEta")
        self.regPhiHist("ElPhi")
        self.regTH1("ElEnrgy","",500,0,500)
        self.regTH1("ElMultplcty","",20,-0.5,19.5)

        self.regPtHist("SlctdElPt")
        self.regEtaHist("SlctdElEta")
        self.regPhiHist("SlctdElPhi")
        self.regTH1("SlctdElE","",4500,0,500)
        self.regTH1("SlctdElMultplcty","",8,-0.5,7.5)

        self.regTHnSparse("ZCandKine","",nbins=7,\
                bins=[1000,100,500, 1000, 100, 500,400],\
                fmin=[0  , -5, -5, 0   , -5 , -5 ,0 ],\
                fmax=[500, 5 ,  5, 500 , 5  , 5  ,200])

        self.regTH1("ElParentPDG","ElParentPDG",10,0,10)
        self.regTH1("ElGrndParentPDG","El Grand Parent PDG",10,0,10)
        self.regTH1("BMtchElMultplcty","",8,-0.5,7.5)
        self.regTH1("BGrndMtchElMultplcty","",8,-0.5,7.5)

        '''Truth'''
        self.regTH1("Bhadrons")
        self.regTH1("BHdrnpt","",500,0,500)
        self.regTH1("BHdrneta","",100,-5,5)
        self.regTH2("BHdrnBElPtCorr","",500,0,500,100,0,100)
        self.regTH2("BHdrnEvents","",35,0.5,35.5,10,-0.5,9.5)

        self.regTH1("Chadrons")
        self.regTH2("CHdrnCElPtCorr","",500,0,500,100,0,100)
        self.regPtHist("Cpt")
        self.regEtaHist("Ceta")
        self.regPtHist("CscdPt")
        self.regEtaHist("CscdEta")
        self.regTH2("CHdrnEvents","",35,0.5,35.5,10,-0.5,9.5)
        
        self.regTH2("hardVsSoftEl","",10,0.5,10.5,10,0.5,10.5)

        '''SemiElectron decay '''
        #Bptcut,elptCut,nBEl,nBHadrons
        self.regTHnSparse("BSemiElectron","",nbins=4,\
                bins=[35  ,35  ,11  , 11],\
                fmin=[0.5 ,0.5 ,-0.5, -0.5  ],\
                fmax=[35.5,35.5,10.5, 10.5 ])

        self.regTHnSparse("CSemiElectron","",nbins=4,\
                bins=[35  ,35  ,11  , 11],\
                fmin=[0.5 ,0.5 ,-0.5, -0.5  ],\
                fmax=[35.5,35.5,10.5, 10.5 ])

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
        self.doAuthorEl()
        #self.doTruthAna()
        
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

    
    def doTruthAna(self):
        ptcutlist = [5,10,15,20,25,30]
        for hadPtCut in ptcutlist:
            for ElptCut in ptcutlist:
                nBHadrons   = self.getBMultplcty(hadPtCut,2.5)
                nBEl        = self.getBElMultplcty(hadPtCut,ElptCut,2.5,2.5)
                nCHadrons   = self.getCMultplcty(hadPtCut,2.5)
                nCEl        = self.getCElMultplcty(hadPtCut,ElptCut,2.5,2.5)


                x = [hadPtCut,ElptCut,nBEl,nBHadrons]
                self.gethist("BSemiElectron").Fill(array("d",x))

                x = [hadPtCut,ElptCut,nCEl,nCHadrons]
                self.gethist("CSemiElectron").Fill(array("d",x))

    '''The Author Electron is an electron with track pointing to the cluster identified'''
    def doAuthorEl(self):
        clptVec     = self.getCurrentValue("el_cl_Pt")
        cletaVec    = self.getCurrentValue("el_cl_Eta")
        clphiVec    = self.getCurrentValue("el_cl_Phi")
        clEVec      = self.getCurrentValue("el_cl_E")

        softeVec    = self.getCurrentValue("elAuthorSofte")
        authorVec   = self.getCurrentValue("elAuthor")

        chrgVec     = self.getCurrentValue("el_charge")
        looseIdVec  = self.getCurrentValue("el_loose")
        lossePPIdVec= self.getCurrentValue("el_loosePP")
        medIdVec    = self.getCurrentValue("el_medium")
        medPPIdVec  = self.getCurrentValue("el_mediumPP")
        tightIdVec  = self.getCurrentValue("el_tight")
        tightPPIdVec= self.getCurrentValue("el_tightPP")

        elParentVec = self.getCurrentValue("mtchdParent")
        elGrndParentVec = self.getCurrentValue("mtchdGrndParent")

        nBMtchEl    = 0
        nBGrndMtchEl= 0

        nCMtchEl    = 0
        nCGrndMtchEl= 0

        elList      = []

        for i in xrange(cletaVec.size()):
            clEta   = cletaVec.at(i)
            clPt    = clptVec.at(i)
            clPhi   = clphiVec.at(i)
            clE     = clEVec.at(i)

            author  = authorVec.at(i)
            softe   = softeVec.at(i)
            clChrg  = chrgVec.at(i)
            looseId = looseIdVec.at(i)
            loosePPId=lossePPIdVec.at(i)
            medId   = medIdVec.at(i)
            medPPId = medPPIdVec.at(i)
            tightId = tightIdVec.at(i)
            tightPPid=tightPPIdVec.at(i)

            elParentPDG     = elParentVec.at(i)
            elGrnParentPDG  = elGrndParentVec.at(i)

            if(clPt > 20):

                elCand = ZeeBElectron(clPt,clEta,clPhi,clE,clChrg)
                elCand.setAuthor(author)
                elCand.setSofte(softe)
                elCand.setLoose(looseId)
                elCand.setLoosePP(loosePPId)
                elCand.setTight(tightId)
                elCand.setMedium(medId)
                elCand.setMediumPP(medPPId)

                elList += [elCand]

                self.gethist("ElEta").Fill(clEta)
                self.gethist("ElPt").Fill(clPt)
                self.gethist("ElPhi").Fill(clPhi)
                self.gethist("ElEnrgy").Fill(clE)

                """Truth match"""
                if(elParentPDG != -100):
                    self.gethist("ElParentPDG").Fill(PDG.pdgid_to_name(elParentPDG),1)
                    if(self.isBHadron(elParentPDG)):
                        nBMtchEl +=1
                    elif(self.isCHadron(elParentPDG)):
                        nCMtchEl +=1

                if(elGrnParentPDG != -100):
                    self.gethist("ElGrndParentPDG").Fill(PDG.pdgid_to_name(elGrnParentPDG),1)
                    if(self.isBHadron(elGrnParentPDG)):
                        nBGrndMtchEl +=1
                    elif(self.isCHadron(elParentPDG)):
                        nCGrndMtchEl +=1
            

        #End of loop

        if(len(elList)>1):
            self.gethist("ElMultplcty").Fill(len(elList))
            self.gethist("BMtchElMultplcty").Fill(nBMtchEl)
            self.gethist("BGrndMtchElMultplcty").Fill(nBGrndMtchEl)
            
            elSelectList= [x for x in elList if x.isLoosePP() and (x.isAuthorSofte() or x.isAuthor()) ]
            self.gethist("SlctdElMultplcty").Fill(len(elSelectList))
            for el in elSelectList:
                self.gethist("SlctdElPt").Fill(el.getPt())
                self.gethist("SlctdElEta").Fill(el.getEta())
                self.gethist("SlctdElPhi").Fill(el.getPhi())

            '''Invariant mass of Z boson'''
            elPairList = [(ePlus,eMinus) for ePlus in [x for  x in elList if x.getCharge()==1] \
                    for eMinus in [x for x in elList if x.getCharge()==-1] \
                    if  ePlus != eMinus and ePlus.isZElectron() and eMinus.isZElectron()]

            ZElList = [x for x in elList if x.isZElectron()]
            ZElList.sort(key = lambda x: x.getPt() )
            el1 = ZElList.pop()
            el2 = ZElList.pop()

            if((el1.getCharge()==1 and el2.getCharge() ==-1) or (el1.getCharge()==-1 and el2.getCharge()==1)):
                InvMass = (el1.getLorentzVector() + el2.getLorentzVector()).M()
                
                if(InvMass > 40):
                    zCandKine   = [el1.getPt(),el1.getEta(),el1.getPhi(),el2.getPt(),el2.getEta(),el2.getPhi(),InvMass]
                    self.gethist("ZCandKine").Fill(array("d",zCandKine))


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

    '''Return the multiplicity of B decay electrons as a function of 
    the El pt cut and B-hadron pt cut'''
    def getBElMultplcty(self,Bptcut, Elptcut,Betacut, Eletacut):
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

            if(BHdrnPt > Bptcut and abs(BHdrnEta) < Betacut):
                if(SemiEl ==1):

                    elPass = False
                    for k in xrange(BElPtVec.size()):
                        BElPt   = BElPtVec.at(k)
                        BElEta  = BElEtaVec.at(k)
                        
                        if(BElPt> Elptcut and abs(BElEta) < Eletacut):
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

    '''Return the multiplicity of C decay electrons as a function of 
    the El pt cut and C-hadron pt cut'''
    def getCElMultplcty(self,Cptcut, Elptcut,Cetacut, Eletacut):
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

            if(CHdrnPt > Cptcut and abs(CHdrnEta) < Cetacut):
                if(SemiEl ==1):

                    elPass = False
                    for k in xrange(CElPtVec.size()):
                        CElPt   = CElPtVec.at(k)
                        CElEta  = CElEtaVec.at(k)
                        
                        if(CElPt> Elptcut and abs(CElEta) < Eletacut):
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

    def getNBEl(self,elptcut,eletacut):
        BElEtaVec   = self.getCurrentValue("BsemiElEta")
        BElPtVec    = self.getCurrentValue("BsemiElPt")
        isSemiElVec = self.getCurrentValue("BisSemiElectron")

        nBEl = 0
        for i in xrange(BElPtVec.size()):
            BElPt   = BElPtVec.at(i)
            BElEta  = BElPtVec.at(i)
            isSemi  = isSemiElVec.at(i)
            if(isSemi and BElPt>= elptcut and abs(BElEta) < eletacut):
                nBEl +=1

        return nBEl


    
    #Fill Z Boson Mass and its Z cand electrons
    #kinematics into a THnSparse  
    def FillZCandKinematics(self,el1,el2):
        fourVector  = el1.getLorentzVector() + el2.getLorentzVector()
        InvMass     = fourVector.M()
        zCandKine   = [el1.getPt(),el1.getEta(),el1.getPhi(),el2.getPt(),el2.getEta(),el2.getPhi(),InvMass]
        self.gethist("ZCandKine").Fill(array("d",zCandKine))

    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    '''Best boson the one which is closer to nominal mass'''
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    def getBestBosIndx(self,elPairList):
        nominalZMass = 91.187
        mDiff        = 100
        BestBosIndx  = -1
        for elPair  in elPairList:
            m        = (elPair[0].getLorentzVector() + elPair[1].getLorentzVector()).M()
            if(abs(m - nominalZMass) < mDiff):
                mDiff       = abs(m - nominalZMass)
                BestBosIndx = elPairList.index(elPair)

        return BestBosIndx

    
    ''''''''''''''''''''''''''''''''
    '''Function to add histograms to the analysis chain'''
    ''''''''''''''''''''''''''''''''
    def regPtHist(self,name):
        self.regTH1(name,";[GeV]",500,0,500)


    def regEtaHist(self,name):
        self.regTH1(name,";#eta",500,-5,5)

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
        self.__saveHist(self.__getInHist("ElPt"))
        self.__saveHist(self.__getInHist("ElEta"))
        self.__saveHist(self.__getInHist("ElPhi"))
        self.__saveHist(self.__getInHist("ElEnrgy"))

        h_nEl = self.__getInHist("ElMultplcty")
        h_nEl.GetXaxis().SetTitle("#")
        self.__saveHist(h_nEl)

        self.__saveHist(self.__getInHist("SlctdElPt"))
        self.__saveHist(self.__getInHist("SlctdElEta"))
        self.__saveHist(self.__getInHist("SlctdElPhi"))

        h_nSlctdEl = self.__getInHist("SlctdElMultplcty")
        h_nSlctdEl.GetXaxis().SetTitle("#")
        self.__saveHist(h_nSlctdEl)

        """Truth Match Electrons"""
        self.__saveHist(self.__getInHist("ElParentPDG"))
        self.__saveHist(self.__getInHist("ElGrndParentPDG"))
        self.__saveHist(self.__getInHist("BMtchElMultplcty"))
        self.__saveHist(self.__getInHist("BGrndMtchElMultplcty"))

        ###################################################
        #ZBoson kinematics from THnSparse.
        h_ZCandSparse    = self.__getInHist("ZCandKine")

        if(h_ZCandSparse):
            h_ZBosonEplusPt  = h_ZCandSparse.Projection(0)
            h_ZBosonEplusPt.SetName("BosonCandidateEplusPt")
            h_ZBosonEplusPt.GetXaxis().SetTitle("[GeV]")
            h_ZBosonEplusPt.GetXaxis().SetRangeUser(0,250)
            self.__saveHist(h_ZBosonEplusPt)

            h_ZBosonEMinusPt = h_ZCandSparse.Projection(3)
            h_ZBosonEMinusPt.SetName("BosonCandidateEMinusPt")
            h_ZBosonEMinusPt.GetXaxis().SetTitle("[GeV]")
            self.__saveHist(h_ZBosonEMinusPt)

            h_ZBosonMass    = h_ZCandSparse.Projection(6)
            h_ZBosonMass.SetName("ZBosonMass")
            h_ZBosonMass.GetXaxis().SetTitle("[GeV]")
            self.__saveHist(h_ZBosonMass)

    
    def MakeHistBSemiElectron(self):
        #Retrieve the THnSparse
        BSparse         = self.__getInHist("BSemiElectron")
        if(BSparse):
            #B Electron Multiplicity as a function of B Electron pt cut
            h_ElMulVsElPtCut    = BSparse.Projection(2,1)
            h_ElMulVsElPtCut.SetName("B_ElMultVsElPtCut")
            h_ElMulVsElPtCut.GetXaxis().SetTitle("pt cut")
            h_ElMulVsElPtCut.GetYaxis().SetTitle("# B-hadrons")
            self.__saveHist(h_ElMulVsElPtCut)

            #B Hadron B Electron Correlation
            h_ElPtCutVsBPtCut   = BSparse.Projection(1,0)
            h_ElPtCutVsBPtCut.SetName("B_ElPtCutCorrelation")
            h_ElPtCutVsBPtCut.GetXaxis().SetTitle("B pt [GeV]")
            h_ElPtCutVsBPtCut.GetYaxis().SetTitle("El pt [GeV]")
            self.__saveHist(h_ElPtCutVsBPtCut)

            #BSemiElectron multiplicity as a function of B-hadron pt cut
            h_BElVsPtCut    = BSparse.Projection(2,0)
            h_BElVsPtCut.SetName("BElVsPtCut")
            h_BElVsPtCut.GetXaxis().SetTitle("pt cut")
            h_BElVsPtCut.GetYaxis().SetTitle("# B-el")
            self.__saveHist(h_BElVsPtCut)

            #Multiplicity of B Electrons as a function of B- multiplicity
            h_BElVsBMul     = BSparse.Projection(2,3)
            h_BElVsBMul.SetName("BElVsBMul")
            h_BElVsBMul.GetXaxis().SetTitle("nBHadrons")
            h_BElVsBMul.GetYaxis().SetRangeUser(0,8)
            h_BElVsBMul.GetYaxis().SetTitle("# B-el")
            self.__saveHist(h_BElVsBMul)

            #Multiplicity of B-hadrons at a give B-hadron pt
            BSparse.GetAxis(0).SetRangeUser(5,6)
            h_BMult_5       = BSparse.Projection(3)
            h_BMult_5.SetFillStyle(3644)
            h_BMult_5.SetName("BMult_5")
            self.__saveHist(h_BMult_5)

            BSparse.GetAxis(0).SetRangeUser(10,11)
            h_BMult_10      = BSparse.Projection(3)
            h_BMult_10.SetFillStyle(3544)
            h_BMult_10.SetName("BMult_10")
            self.__saveHist(h_BMult_10)

            BSparse.GetAxis(0).SetRangeUser(15,16)
            h_BMult_15      = BSparse.Projection(3)
            h_BMult_15.SetFillStyle(3444)
            h_BMult_15.SetName("BMult_15")
            self.__saveHist(h_BMult_15)

            BSparse.GetAxis(0).SetRangeUser(20,21)
            h_BMult_20      = BSparse.Projection(3)
            h_BMult_20.SetFillStyle(3344)
            h_BMult_20.SetName("BMult_20")
            self.__saveHist(h_BMult_20)

            BSparse.GetAxis(0).SetRangeUser(25,26)
            h_BMult_25      = BSparse.Projection(3)
            h_BMult_25.SetFillStyle(3244)
            h_BMult_25.SetName("BMult_25 ")
            self.__saveHist(h_BMult_25)

            BSparse.GetAxis(0).SetRangeUser(30,31)
            h_BMult_30      = BSparse.Projection(3)
            h_BMult_30.SetFillStyle(3144)
            h_BMult_30.SetName("BMult_30")
            self.__saveHist(h_BMult_30)

            #Multiplicity of B-electrons at a a given el-pt
            BSparse.GetAxis(0).SetRangeUser(BSparse.GetAxis(0).GetXmin(),BSparse.GetAxis(0).GetXmax())
            BSparse.GetAxis(1).SetRangeUser(BSparse.GetAxis(1).GetXmin(),BSparse.GetAxis(1).GetXmax())
            BSparse.GetAxis(2).SetRangeUser(BSparse.GetAxis(2).GetXmin(),BSparse.GetAxis(2).GetXmax())
            BSparse.GetAxis(3).SetRangeUser(BSparse.GetAxis(3).GetXmin(),BSparse.GetAxis(3).GetXmax())


            BSparse.GetAxis(1).SetRangeUser(5,6)
            h_BMult_elPt_5  = BSparse.Projection(2)
            h_BMult_elPt_5.SetName("BElMult_5")
            h_BMult_elPt_5.SetFillStyle(3644)
            self.__saveHist(h_BMult_elPt_5)

            BSparse.GetAxis(1).SetRangeUser(10,11)
            h_BMult_elPt_10 = BSparse.Projection(2)
            h_BMult_elPt_10.SetName("BElMult_10")
            h_BMult_elPt_10.SetFillStyle(3544)
            self.__saveHist(h_BMult_elPt_10)

            BSparse.GetAxis(1).SetRangeUser(15,16)
            h_BMult_elPt_15 = BSparse.Projection(2)
            h_BMult_elPt_15.SetName("BElMult_15")
            h_BMult_elPt_15.SetFillStyle(3444)
            self.__saveHist(h_BMult_elPt_15)

            BSparse.GetAxis(1).SetRangeUser(20,21)
            h_BMult_elPt_20 = BSparse.Projection(2)
            h_BMult_elPt_20.SetName("BElMult_20")
            h_BMult_elPt_20.SetFillStyle(3344)
            self.__saveHist(h_BMult_elPt_20)

            BSparse.GetAxis(1).SetRangeUser(25,26)
            h_BMult_elPt_25 = BSparse.Projection(2)
            h_BMult_elPt_25.SetName("BElMult_25")
            h_BMult_elPt_25.SetFillStyle(3244)
            self.__saveHist(h_BMult_elPt_25)

            BSparse.GetAxis(1).SetRangeUser(30,31)
            h_BMult_elPt_30 = BSparse.Projection(2)
            h_BMult_elPt_30.SetName("BElMult_30")
            h_BMult_elPt_30.SetFillStyle(3144)
            self.__saveHist(h_BMult_elPt_30)


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
        #Retrieve the THnSparse
        CSparse         = self.__getInHist("CSemiElectron")
        if(CSparse):
            #C Electron Multiplicity as a function of C Electron pt cut
            h_ElMulVsElPtCut    = CSparse.Projection(2,1)
            h_ElMulVsElPtCut.SetName("C_ElMultVsElPtCut")
            h_ElMulVsElPtCut.GetXaxis().SetTitle("pt cut")
            h_ElMulVsElPtCut.GetYaxis().SetTitle("# C-hadrons")
            self.__saveHist(h_ElMulVsElPtCut)

            #C Hadron C Electron Correlation
            h_ElPtCutVsCPtCut   = CSparse.Projection(1,0)
            h_ElPtCutVsCPtCut.SetName("C_ElPtCutCorrelation")
            h_ElPtCutVsCPtCut.GetXaxis().SetTitle("C pt [GeV]")
            h_ElPtCutVsCPtCut.GetYaxis().SetTitle("El pt [GeV]")
            self.__saveHist(h_ElPtCutVsCPtCut)

            #CSemiElectron multiplicity as a function of C-hadron pt cut
            h_CElVsPtCut    = CSparse.Projection(2,0)
            h_CElVsPtCut.SetName("CElVsPtCut")
            h_CElVsPtCut.GetXaxis().SetTitle("pt cut")
            h_CElVsPtCut.GetYaxis().SetTitle("# C-el")
            self.__saveHist(h_CElVsPtCut)

            #Multiplicity of C Electrons as a function of C- multiplicity
            h_CElVsCMul     = CSparse.Projection(2,3)
            h_CElVsCMul.SetName("CElVsCMul")
            h_CElVsCMul.GetXaxis().SetTitle("nCHadrons")
            h_CElVsCMul.GetYaxis().SetRangeUser(0,8)
            h_CElVsCMul.GetYaxis().SetTitle("# C-el")
            self.__saveHist(h_CElVsCMul)

            #Multiplicity of C-hadrons at a give C-hadron pt
            CSparse.GetAxis(0).SetRangeUser(5,6)
            h_CMult_5       = CSparse.Projection(3)
            h_CMult_5.SetFillStyle(3644)
            h_CMult_5.SetName("CMult_5")
            self.__saveHist(h_CMult_5)

            CSparse.GetAxis(0).SetRangeUser(10,11)
            h_CMult_10      = CSparse.Projection(3)
            h_CMult_10.SetFillStyle(3544)
            h_CMult_10.SetName("CMult_10")
            self.__saveHist(h_CMult_10)

            CSparse.GetAxis(0).SetRangeUser(15,16)
            h_CMult_15      = CSparse.Projection(3)
            h_CMult_15.SetFillStyle(3444)
            h_CMult_15.SetName("CMult_15")
            self.__saveHist(h_CMult_15)

            CSparse.GetAxis(0).SetRangeUser(20,21)
            h_CMult_20      = CSparse.Projection(3)
            h_CMult_20.SetFillStyle(3344)
            h_CMult_20.SetName("CMult_20")
            self.__saveHist(h_CMult_20)

            CSparse.GetAxis(0).SetRangeUser(25,26)
            h_CMult_25      = CSparse.Projection(3)
            h_CMult_25.SetFillStyle(3244)
            h_CMult_25.SetName("CMult_25 ")
            self.__saveHist(h_CMult_25)

            CSparse.GetAxis(0).SetRangeUser(30,31)
            h_CMult_30      = CSparse.Projection(3)
            h_CMult_30.SetFillStyle(3144)
            h_CMult_30.SetName("CMult_30")
            self.__saveHist(h_CMult_30)

            #Multiplicity of C-electrons at a a given el-pt
            CSparse.GetAxis(0).SetRangeUser(CSparse.GetAxis(0).GetXmin(),CSparse.GetAxis(0).GetXmax())
            CSparse.GetAxis(1).SetRangeUser(CSparse.GetAxis(1).GetXmin(),CSparse.GetAxis(1).GetXmax())
            CSparse.GetAxis(2).SetRangeUser(CSparse.GetAxis(2).GetXmin(),CSparse.GetAxis(2).GetXmax())
            CSparse.GetAxis(3).SetRangeUser(CSparse.GetAxis(3).GetXmin(),CSparse.GetAxis(3).GetXmax())


            CSparse.GetAxis(1).SetRangeUser(5,6)
            h_CMult_elPt_5  = CSparse.Projection(2)
            h_CMult_elPt_5.SetName("CElMult_5")
            h_CMult_elPt_5.SetFillStyle(3644)
            self.__saveHist(h_CMult_elPt_5)

            CSparse.GetAxis(1).SetRangeUser(10,11)
            h_CMult_elPt_10 = CSparse.Projection(2)
            h_CMult_elPt_10.SetName("CElMult_10")
            h_CMult_elPt_10.SetFillStyle(3544)
            self.__saveHist(h_CMult_elPt_10)

            CSparse.GetAxis(1).SetRangeUser(15,16)
            h_CMult_elPt_15 = CSparse.Projection(2)
            h_CMult_elPt_15.SetName("CElMult_15")
            h_CMult_elPt_15.SetFillStyle(3444)
            self.__saveHist(h_CMult_elPt_15)

            CSparse.GetAxis(1).SetRangeUser(20,21)
            h_CMult_elPt_20 = CSparse.Projection(2)
            h_CMult_elPt_20.SetName("CElMult_20")
            h_CMult_elPt_20.SetFillStyle(3344)
            self.__saveHist(h_CMult_elPt_20)

            CSparse.GetAxis(1).SetRangeUser(25,26)
            h_CMult_elPt_25 = CSparse.Projection(2)
            h_CMult_elPt_25.SetName("CElMult_25")
            h_CMult_elPt_25.SetFillStyle(3244)
            self.__saveHist(h_CMult_elPt_25)

            CSparse.GetAxis(1).SetRangeUser(30,31)
            h_CMult_elPt_30 = CSparse.Projection(2)
            h_CMult_elPt_30.SetName("CElMult_30")
            h_CMult_elPt_30.SetFillStyle(3144)
            self.__saveHist(h_CMult_elPt_30)

#End of plotscript class
