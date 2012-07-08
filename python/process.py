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
sfDict = {
    'znp0':   0.4115,
    'znp1':   0.2865,
    'znp2':   0.0670,
    'znp3':   0.0580,
    'znp4':   0.0437,
    'znp5':   0.0411,
    'zbbnp0': 0.0650,
    'zbbnp1': 0.0630,
    'zbbnp2': 0.0600,
    'zbbnp3': 0.0534,
    'zccnp0': 0.0813,
    'zccnp1': 0.0819,
    'zccnp2': 0.0815,
    'zccnp3': 0.0778,
    'zznp0' : 0.0511,
    'zznp1' : 0.0467,
    'zznp2' : 0.0176,
    'zznp3' : 0.0154,
    'wwlnulnunp0':0.0419,
    'wwlnulnunp1':0.0397,
    'wwlnulnunp2':0.0293,
    'wwlnulnunp3':0.0228,
    'wenunp0':2.6393,
    'wenunp1':0.6733,
    'wenunp2':0.3941,
    'wenunp3':0.3956,
    'wenunp4':0.0937,
    'wenunp5':0.0277,
    'ttbar' : 0.0383,
    'singleTop': 0.2946,
    'jf17'  : 4879150
    }

flist   = 0
mySf    = 1
if(len(sys.argv)>1):
    print basename(sys.argv[1])
    flist = glob.glob(sys.argv[1]+"/*root*")
    for name,sf in sfDict.iteritems():
        if(name == basename(sys.argv[1])):
            print "Found SF"
            mySf = sf
    
    print "current SF =",mySf 
    MultiProc(flist,mySf)

elif(os.path.exists(rawHistFname)):
    inFile  = ROOT.TFile(rawHistFname)
    myPlotScript = plotscript(inFile)

else:
    print "Nothing done!!!!"
