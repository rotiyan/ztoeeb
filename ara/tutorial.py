import user
import glob
import ROOT
import PyCintex
import AthenaROOTAccess.transientTree
t = [AthenaROOTAccess.transientTree.makeTree(ROOT.TFile.Open(x)) for x in glob.glob('/tmp/narayan/mc11_7TeV.107650.AlpgenJimmyZeeNp0_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993_tid640128_00/*root*')]


def toIter(beg,end):
    while beg != end:
        yield beg.__deref__()
        beg.__preinc__()
    return

#for tranTree in t:
#    elColl = tranTree.ElectronAODCollection
#    for el in toIter(elColl.begin(),elColl.end()):
#        print el
