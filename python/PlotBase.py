import abc
import ROOT

class PlotBase(object):
    __metaclass__   = abc.ABCMeta
    def __init__(self,hists,tree,event=0):
        self.hists  = hists
        self.myTree = tree
        self.event  = event

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
        self.__applyStyle()

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

    '''Add histograms to the private list'''
    def addh1(self,name,title="",nbins=1,low=0,high=1):
        self.hists[name]    = ROOT.TH1F(name,title,nbins,low,high)
        print self.hists

    def addh2(self,name,title="",nxbins=1,xlow=0,xhigh=1,nybins=1,ylow=0,yhigh=1):
        self.hists[name]    = ROOT.TH2F(name,title,nxbins,xlow,xhigh,nybins,ylow,yhigh)

    def addh3(self,name,title="",nxbins=1,xlow=0,xhigh=1,nybins=1,ylow=0,yhigh=1,nzbins=1,zlow=0,zhigh=1):
        self.hists[name]    = ROOT.TH3F(name,title,nxbins,xlow,xhigh,nybins,ylow,yhigh,nzbins,zlow,zhigh)

    def setTTree(self,tree):
        self.myTree = tree
        print "Input ROOT Tree: ", self.myTree.GetName()

    '''Private function to apply style in the ABC'''
    def __applyStyle(self):
        #ROOT.gSystem.Load("AtlasStyle.py")
        for name,hist in self.hists.iteritems():
            hist.Sumw2()


