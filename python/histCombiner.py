import ROOT
ROOT.TH1.AddDirectory(False)
import os,sys


''' Class to combine the MC and data histograms.
    The program is expected be run from a folder which contains
    both data and background histograms
    
    The progaram would look for TFiles of the name procHist.root,
    extract respective histograms and  associate the directory name 
    as the legend of the histogram

    the data histogram files should be stored in a directory named 'data'    
    '''
    

class histCombiner():
    def __init__(self,rootFileName="procHist.root"):
        self.fileDict   = dict([(dir,ROOT.TFile(dir+"/"+rootFileName)) for dir in os.listdir(".") if os.path.isdir(dir)])
        self.mergeDict  = {}

        self.mergeDS("zbb","zeebb")
        self.mergeDS("zcc","zeecc")
        self.mergeDS("znp","zeeinc")
        self.mergeDS("ztautaubb","ztautaubb")
        self.mergeDS("ztautaunp","ztautauinc")
        self.mergeDS("wtaununp","wtaunu")
        self.mergeDS("wenunp","wenunp")
        self.mergeDS("ttbar","ttbar")
        self.mergeDS("singleTop","singleTop")
        self.mergeDS("data","data")

    def getHistDict(self):
        return self.fileDict

    def getHistList(self):
        return self.mergeDict['data'].ls()

    def mergeDS(self,inputTag,outputTag):
        fNameList = [file.GetName() for name, file in self.fileDict.iteritems() if name.startswith(inputTag) ]
        
        if(len(fNameList)>0):
            if( not os.path.exists(outputTag+".root")):
                os.system("hadd -f "+outputTag+".root "+ " ".join(fNameList))
            else:
                print "File",outputTag, "already exists"
                
            self.mergeDict[outputTag]   = ROOT.TFile(outputTag+".root")

        else:
            print "--------------------",outputTag,'DOES NOT EXIST----------------'



        '''
        fMerger = ROOT.TFileMerger()
        [fMerger.AddFile(file.GetName()) for name, file in self.fileDict.iteritems() if name.startswith(inputTag)]
        fMerger.OutputFile(outputTag+".root")
        fMerger.Merge()

        self.mergeDict[outputTag]   = ROOT.TFile(outputTag+".root")
        print self.mergeDict[outputTag].GetNbytesInfo(), self.mergeDict[outputTag].GetName()

        fMerger.Clear()
        '''

    def makeStack(self,histname,ratio=0,ymax=1e7,logy=1):
        mcHistTuple  = [(name,file.Get(histname)) for name,file in self.mergeDict.iteritems() if name !='data' and file.Get(histname)]

        iColor = 3
        for i in xrange(len(mcHistTuple)):
            if(iColor ==10):
                iColor +=1

            mcHistTuple[i][1].SetFillColor(iColor)
            mcHistTuple[i][1].SetLineStyle(0)
            mcHistTuple[i][1].SetLineColor(iColor)
            mcHistTuple[i][1].SetFillColor(iColor)
            #mcHistTuple[i][1].SetFillStyle(3004)
            mcHistTuple[i][1].SetMarkerStyle(0)

            iColor +=1

        if(len(mcHistTuple)==0):
            print histname,"not found"
        else:
            mcHistTuple.sort(key=lambda x:x[1].Integral())

            dataHist    = self.mergeDict['data'].Get(histname)

            hs = ROOT.THStack(histname,histname+"Stack")
            ROOT.SetOwnership(hs,False)
            leg = ROOT.TLegend(0.6,0.5,0.95,0.9)
            ROOT.SetOwnership(leg,False)
            leg.SetFillStyle(0)
            leg.SetLineColor(0)

            legEntryList = []
            
            for ob in mcHistTuple:
                hist = ob[1].Clone()
                
                hs.Add(hist)
                legEntryList +=[(hist,ob[0])]

            """Legend"""
            if(dataHist.GetRMS() !=0):
                leg.AddEntry(dataHist,'data')

            legEntryList.reverse()
            for entry in legEntryList:
                leg.AddEntry(entry[0],entry[1])

            '''if requested to make ratio plot'''
            if(ratio):
                c = ROOT.TCanvas("c1","Canvas",600,848)
                pad1    = ROOT.TPad("pad1","pad1",0,0.3,1,1)
                pad2    = ROOT.TPad("pad2","pad2",0,0,1,0.3)
                pad1.SetBottomMargin(0)
                pad2.SetTopMargin(0)

                pad1.cd()
                hs.Draw("HIST")
                hs.SetMaximum(ymax)
                hs.SetMinimum(10)

                leg.Draw('same')
                if(dataHist.GetRMS() !=0):
                    dataHist.Draw('same')
                    #hs.GetHistogram().GetXaxis().SetTitle(dataHist.GetXaxis().GetTitle())

                pad2.cd()
                sumHist = reduce(lambda x,y: x + y,mcHistTuple)

                h0 = mcHistTuple[0][1].Clone()
                for i in xrange(1,len(mcHistTuple)):
                    h0.Add(mcHistTuple[i][1])

                d = dataHist.Clone()
                d.Divide(h0)
                d.GetXaxis().SetLabelFont(63)
                d.GetXaxis().SetTitleSize(0.08)
                d.GetXaxis().SetLabelSize(16)
                d.GetYaxis().SetLabelFont(63)
                d.GetYaxis().SetLabelSize(16)
                
                d.SetMinimum(0)
                d.SetMaximum(3)
                d.GetYaxis().SetTitle("data/MC")
                d.Draw()

                c.cd()
                pad1.SetLogy(logy)
                pad1.Draw()
                pad2.Draw()

                ROOT.gPad.Print(histname+".ps","Portrait")
                ROOT.gPad.Update()

            else:
                ROOT.gStyle.SetMarkerSize(1.5)
                hs.Draw("HISTTEXT")
                hs.SetMaximum(ymax)
                hs.SetMinimum(10)

                leg.Draw("same")
                if(dataHist.GetRMS() !=0):
                    dataHist.Draw("same")
                    hs.GetHistogram().GetXaxis().SetTitle(dataHist.GetXaxis().GetTitle())

                ROOT.gPad.SetLogy(logy)
                ROOT.gPad.Print(histname+".ps","Landscape")
                ROOT.gPad.Update()

    def makeDataHists(self):
        self.makeStack("ElMultplcty",1,1e9)
        self.makeStack("ElPt",1)
        self.makeStack("ElEta",1)
        self.makeStack("ElPhi",1)
        self.makeStack("ZBosonMass",1)
        self.makeStack("BosonCandidateEplusPt",1)
        self.makeStack("BosonCandidateEMinusPt",1)

        self.makeStack("SlctdElMultplcty",1,1e9)
        self.makeStack("SlctdElPt",1)
        self.makeStack("SlctdElEta",1)
        self.makeStack("SlctdElPhi",1)

    def makeMCHists(self):
        self.makeStack("BElMult_5")
        self.makeStack("BMult_5")
        self.makeStack("BElMult_10")
        self.makeStack("BMult_10")
        self.makeStack("BElMult_15")
        self.makeStack("BMult_15")
        self.makeStack("BElMult_20")
        self.makeStack("BMult_20")
        self.makeStack("BElMult_25")
        self.makeStack("BMult_25")
        self.makeStack("BElMult_30")
        self.makeStack("BMult_30")



myHC    = histCombiner("procHist.root")
myHC.makeDataHists()
#myHC.makeMCHists()
