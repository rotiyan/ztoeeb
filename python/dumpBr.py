import ROOT,sys
try:
    print "The list of branches in the root file passed as argument"
    f   = ROOT.TFile(sys.argv[1])
    t   = f.Get("el")
    t.Show()
except:
    print "No root file passed as argument"
