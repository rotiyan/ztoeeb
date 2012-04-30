import abc
from PlotBase import PlotBase

class MyPlotter(PlotBase):
    def __init__(self,tree,hists = {}):
        self.hists  = hists
        self.myTree   = tree
    
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
        self.addEtaHist("trkEtaMtchd")
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
        self.addh1("Chadrons")
        self.addh1("BMultiplcty","",10,0,10)
        self.addh1("CMultiplcty","",10,0,10)

        self.addh2("matchedZElVsBEl",";# ZEl; #BEl",10,0,10,10,0,10)
        self.addh2("matchedZElVsCEl",";# ZEl; #BEl",10,0,10,10,0,10)
        self.addh2("hardVsSoftEl","",10,0,10,10,0,10)

    def execute(self):
        trketaVec   = self.getCurrentValue("el_trk_Eta")
        trkptVec    = self.getCurrentValue("el_trk_Pt")
        trkphiVec   = self.getCurrentValue("el_trk_Phi")

        cletaVec    = self.getCurrentValue("el_cl_Eta")
        clptVec     = self.getCurrentValue("el_cl_Pt")
        clphiVec    = self.getCurrentValue("el_cl_Phi")

        trthetaVec  = self.getCurrentValue("el_truth_Eta")
        trthptVec   = self.getCurrentValue("el_truth_Pt")
        trthphiVec  = self.getCurrentValue("el_truth_Phi")

        isMtchdVec  = self.getCurrentValue("elIsMtchd")
        softeVec    = self.getCurrentValue("elAuthorSofte")
        authorVec   = self.getCurrentValue("elAuthor")

        parentPDg   = self.getCurrentValue("mtchdParent")

        #Truth
        BPDGVec         = self.getCurrentValue("BPDG")
        BPtVec          = self.getCurrentValue("BPt")
        BEtaVec         = self.getCurrentValue("BEta")
        BPhiVec         = self.getCurrentValue("BPhi")
        BIsSemiLeptVec  = self.getCurrentValue("BisSemiElectron")

        CPDGVec         = self.getCurrentValue("CPDG")
        CPtVec          = self.getCurrentValue("CPt")
        CEtaVec         = self.getCurrentValue("CEta")
        CPhiVec         = self.getCurrentValue("CPhi")
        CIsSemiLeptVec  = self.getCurrentValue("CisSemiElectron")


        nZEl    = 0
        nBEl    = 0
        nCEl    = 0
        nOther  = 0
        
        #All Vectors of the same size as the electron container
        for i in range(cletaVec.size()):
            trkEta  = trketaVec.at(i)
            trkPhi  = trkphiVec.at(i)
            trkPt   = trkptVec.at(i)
            
            clEta   = cletaVec.at(i)
            clPhi   = clphiVec.at(i)
            clPt    = clptVec.at(i)
            
            trthEta = trthetaVec.at(i)
            trthPhi = trthphiVec.at(i)
            trthPt  = trthptVec.at(i)

            IsMtchd = isMtchdVec.at(i)
            author  = authorVec.at(i)
            softe   = softeVec.at(i)

            parent  = parentPDg.at(i)

            if(abs(clEta) < 2.47 and clEta !=-100 and (author ==1 and softe==1)):
                self.gethist("clEta").Fill(clEta)
                self.gethist("clPhi").Fill(clPhi)
                self.gethist("clPt").Fill(clPt)
                if(IsMtchd==1):
                    self.gethist("clEtaMtchd").Fill(clEta)
                    self.gethist("clPhiMtchd").Fill(clPhi)
                    self.gethist("clPtMtchd").Fill(clPt)

            if(abs(trkEta) < 2.47 and trkEta != -100 and softe ==1):
                self.gethist("trkEta").Fill(trkEta)
                self.gethist("trkPt").Fill(trkPt)
                self.gethist("trkPhi").Fill(trkPhi)
                if(IsMtchd==1):
                    self.gethist("trkEtaMtchd").Fill(trkEta)
                    self.gethist("trkPtMtchd").Fill(trkPt)
                    self.gethist("trkPhiMtchd").Fill(trkPhi)

            #TruthMatched Electrons
            if( IsMtchd ==1):

                self.gethist("elParent").Fill(str(parent),1)
                if(abs(parent) ==23 and author ==1 and clPt >20 and (abs(clEta) < 2.47)):
                    nZEl    += 1
                elif(self.isBHadron(parent) and softe==1 and trkEta < 2.47 and trkPt >2):
                    nBEl    +=1
                elif(self.isCHadron(parent) and softe==1 and trkEta < 2.47 and trkPt >2):
                    nCEl    +=1
                else:
                    nOther  +=1

        self.gethist("matchedZElVsBEl").Fill(nZEl,nBEl)
        self.gethist("matchedZElVsCEl").Fill(nZEl,nCEl)\

        #Truth
        self.CalcBMultplcty(20,2.5)
        self.CalcCMultplcty(20,2.5)



    def finalize(self):
        '''Efficiency histograms'''
        self.gethist("trkMtchEffVsPt").Divide(self.gethist("trkPtMtchd"), self.gethist("trkPt"))
        self.gethist("trkMtchEffVsEta").Divide(self.gethist("trkEtaMtchd"), self.gethist("trkEta"))
        self.gethist("trkMtchEffVsPhi").Divide(self.gethist("trkPhiMtchd"), self.gethist("trkPhi"))

        self.gethist("clMtchEffVsPt").Divide(self.gethist("clPtMtchd"), self.gethist("clPt"))
        self.gethist("clMtchEffVsEta").Divide(self.gethist("clEtaMtchd"), self.gethist("clEta"))
        self.gethist("clMtchEffVsPhi").Divide(self.gethist("clPhiMtchd"), self.gethist("clPhi"))

        #Save histograms to disk
        histlist = self.gethistList()
        f = ROOT.TFile("outputHist.root","RECREATE")
        for h in histlist:
            h.Write()
        f.Close()

    def CalcBMultplcty(self,ptcut,etacut):
        BPtVec      = self.getCurrentValue("BPt")
        BEtaVec     = self.getCurrentValue("BEta")
        isSemiElVec = self.getCurrentValue("BisSemiElectron")
        
        nBHadrons = 0
        for i in range(BPtVec.size()):
            BHdrnPt = BPtVec.at(i)
            BHdrnEta= BEtaVec.at(i)

            if(BHdrnPt >ptcut and abs(BHdrnEta) < etacut):
                self.gethist("Bhadrons").Fill("Bhadrons",1)
                nBHadrons +=1
                if(isSemiElVec.at(i)==1):
                    self.gethist("Bhadrons").Fill("Bsemi",1)

        self.gethist("BMultiplcty").Fill(nBHadrons)

    def CalcCMultplcty(self,ptcut,etacut):
        CPtVec      = self.getCurrentValue("CPt")
        CEtaVec     = self.getCurrentValue("CEta")
        isSemiElVec = self.getCurrentValue("CisSemiElectron")
        
        nCHadrons = 0
        for i in range(CPtVec.size()):
            CHdrnPt = CPtVec.at(i)
            CHdrnEta= CEtaVec.at(i)

            if(CHdrnPt >ptcut and abs(CHdrnEta) < etacut):
                self.gethist("Chadrons").Fill("Chadrons",1)
                nCHadrons +=1
                if(isSemiElVec.at(i)==1):
                    self.gethist("Chadrons").Fill("Csemi",1)

        self.gethist("CMultiplcty").Fill(nCHadrons)



    def addPtHist(self,name):
        self.addh1(name,";[GeV]",500,0,500)


    def addEtaHist(self,name):
        self.addh1(name,";#eta",100,-5,5)

    def addPhiHist(self,name):
        self.addh1(name,"",100,-4,4)

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



import ROOT 
f = ROOT.TFile("../share/softElectron.root")
t = f.Get("el")

plotter = MyPlotter(t)
plotter.run()
