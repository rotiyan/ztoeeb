#!/usr/bin/python
import ROOT, sys,os,glob 
from multiprocessing import Process,Queue,current_process
from os.path import basename

from NtupleAna import NtupleAna, plotscript
########################################
def MultiProc(flist,scaleFactor=1):
    processes   = []
    doneQue = Queue()
    try:
        for fname in flist:
            f = ROOT.TFile(fname)
            t = f.Get("el")
            plotter = NtupleAna(t, "outputHist",scaleFactor)
            p = Process(target=plotter.run)
            p.start()
            processes.append(p)
        for p in processes:
            p.join()

        os.system("hadd -f " + rawHistFname + " outputHist*")
        os.system("rm  outputHist*")

    except:
        print "something wrong"

#########################################
rawHistFname = 'rawHist.root'

from DataInfo import DataInfo
#Data luminosity in femto barn inverse
myDataInfo  = DataInfo(4.639)
sfDict      = myDataInfo.GetSigmaCalcSF()

flist   = 0
mySf    = 1
if(len(sys.argv)>1):
    print basename(sys.argv[1])
    flist = glob.glob(sys.argv[1]+"/*root*")
    for name,sf in sfDict.iteritems():
        if(name == basename(sys.argv[1]).strip('/')):
            print "Found SF"
            mySf = sf
    
    print "current SF =",mySf 
    MultiProc(flist,mySf)

elif(os.path.exists(rawHistFname)):
    inFile  = ROOT.TFile(rawHistFname)
    myPlotScript = plotscript(inFile)

else:
    print "Nothing done!!!!"
