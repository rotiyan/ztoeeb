import abc
from NtupleAnaBase import NtupleAnaBase
from array import array

import ROOT, sys,os,glob 
from multiprocessing import Process,Queue,current_process


class NtupleAna(NtupleAnaBase):
    def __init__(self,tree,outFileName,hists = {}):
        self.hists  = hists
        self.myTree   = tree
        self.barrelEtacut   = 2.47
        self.cracketa1      = 1.37
        self.cracketa2      = 1.52

        self.outFileName    = outFileName
    
    def initialize(self):
        self.addPtHist("trkPt")
        self.addPtHist("trkPtMtchd")
        self.addPtHist("trkMtchEffVsPt")
        self.addPtHist("clPt")
        self.addPtHist("clPtMtchd")
        self.addPtHist("clMtchEffVsPt")
        self.addPtHist("trthPt")

        self.addEtaHist("trkEta")
        self.addEtaHist("trkEtaMtchd")
        self.addEtaHist("trkMtchEffVsEta")
        self.addEtaHist("clEta")
        self.addEtaHist("clEtaMtchd")
        self.addEtaHist("clMtchEffVsEta")
        self.addEtaHist("trthEta")

        self.addPhiHist("trkPhi")
        self.addPhiHist("trkPhiMtchd")
        self.addPhiHist("trkMtchEffVsPhi")
        self.addPhiHist("clPhi")
        self.addPhiHist("clPhiMtchd")
        self.addPhiHist("clMtchEffVsPhi")
        self.addPhiHist("trthPhi")

        self.addh1("elParent")

        '''Truth'''
        self.addh1("Bhadrons")
        self.addh1("Bpt","",500,0,500)
        self.addh1("Beta","",100,-5,5)
        self.addh3("BMultiplcty","BMultiplcty;ptcut;etacut;nBHadrons",15,0,30,10,0,10,10,0,10)
        self.addProfile2D("BElEvents","",30,0.5,30.5,10,-0.5,9.5)

        self.addh1("Chadrons")
        self.addPtHist("Cpt")
        self.addEtaHist("Ceta")
        self.addPtHist("CscdPt")
        self.addEtaHist("CscdEta")
        self.addh3("CMultiplcty","CMultiplcty;ptcut;etacut;nCHadrons",50,0,50,10,0,10,10,0,10)

        self.addh2("matchedZElVsBEl",";# ZEl; #BEl",10,0.5,10.5,10,0.5,10.5)
        self.addh2("matchedZElVsCEl",";# ZEl; #BEl",10,0.5,10.5,10,0.5,10.5)
        self.addh2("hardVsSoftEl","",10,0.5,10.5,10,0.5,10.5)

#        self.addh3("BbdeltaRMin",";ptcut;etacut;#Delta R min",50,0,50,10,0,10,500,0,5)
#        self.addh3("BbdeltaRMax",";ptcut;etacut;#Delta R max",50,0,50,10,0,10,500,0,5)
#
#        self.addh3("CbdeltaRMin",";ptcut;etacut;#Delta R min",50,0,50,10,0,10,500,0,5)
#        self.addh3("CbdeltaRMax",";ptcut;etacut;#Delta R max",50,0,50,10,0,10,500,0,5)
#
#        self.addh3("CascadedRMin",";ptcut;etacut;#Delta R min",50,0,50,10,0,10,500,0,5)
#        self.addh3("CascadedRMax",";ptcut;etacut;#Delta R min",50,0,50,10,0,10,500,0,5)
        
        '''pt,eta,nB,nC,dr{},dr{}'''
        self.addhnSparse("BHadron_1","",nbins=4,\
                bins=[100,100,100,100],\
                fmin=[0  ,-5 ,0  ,0  ],\
                fmax=[100, 5 ,1  ,1  ])

        self.addhnSparse("BHadron_2","",nbins=6,\
                bins=[100,100,100,100,100,100],\
                fmin=[0  ,-5 ,0  ,0  ,0  ,0],\
                fmax=[100,5  ,1  ,1  ,1  ,1])

        '''SemiElectron decay '''
        self.addhnSparse("BSemiElectron","",nbins=3,\
                bins=[30  ,11  ,10],\
                fmin=[0.5 ,-0.5,0.5  ],\
                fmax=[30.5,10.5,10.5 ])

        self.addhnSparse("CSemiElectron","",nbins=3,\
                bins=[30  ,11  ,10],\
                fmin=[0.5 ,-0.5,0.5  ],\
                fmax=[30.5,10.5,10.5 ])


        '''ZB electron'''
        self.addhnSparse("ZBEl","",nbins=5,\
                bins=[200,200,100,100,10],\
                fmin=[0  ,0  ,-5 , -5,0.5 ],\
                fmax=[200,200,5  ,  5,10.5])

        '''bquark correlation'''
        self.addPtHist("bQuarkPt")

        self.addh3("bquarkPtDeltaR",";b1;b2",200,0,200,200,0,200,100,0,2)
        self.addhnSparse("bquarkKinematics",nbins=7,\
                bins=[200,200,100,100,100,100,100],\
                fmin=[0  ,0  ,0  ,-5 ,-5 ,-5 ,-4 ],\
                fmax=[100,100,100,5  , 5 ,5  , 4 ])

        self.addh1("nbQuark","",10,0.5,10.5)
        
        '''Z boson decay electrons'''
        self.addPtHist("ZElPt")
        self.addEtaHist("ZElEta")
        self.addPhiHist("ZElPhi")


    def execute(self):
        #Electron cluster analysis
        self.clAna()
        
        #Electron track particle analaysis
        self.trkAna()
        
        #Electron truth analysis
        self.trthAna()

        self.fillBKinematics(2,2.5)
        self.fillCKinematics(2,2.5)

        self.makeDeltaRPlots()

        self.hadronElectronMatching()

        self.doZElectron()

    def finalize(self):
        '''Efficiency histograms'''
        self.gethist("trkMtchEffVsPt").Divide(self.gethist("trkPtMtchd"), self.gethist("trkPt"))
        self.gethist("trkMtchEffVsEta").Divide(self.gethist("trkEtaMtchd"), self.gethist("trkEta"))
        self.gethist("trkMtchEffVsPhi").Divide(self.gethist("trkPhiMtchd"), self.gethist("trkPhi"))

        self.gethist("clMtchEffVsPt").Divide(self.gethist("clPtMtchd"), self.gethist("clPt"))
        self.gethist("clMtchEffVsEta").Divide(self.gethist("clEtaMtchd"), self.gethist("clEta"))
        self.gethist("clMtchEffVsPhi").Divide(self.gethist("clPhiMtchd"), self.gethist("clPhi"))

        '''Compare B-hadron b-quark delta r'''
        #self.compareBbDeltaR(5,2.5)
        #self.compareBbDeltaR(10,2.5)
        #self.compareBbDeltaR(20,2.5)

        '''Compare C-hadron b-quark delta r'''
        #self.compareCbDeltaR(5,2.5)
        #self.compareCbDeltaR(10,2.5)
        #self.compareCbDeltaR(20,2.5)


        #Save histograms to disk
        histlist = self.gethistList()
        fname = self.outFileName+current_process().name
        f = ROOT.TFile(fname,"RECREATE")
        for h in histlist:
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

        ptcutlist = [2,5,10,15,20]

        ZElPt       = self.getCurrentValue("ZElPt")
        ZElEta      = self.getCurrentValue("ZElEta")

        for ptcut in ptcutlist:
            nBHadrons   = self.getBMultplcty(ptcut,2.5)
            nBEl        = self.getBElMultplcty(ptcut,2.5)

            if(BSemiElPtVec.size()):
                x = [ptcut,nBEl,nBHadrons]
                self.gethist("BSemiElectron").Fill(array("d",x))

                isSemiBEvent = 0
                if(nBEl):
                    isSemiBEvent =1
                self.gethist("BElEvents").Fill(ptcut,nBHadrons,isSemiBEvent)

                y = [ZElPt[0],ZElPt[1],ZElEta[0],ZElEta[1],BSemiElPtVec[0],BSemiElEtaVec[0],nBHadrons]
                self.gethist("ZBEl").Fill(array("d",y));

            nCHadrons   = self.getCMultplcty(ptcut,2.5)
            nCEl        = self.getCElMultplcty(ptcut,2.5)

            if(CSemiElPtVec.size()):
                x = [ptcut,nCEl,nCHadrons]
                self.gethist("CSemiElectron").Fill(array("d",x))


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
            
            
            ptcutlist   = [2,5,10,15,20]
            etacutlist  = [2.5,5]

            for pt in ptcutlist:
                for eta in etacutlist:
                    self.gethist("BMultiplcty").Fill(pt,eta,self.getBMultplcty(pt,eta))
                    self.gethist("CMultiplcty").Fill(pt,eta,self.getCMultplcty(pt,eta))

                    nBhadrons   = self.getBMultplcty(pt,eta)
                    nChadrons   = self.getCMultplcty(pt,eta)
                    
                    BPtEtaPhi   = zip(BPtVec,BEtaVec,BPhiVec)
                    bPtEtaPhi   = zip(bQuarkME_pt,bQuarkME_eta,bQuarkME_phi)
                    CPtEtaPhi   = zip(CPtVec,CEtaVec,CPhiVec)

                    Bbdrlist    = [ self.deltaR(eta1,phi1,eta2,phi2) for pt1,eta1,phi1  in  BPtEtaPhi for pt2,eta2, phi2 in bPtEtaPhi]
                    Bbdrlist.sort()
                    Cbdrlist    = [ self.deltaR(eta1,phi1,eta2,phi2) for pt1,eta1,phi1  in  CPtEtaPhi for pt2,eta2, phi2 in bPtEtaPhi]
                    Cbdrlist.sort()
                    
                    x = []
                    x.append(pt)
                    x.append(eta)
                    x = x + Bbdrlist
                    if(nBhadrons ==1):                  
                        self.gethist("BHadron_1").Fill(array("d",x))
                    if(nBhadrons ==2):
                        self.gethist("BHadron_2").Fill(array("d",x))



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

        isSemiElVec = self.getCurrentValue("BisSemiElectron")

        nBEl    = 0
        for i in xrange(BPtVec.size()):
            BHdrnPt = BPtVec.at(i)
            BHdrnEta= BEtaVec.at(i)
            SemiEl  = isSemiElVec.at(i)

            if(BHdrnPt > ptcut and abs(BHdrnEta) < etacut):
                if(SemiEl ==1):
                    nBEl +=1

        return nBEl

    def fillBKinematics(self,ptcut,etacut):
        BPtVec      = self.getCurrentValue("BPt")
        BEtaVec     = self.getCurrentValue("BEta")
        BPhiVec     = self.getCurrentValue("BPhi")

        isSemiElVec = self.getCurrentValue("BisSemiElectron")
        
        nBHadrons = 0
        for i in xrange(BPtVec.size()):
            BHdrnPt = BPtVec.at(i)
            BHdrnEta= BEtaVec.at(i)

            if(BHdrnPt >ptcut and abs(BHdrnEta) < etacut):
                self.gethist("Bhadrons").Fill("Bhadrons",1)
                self.gethist("Bpt").Fill(BHdrnPt)
                self.gethist("Beta").Fill(BHdrnEta)
                nBHadrons +=1
                if(isSemiElVec.at(i)==1):
                    self.gethist("Bhadrons").Fill("Bsemi",1)


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

    '''# Soft Electrons in PtCut,ecatcut bins'''
    def getCElMultplcty(self,ptcut,etacut):
        CPtVec      = self.getCurrentValue("CPt")
        CEtaVec     = self.getCurrentValue("CEta")
        isSemiElVec = self.getCurrentValue("CisSemiElectron")

        nCEl = 0
        for i in xrange(CPtVec.size()):
            CHdrnPt = CPtVec.at(i)
            CHdrnEta= CEtaVec.at(i)
            SemiEl  = isSemiElVec.at(i)

            if(CHdrnPt > ptcut and abs(CHdrnEta) < etacut):
                if(SemiEl ==1):
                    nCEl +=1

        return nCEl


    def fillCKinematics(self,ptcut,etacut):
        CPtVec      = self.getCurrentValue("CPt")
        CEtaVec     = self.getCurrentValue("CEta")
        isSemiElVec = self.getCurrentValue("CisSemiElectron")
        BBCVec      = self.getCurrentValue("BBC")
        CParentBCVec= self.getCurrentValue("CParentBC")
        
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

    '''TrkParticle'''
    def trkAna(self):
        trketaVec   = self.getCurrentValue("el_trk_Eta")
        trkptVec    = self.getCurrentValue("el_trk_Pt")
        trkphiVec   = self.getCurrentValue("el_trk_Phi")
        softeVec    = self.getCurrentValue("elAuthorSofte")
        isMtchdVec  = self.getCurrentValue("elIsMtchd")




        for i in xrange(trketaVec.size()):
            trkEta  = trketaVec.at(i)
            trkPhi  = trkphiVec.at(i)
            trkPt   = trkptVec.at(i)

            softe   = softeVec.at(i)

            IsMtchd = isMtchdVec.at(i)

            if(abs(trkEta) < 2.47 and trkEta != -100 and softe ==1):
                self.gethist("trkEta").Fill(trkEta)
                self.gethist("trkPt").Fill(trkPt)
                self.gethist("trkPhi").Fill(trkPhi)
                if(IsMtchd==1):
                    self.gethist("trkEtaMtchd").Fill(trkEta)
                    self.gethist("trkPtMtchd").Fill(trkPt)
                    self.gethist("trkPhiMtchd").Fill(trkPhi)

    '''Calo cluster'''
    def clAna(self):
        cletaVec    = self.getCurrentValue("el_cl_Eta")
        clptVec     = self.getCurrentValue("el_cl_Pt")
        clphiVec    = self.getCurrentValue("el_cl_Phi")

        softeVec    = self.getCurrentValue("elAuthorSofte")
        authorVec   = self.getCurrentValue("elAuthor")

        isMtchdVec  = self.getCurrentValue("elIsMtchd")


        for i in xrange(cletaVec.size()):
            clEta   = cletaVec.at(i)
            clPhi   = clphiVec.at(i)
            clPt    = clptVec.at(i)
            IsMtchd = isMtchdVec.at(i)

            author  = authorVec.at(i)
            softe   = softeVec.at(i)
            
            if(abs(clEta) < 2.47 and clEta !=-100 and
                    not(1.37 < abs(clEta) and abs(clEta)<1.52) and
                    (author ==1 and softe==1)):
                self.gethist("clEta").Fill(clEta)
                self.gethist("clPhi").Fill(clPhi)
                self.gethist("clPt").Fill(clPt)

                if(IsMtchd==1):
                    self.gethist("clEtaMtchd").Fill(clEta)
                    self.gethist("clPhiMtchd").Fill(clPhi)
                    self.gethist("clPtMtchd").Fill(clPt)

    '''truth Analysis'''                
    def trthAna(self):
        cletaVec    = self.getCurrentValue("el_cl_Eta")
        clptVec     = self.getCurrentValue("el_cl_Pt")
        
        trthetaVec  = self.getCurrentValue("el_truth_Eta")
        trthptVec   = self.getCurrentValue("el_truth_Pt")
        trthphiVec  = self.getCurrentValue("el_truth_Phi")

        isMtchdVec  = self.getCurrentValue("elIsMtchd")

        softeVec    = self.getCurrentValue("elAuthorSofte")
        authorVec   = self.getCurrentValue("elAuthor")

        parentPDg   = self.getCurrentValue("mtchdParent")


        nZEl    =   0
        nBEl    =   0
        nCEl    =   0
        nOther  =   0

        for i in xrange(cletaVec.size()):
            trthEta = trthetaVec.at(i)
            trthPhi = trthphiVec.at(i)
            trthPt  = trthptVec.at(i)

            IsMtchd = isMtchdVec.at(i)
            author  = authorVec.at(i)
            softe   = softeVec.at(i)

            parent  = parentPDg.at(i)

            clEta   = cletaVec.at(i)
            clPt    = clptVec.at(i)


            #TruthMatched Electrons
            if( IsMtchd ==1 and abs(clEta)<2.47):

                self.gethist("elParent").Fill(str(parent),1)
                if(abs(parent) ==23 and author ==1 and clPt >20 and (abs(clEta) < 2.47)):
                    nZEl    += 1
                elif(self.isBHadron(parent) and softe==1 and trthEta< 2.47 and trthPt>2):
                    nBEl    +=1
                elif(self.isCHadron(parent) and softe==1 and trthEta< 2.47 and trthPt>2):
                    nCEl    +=1
                else:
                    nOther  +=1

        self.gethist("matchedZElVsBEl").Fill(nZEl,nBEl)
        self.gethist("matchedZElVsCEl").Fill(nZEl,nCEl)

    ''''''''''''''''''''''''''''''''
    '''Function to add histograms'''
    ''''''''''''''''''''''''''''''''
    def addPtHist(self,name):
        self.addh1(name,";[GeV]",500,0,500)


    def addEtaHist(self,name):
        self.addh1(name,";#eta",100,-5,5)

    def addPhiHist(self,name):
        self.addh1(name,"",100,-4,4)


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

    '''checks eta isolation cuts'''
    def isGoodEta(eta):
        isGood = False

        if(abs(eta) < self.barrelEtacut and not (abs(eta)<self.cracketa1  and abs(eta) <self.cracketa2)):
            isGood = True

        return isGood

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

        '''Write both histogram lists to a file'''
        f = ROOT.TFile("procHist.root","RECREATE")
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

    def __addHist(self,hist):
        self.outHistList += [hist]


    ''' Create functions to make the final histograms here.
    Functions with '__' will not be executed while creating
    final histograms'''

    def MakeHistBSemiElectron(self):
        '''Retrieve the THnSparse'''
        BSparse         = self.__getInHist("BSemiElectron")

        #B Multiplicity as a function of B-hadron pt cut
        h_mulVsPtCut    = BSparse.Projection(2,0)
        h_mulVsPtCut.GetXaxis().SetTitle("pt cut")
        h_mulVsPtCut.GetYaxis().SetTitle("# B-hadrons")
        h_mulVsPtCut.Draw("colztext")
        ROOT.gPad.SetLogz()
        ROOT.gPad.Print(self.pdfPath+"/BMultVsPtcut.pdf","Landscapepdf")
        self.__addHist(h_mulVsPtCut)

        #BSemiElectron multiplicyt as a function of B-hadron pt cut
        h_BElVsPtCut    = BSparse.Projection(1,0)
        h_BElVsPtCut.GetXaxis().SetTitle("pt cut")
        h_BElVsPtCut.GetYaxis().SetTitle("# B-el")
        h_BElVsPtCut.Draw("colztext")
        ROOT.gPad.SetLogz()
        ROOT.gPad.Print(self.pdfPath+"/BElVsPtCut.pdf","Landscapepdf")
        self.__addHist(h_BElVsPtCut)

        #Multiplicyt of B Electrons as a function of B- multiplicity
        h_BElVsBMul     = BSparse.Projection(1,2)
        h_BElVsBMul.GetXaxis().SetTitle("nBHadrons")
        h_BElVsBMul.GetYaxis().SetRangeUser(0,8)
        h_BElVsBMul.GetYaxis().SetTitle("# B-el")
        h_BElVsBMul.Draw("colztext")
        ROOT.gPad.SetLogz()
        ROOT.gPad.Print(self.pdfPath+"/BElVsBMul.pdf","Landscapepdf")
        self.__addHist(h_BElVsBMul)

        #Multiplicity of B-hadrons
        h_BMult         = BSparse.Projection(2)
        h_BMult.GetXaxis().SetTitle("# B-hadrons")
        h_BMult.Draw()
        ROOT.gPad.SetLogy()
        ROOT.gPad.Print(self.pdfPath+"/BMult.pdf","Landscapepdf")
        self.__addHist(h_BMult)

        #B Semi electron events as a function of B ptcut and B multiplicity
        h_BElEvents     = self.__getInHist("BElEvents")
        h_BElEvents.Draw("colztext")
        h_BElEvents.GetXaxis().SetTitle(" pt cut")
        h_BElEvents.GetYaxis().SetTitle("# B-hadrons")
        ROOT.gPad.SetLogy(0)
        ROOT.gPad.SetLogz(0)
        ROOT.gPad.Print(self.pdfPath+"/BElEvents_ptcut_BMul.pdf","Landscapepdf")
        self.__addHist(h_BElEvents)

    def MakeHistCSemiElectron(self):
        '''Retrieve THnSparse'''
        CSparse         = self.__getInHist("CSemiElectron")
        h_mulVsPtCut    = CSparse.Projection(2,0)
        h_mulVsPtCut.GetXaxis().SetTitle("pt cut")
        h_mulVsPtCut.GetYaxis().SetTitle("# C-hadrons")
        h_mulVsPtCut.Draw("colztext")
        ROOT.gPad.SetLogz()
        ROOT.gPad.SetLogy(0)
        ROOT.gPad.Print(self.pdfPath+"/CMultVsPtcut.pdf","Landscapepdf")
        self.__addHist(h_mulVsPtCut)

        h_CElVsPtCut    = CSparse.Projection(1,0)
        h_CElVsPtCut.GetXaxis().SetTitle("pt cut")
        h_CElVsPtCut.GetYaxis().SetTitle("# C-el")
        h_CElVsPtCut.Draw("colztext")
        ROOT.gPad.SetLogz()
        ROOT.gPad.Print(self.pdfPath+"/CElVsPtCut.pdf","Landscapepdf")
        self.__addHist(h_CElVsPtCut)

        h_CElVsCMul     = CSparse.Projection(1,2)
        h_CElVsCMul.GetXaxis().SetTitle("# C-hadrons")
        h_CElVsCMul.GetYaxis().SetTitle("# C-el")
        h_CElVsCMul.GetYaxis().SetRangeUser(0,8)
        h_CElVsCMul.Draw("colztext")
        ROOT.gPad.SetLogz()
        ROOT.gPad.Print(self.pdfPath+"/CElVsCMul.pdf","Landscapepdf")
        self.__addHist(h_CElVsCMul)

        #Multiplicity of C-hadrons
        h_CMult         = CSparse.Projection(2)
        h_CMult.GetXaxis().SetTitle("# C-hadrons")
        h_CMult.Draw()
        ROOT.gPad.SetLogy()
        ROOT.gPad.Print(self.pdfPath+"/CMult.pdf","Landscapepdf")
        self.__addHist(h_CMult)

#End of class plotscript
