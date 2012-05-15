import ROOT, sys,os,glob 
from multiprocessing import Process,Queue,current_process

from plotter import MyPlotter
from plotscript import  plotscript
########################################
rawHistFname = 'rawHist.root'
flist       = glob.glob(str(sys.argv[1])+"/*root")
def MultiProc(flist):
    processes   = []
    doneQue = Queue()
    try:
        for fname in flist:
            f = ROOT.TFile(fname)
            t = f.Get("el")
            plotter = MyPlotter(t, outFileName = "outputHist")
            p = Process(target=plotter.run)
            p.start()
            processes.append(p)
        for p in processes:
            p.join()

        os.system("hadd -f " + rawHistFname + " outputHist*")
        os.system("rm  outputHist*")

    except:
        print "something wrong"

#MultiProc(flist)

inFile  = ROOT.TFile(rawHistFname)
myPlotScript = plotscript(inFile)
