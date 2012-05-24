import ROOT
import os

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
        BSparse = self.__getInHist("BSemiElectron")
        h_mulVsPtCut = BSparse.Projection(1,0)
        h_mulVsPtCut.GetXaxis().SetTitle("pt cut")
        h_mulVsPtCut.GetYaxis().SetTitle("multiplicity")
        h_mulVsPtCut.Draw("colz")
        print type(self.pdfPath)
        ROOT.gPad.Print(self.pdfPath+"/BSemiElectron_MultVsPtCut.pdf","Landscapepdf")

#End of class plotscript
