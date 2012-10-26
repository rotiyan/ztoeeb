import user
import glob
import ROOT
import sys
import PyCintex
import AthenaROOTAccess.transientTree

def toIter(beg,end):
    while beg != end:
        yield beg.__deref__()
        beg.__preinc__()
    return


ds = sys.argv[1]
if(ds):
    print glob.glob(ds+"*root*/")
    #treeList    = [AthenaROOTAccess.transientTree.makeTree(ROOT.TFile.Open(x)) for x in glob.glob(ds)]
    #chain       = ROOT.TChain("CollectionTree_trans")
    #[chain.Add(x) for x in treeList]


#for tranTree in t:
    #elColl = tranTree.ElectronAODCollection
    #for el in toIter(elColl.begin(),elColl.end()):
        #print el
