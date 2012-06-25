import abc
import ROOT
from array import array 

class NtupleAnaBase(object):
    __metaclass__   = abc.ABCMeta
    def __init__(self,hists,tree,event=0,histfile=0):
        self.hists  = hists
        self.myTree = tree
        self.event  = event

        self.histFile   = histfile

    '''Abstract method to be used by the user 
    to addhistograms and add tree'''
    @abc.abstractmethod
    def initialize(self):
        return

    '''Abstract method to be used by the user to fill 
    the histograms'''
    @abc.abstractmethod
    def execute(self):
        return

    '''The histograms must be saved 
    to disk in this function'''
    @abc.abstractmethod
    def finalize(self):
        return

    def run(self):
        '''program flow'''
        self.initialize()

        for hist in self.gethistList():
            hist.Sumw2()

        print "Total Ntuple Entries: ", self.myTree.GetEntriesFast()
        counter = 0

        for event  in self.myTree:
            counter += 1
            if(not (counter % 1000)):
                print "Event: ",counter,"/",self.myTree.GetEntriesFast()
            self.event = event
            self.execute()
        
        self.finalize()

    '''returns the current value of the branch'''
    def getCurrentValue(self,mytype):
        event = self.event
        try:
            return getattr(event,mytype)
        except:
            print "Branch ", mytype, "does not exist in", self.myTree.GetName()

    def gethist(self,name):
        try: 
            return self.hists[name]
        except:
            print name," not found" 

    def gethistList(self):
        return self.hists.values()

    def addtohistList(self,h):
        if("TH1" in str(type(h))):
            self.hists[h.GetName()] = h
        else:
            print "object ",type(h)," is not a TH1 type"

    def removeFromhisList(self,h):
        if("TH1" in str(type(h))):
            try:
                del self.hists[h.GetName()]
            except:
                print h.GetName()," was not found in the list of histograms"
        else:
            print "object ",type(h)," is not a TH1 type"

    '''Add histograms to the private list'''
    def regTH1(self,name,title="",nbins=1,low=0,high=1):
        self.hists[name]    = ROOT.TH1F(name,title,nbins,low,high)

    def regTH2(self,name,title="",nxbins=1,xlow=0,xhigh=1,nybins=1,ylow=0,yhigh=1):
        self.hists[name]    = ROOT.TH2F(name,title,nxbins,xlow,xhigh,nybins,ylow,yhigh)

    def regTH3(self,name,title="",nxbins=1,xlow=0,xhigh=1,nybins=1,ylow=0,yhigh=1,nzbins=1,zlow=0,zhigh=1):
        self.hists[name]    = ROOT.TH3F(name,title,nxbins,xlow,xhigh,nybins,ylow,yhigh,nzbins,zlow,zhigh)

    def regProfHist(self,name,title="",nxbins=1,xlow=0,xhigh=1):
        self.hists[name]    = ROOT.TProfile(name,title,nxbins,xlow,xhigh)

    def regProfHist2D(self,name,title="",nxbins=1,xlow=0,xhigh=1,nybins=1,ylow=0,yhigh=1):
        self.hists[name]    = ROOT.TProfile2D(name,title,nxbins,xlow,xhigh,nybins,ylow,yhigh)

    def regTHnSparse(self,name,title="",nbins=1,bins=[1],fmin=[0],fmax=[1]):
        self.hists[name]    = ROOT.THnSparseD(name,title,nbins,array("i",bins),array("d",fmin),array("d",fmax))

    def setTTree(self,tree):
        self.myTree = tree
        print "Input ROOT Tree: ", self.myTree.GetName()


    '''Set the output root file'''
    def setOutputHistFile(self,file):
        self.histFile = file

    def getOutputHistFile(self):
        return self.histFile
    
#End of class Plotbase
