import abc
from PlotBase import PlotBase

import ROOT, sys,os,glob 
from multiprocessing import Process,Queue,current_process


class MyPlotter(PlotBase):
    def __init__(self,tree,hists = {}):
        self.hists  = hists
        self.myTree   = tree
        self.barrelEtacut   = 2.47
        self.cracketa1      = 1.37
        self.cracketa2      = 1.52
    
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
        self.addh3("BMultiplcty","BMultiplcty;ptcut;etacut;nBHadrons",50,0,50,10,0,10,10,0,10)
        self.addh1("Chadrons")
        self.addh1("Cpt","",500,0,500)
        self.addh1("Ceta","",100,-5,5)
        self.addh3("CMultiplcty","CMultiplcty;ptcut;etacut;nCHadrons",50,0,50,10,0,10,10,0,10)

        self.addh2("matchedZElVsBEl",";# ZEl; #BEl",10,0,10,10,0,10)
        self.addh2("matchedZElVsCEl",";# ZEl; #BEl",10,0,10,10,0,10)
        self.addh2("hardVsSoftEl","",10,0,10,10,0,10)

        self.addh3("BbdeltaRMin",";ptcut;etacut;#Delta R min",50,0,50,10,0,10,500,0,5)
        self.addh3("BbdeltaRMax",";ptcut;etacut;#Delta R max",50,0,50,10,0,10,500,0,5)

        self.addh3("CbdeltaRMin",";ptcut;etacut;#Delta R min",50,0,50,10,0,10,500,0,5)
        self.addh3("CbdeltaRMax",";ptcut;etacut;#Delta R max",50,0,50,10,0,10,500,0,5)

        self.addh3("CascadedRMin",";ptcut;etacut;#Delta R min",50,0,50,10,0,10,500,0,5)
        self.addh3("CascadedRMax",";ptcut;etacut;#Delta R min",50,0,50,10,0,10,500,0,5)

        '''bquark correlation'''
        self.addh2("bquarkPtCorrel",";b1;b2",200,0,200,200,0,200)
        self.addh2("bquarkEtaCorrel",";b1;b2",100,-5,5,100,-5,5)
        self.addh2("bquarkPhiCorrel",";b1;b2",100,-5,5,100,-5,5)

        self.addh1("nbQuark","",10,0,10)


    def execute(self):
        #Electron cluster analysis
        self.clAna()
        
        #Electron track particle analaysis
        self.trkAna()
        
        #Electron truth analysis
        self.trthAna()

        self.fillBKinematics(5,100)
        self.fillCKinematics(5,100)

        self.makeDeltaRPlots()


    def finalize(self):
        '''Efficiency histograms'''
        self.gethist("trkMtchEffVsPt").Divide(self.gethist("trkPtMtchd"), self.gethist("trkPt"))
        self.gethist("trkMtchEffVsEta").Divide(self.gethist("trkEtaMtchd"), self.gethist("trkEta"))
        self.gethist("trkMtchEffVsPhi").Divide(self.gethist("trkPhiMtchd"), self.gethist("trkPhi"))

        self.gethist("clMtchEffVsPt").Divide(self.gethist("clPtMtchd"), self.gethist("clPt"))
        self.gethist("clMtchEffVsEta").Divide(self.gethist("clEtaMtchd"), self.gethist("clEta"))
        self.gethist("clMtchEffVsPhi").Divide(self.gethist("clPhiMtchd"), self.gethist("clPhi"))

        '''Compare B-hadron b-quark delta r'''
        self.compareBbDeltaR(5,2.5)
        self.compareBbDeltaR(10,2.5)
        self.compareBbDeltaR(20,2.5)

        '''Compare C-hadron b-quark delta r'''
        self.compareCbDeltaR(5,2.5)
        self.compareCbDeltaR(10,2.5)
        self.compareCbDeltaR(20,2.5)


        #Save histograms to disk
        histlist = self.gethistList()
        fname = "outputHist"+current_process().name+"._root"
        f = ROOT.TFile(fname,"RECREATE")
        for h in histlist:
            h.Write()
        f.Close()

    ''''''''''''''''''''''''''''''''''''''''''''
    ''''Functions to be called inside execute'''
    ''''''''''''''''''''''''''''''''''''''''''''

    def makeDeltaRPlots(self):
        ptcutlist   = [2,5,10,15,20]
        etacutlist  = [2.5,5]

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
            self.gethist("bquarkPtCorrel").Fill(bQuarkME_pt[0],bQuarkME_pt[1])
            self.gethist("bquarkEtaCorrel").Fill(bQuarkME_eta[0],bQuarkME_eta[1])
            self.gethist("bquarkPhiCorrel").Fill(bQuarkME_phi[0], bQuarkME_phi[1])

            self.gethist("nbQuark").Fill(len(bQuarkME_pt))

            for pt in ptcutlist:
                for eta in etacutlist:
                    self.gethist("BMultiplcty").Fill(pt,eta,self.getBMultplcty(pt,eta))
                    self.gethist("CMultiplcty").Fill(pt,eta,self.getCMultplcty(pt,eta))

                    nBhadrons   = self.getBMultplcty(pt,eta)
                    nChadrons   = self.getCMultplcty(pt,eta)
                    
                    if(nBhadrons ==1):
                        drlist  = [ self.deltaR(eta1,phi1,eta2,phi2) for eta1 in BEtaVec for phi1 in BPhiVec for eta2 in bQuarkME_eta for phi2 in bQuarkME_phi]
                        self.gethist("BbdeltaRMin").Fill(pt,eta,min(drlist))
                        self.gethist("BbdeltaRMax").Fill(pt,eta,max(drlist))

                        if(nChadrons ==1):
                            drlist  = [ self.deltaR(eta1,phi1,eta2,phi2) for eta1 in CEtaVec for phi1 in CPhiVec for eta2 in bQuarkME_eta for phi2 in bQuarkME_phi]
                            self.gethist("CbdeltaRMin").Fill(pt,eta,min(drlist))
                            self.gethist("CbdeltaRMax").Fill(pt,eta,max(drlist))

                            for bbc in BBC:
                                for cpbc in CparentBC:
                                    if(bbc == cpbc):
                                        self.gethist("CascadedRMin").Fill(pt,eta,min(drlist))
                                        self.gethist("CascadedRMax").Fill(pt,eta,max(drlist))


    def getBMultplcty(self,ptcut,etacut):
        BPtVec      = self.getCurrentValue("BPt")
        BEtaVec     = self.getCurrentValue("BEta")
        BPhiVec     = self.getCurrentValue("BPhi")

        isSemiElVec = self.getCurrentValue("BisSemiElectron")
        
        nBHadrons = 0
        for i in range(BPtVec.size()):
            BHdrnPt = BPtVec.at(i)
            BHdrnEta= BEtaVec.at(i)

            if(BHdrnPt >ptcut and abs(BHdrnEta) < etacut):
                nBHadrons +=1

        return nBHadrons


    def fillBKinematics(self,ptcut,etacut):
        BPtVec      = self.getCurrentValue("BPt")
        BEtaVec     = self.getCurrentValue("BEta")
        BPhiVec     = self.getCurrentValue("BPhi")

        isSemiElVec = self.getCurrentValue("BisSemiElectron")
        
        nBHadrons = 0
        for i in range(BPtVec.size()):
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
        for i in range(CPtVec.size()):
            CHdrnPt = CPtVec.at(i)
            CHdrnEta= CEtaVec.at(i)

            if(CHdrnPt >ptcut and abs(CHdrnEta) < etacut):
                nCHadrons +=1

        return nCHadrons


    def fillCKinematics(self,ptcut,etacut):
        CPtVec      = self.getCurrentValue("CPt")
        CEtaVec     = self.getCurrentValue("CEta")
        isSemiElVec = self.getCurrentValue("CisSemiElectron")
        
        nCHadrons = 0
        for i in range(CPtVec.size()):
            CHdrnPt = CPtVec.at(i)
            CHdrnEta= CEtaVec.at(i)

            if(CHdrnPt >ptcut and abs(CHdrnEta) < etacut):
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




        for i in range(trketaVec.size()):
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


        for i in range(cletaVec.size()):
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

        for i in range(cletaVec.size()):
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

    '''Do Multiprocessing'''


#end of plotter class



flist       = glob.glob(str(sys.argv[1])+"/*root")

def MultiProc(flist):
    processes   = []
    doneQue = Queue()
    try: 
        for fname in flist:
            f = ROOT.TFile(fname)
            t = f.Get("el")
            plotter = MyPlotter(t)
            p = Process(target=plotter.run)
            p.start()
            processes.append(p)
        for p in processes:
            p.join()

        os.system("hadd -f outputHist.root *._root")
        os.system("rm *._root")
    except:
        print "something wrong"

MultiProc(flist)
