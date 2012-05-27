import ROOT, sys,os,glob 
from multiprocessing import Process,Queue,current_process

from NtupleAna import NtupleAna, plotscript
########################################
def MultiProc(flist):
    processes   = []
    doneQue = Queue()
    try:
        for fname in flist:
            f = ROOT.TFile(fname)
            t = f.Get("el")
            plotter = NtupleAna(t, outFileName = "outputHist")
            p = Process(target=plotter.run)
            p.start()
            processes.append(p)
        for p in processes:
            p.join()

        os.system("hadd -f " + rawHistFname + " outputHist*")
        os.system("rm  outputHist*")

    except:
        print "something wrong"


rawHistFname = 'rawHist.root'
if( not os.path.exists(rawHistFname)):
    flist   = glob.glob(raw_input("Path to Ntuple Folder: ")+"/*root")
    MultiProc(flist)

inFile  = ROOT.TFile(rawHistFname)
myPlotScript = plotscript(inFile)
