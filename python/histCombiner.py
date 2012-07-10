import ROOT
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

        self.mergeDS("zbb","zbb")
        self.mergeDS("zcc","zcc")
        self.mergeDS("znp","zinc")
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

    def makeStack(self,histname,ymax=1e6,logy=1):
        mcHistTuple  = [(name,file.Get(histname)) for name,file in self.mergeDict.iteritems() if name !='data']
        mcHistTuple.sort(key=lambda x:x[1].Integral())

        dataHist    = self.mergeDict['data'].Get(histname)
        dataHist.SetMarkerStyle(1)

        hs = ROOT.THStack(histname,histname+"Stack")
        leg = ROOT.TLegend(0.6,0.6,0.85,0.85)
        leg.SetFillStyle(0)
        leg.SetLineColor(0)
        
        iColor = 2
        for ob in mcHistTuple:
            hist = ob[1].Clone()
            
            hist.SetLineColor(iColor)
            hist.SetFillColor(iColor)

            hist.SetMarkerStyle(0)
            
            hs.Add(hist)
            leg.AddEntry(hist,ob[0],"F")

            iColor +=1

        leg.AddEntry(dataHist,'data')
        hs.Draw()
        hs.SetMaximum(ymax)
        hs.SetMinimum(1)


   
        hs.GetHistogram().GetXaxis().SetTitle(dataHist.GetXaxis().GetTitle())
 
        leg.Draw("same")
        dataHist.Draw("same")

        ROOT.gPad.SetLogy(logy)
        ROOT.gPad.Print(histname+".pdf","Landscapepdf")


myHC    = histCombiner("procHist.root")
print myHC.getHistList()

myHC.makeStack("ElAuthorMultplcty",1e7)
