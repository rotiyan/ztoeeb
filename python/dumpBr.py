import ROOT,sys

f   = ROOT.TFile(sys.argv[1])
t   = f.Get("el")

t.Show()
