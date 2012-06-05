import ROOT, sys,os,glob 
from multiprocessing import Process,Queue,current_process

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
sfDict = {
    'ZeeNp0':   0.5057,
    'ZeeNp1':   0.3521,
    'ZeeNp2':   0.0824,
    'ZeeNp3':   0.0713,
    'ZeeNp4':   0.0537,
    'ZeeNp5':   0.0505,
    'ZeebbNp0': 0.0799,
    'ZeebbNp1': 0.0744,
    'ZeebbNp2': 0.0737,
    'ZeebbNp3': 0.0657,
    'ZeeccNp0': 0.0999,
    'ZeeccNp1': 0.1007,
    'ZeeccNp2': 0.1002,
    'ZeeccNp3': 0.0956
    }

flist   = 0
mySf    = 1
if(len(sys.argv)>1):
    flist = glob.glob(sys.argv[1]+"/*root*")
    for name,sf in sfDict.iteritems():
        if(name in sys.argv[1]):
            print "Found SF"
            mySf = sf
    
    print "current SF =",mySf 
    MultiProc(flist,1/mySf)

elif(os.path.exists(rawHistFname)):
    inFile  = ROOT.TFile(rawHistFname)
    myPlotScript = plotscript(inFile)

else:
    print "Nothing done!!!!"
