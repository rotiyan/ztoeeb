
import ROOT
import os

def getMergedHist(flist,histName):
    h = []
    for fname in flist:
        f   =   ROOT.TFile(fname+"/procHist.root")
        hist=   f.Get(histName).Clone()
        hist.SetDirectory(0)
        h.append(hist)
    
    h0 = h[0]
    for i in range(1,len(h)):
        h0.Add(h[i])
    return h0

# Make a fileName,file dictionary
zbb     = [x for x in os.listdir(".") if os.path.isdir(x) and  "ZeebbNp" in x  ]
zcc     = [x for x in os.listdir(".") if os.path.isdir(x) and  "ZeeccNp" in x  ]
zlight  = [x for x in os.listdir(".") if os.path.isdir(x) and  "ZeeNp" in x    ]


#Get the histograms
h_zb_BElEvents      = getMergedHist(zbb,"BElEvents")
h_zc_BElEvents      = getMergedHist(zcc,"BElEvents")
h_zl_BElEvents      = getMergedHist(zlight,"BElEvents")


h_zb_BHdrnEvents    = getMergedHist(zbb,"BHdrnEvents")
h_zc_BHdrnEvents    = getMergedHist(zcc,"BHdrnEvents")
h_zl_BHdrnEvents    = getMergedHist(zlight,"BHdrnEvents")

h_zb_BElPt          = getMergedHist(zbb,"BElPt")
h_zc_BElPt          = getMergedHist(zcc,"BElPt")
h_zl_BElPt          = getMergedHist(zlight,"BElPt")

h_zb_CElPt          = getMergedHist(zbb,"CElPt")
h_zc_CElPt          = getMergedHist(zcc,"CElPt")
h_zl_CElPt          = getMergedHist(zlight,"CElPt")

#########################################################
h_zb_BMult_5        = getMergedHist(zbb,"BMult_5")
h_zb_BMult_10       = getMergedHist(zbb,"BMult_10")
h_zb_BMult_15       = getMergedHist(zbb,"BMult_15")
h_zb_BMult_20       = getMergedHist(zbb,"BMult_20")
h_zb_BMult_25       = getMergedHist(zbb,"BMult_25")
h_zb_BMult_30       = getMergedHist(zbb,"BMult_30")

h_zb_CMult_5        = getMergedHist(zbb,"CMult_5")
h_zb_CMult_10       = getMergedHist(zbb,"CMult_10")
h_zb_CMult_15       = getMergedHist(zbb,"CMult_15")
h_zb_CMult_20       = getMergedHist(zbb,"CMult_20")
h_zb_CMult_25       = getMergedHist(zbb,"CMult_25")
h_zb_CMult_30       = getMergedHist(zbb,"CMult_30")

h_zc_BMult_5        = getMergedHist(zcc,"BMult_5")
h_zc_BMult_10       = getMergedHist(zcc,"BMult_10")
h_zc_BMult_15       = getMergedHist(zcc,"BMult_15")
h_zc_BMult_20       = getMergedHist(zcc,"BMult_20")
h_zc_BMult_25       = getMergedHist(zcc,"BMult_25")
h_zc_BMult_30       = getMergedHist(zcc,"BMult_30")

h_zc_CMult_5        = getMergedHist(zcc,"CMult_5")
h_zc_CMult_10       = getMergedHist(zcc,"CMult_10")
h_zc_CMult_15       = getMergedHist(zcc,"CMult_15")
h_zc_CMult_20       = getMergedHist(zcc,"CMult_20")
h_zc_CMult_25       = getMergedHist(zcc,"CMult_25")
h_zc_CMult_30       = getMergedHist(zcc,"CMult_30")

h_zl_BMult_5        = getMergedHist(zlight,"BMult_5")
h_zl_BMult_10       = getMergedHist(zlight,"BMult_10")
h_zl_BMult_15       = getMergedHist(zlight,"BMult_15")
h_zl_BMult_20       = getMergedHist(zlight,"BMult_20")
h_zl_BMult_25       = getMergedHist(zlight,"BMult_25")
h_zl_BMult_30       = getMergedHist(zlight,"BMult_30")

h_zl_CMult_5        = getMergedHist(zlight,"CMult_5")
h_zl_CMult_10       = getMergedHist(zlight,"CMult_10")
h_zl_CMult_15       = getMergedHist(zlight,"CMult_15")
h_zl_CMult_20       = getMergedHist(zlight,"CMult_20")
h_zl_CMult_25       = getMergedHist(zlight,"CMult_25")
h_zl_CMult_30       = getMergedHist(zlight,"CMult_30")
######################################################



# Draw the histograms
c1  = ROOT.TCanvas("Zbb","Zbb")
ROOT.SetOwnership(c1,False)
c1.Divide(2,2)

c1.cd(1)
h_zb_BElEvents.SetTitle("BElEvnts")
h_zb_BElEvents.Draw("colztext")

c1.cd(2)
h_zb_BHdrnEvents.SetTitle("BHdrnEvents")
h_zb_BHdrnEvents.Draw("colztext")

c1.cd(3)
h_BElEff = h_zb_BElEvents
h_BElEff.Divide(h_zb_BHdrnEvents)
h_BElEff.Draw("colztext")


###############
c2  = ROOT.TCanvas("Zcc","Zcc")
ROOT.SetOwnership(c2,False)
c2.Divide(2,2)

c2.cd(1)
h_zc_BElEvents.SetTitle("BElEvents")
h_zc_BElEvents.Draw("colztext")

c2.cd(2)
h_zc_BHdrnEvents.SetTitle("BHdrnEvents")
h_zc_BHdrnEvents.Draw("colztext")

c2.cd(3)
h_BElEff = h_zc_BElEvents.Clone()
h_BElEff.Divide(h_zc_BHdrnEvents)
h_BElEff.Draw("colztext")

##################################
c3 = ROOT.TCanvas("Elpt","Elpt")
ROOT.SetOwnership(c3,False)

h_zb_BElPt.GetXaxis().SetTitle("El pt [GeV]")
h_zb_BElPt.SetMarkerColor(ROOT.kRed+2)
h_zb_BElPt.Draw()

h_zc_BElPt.SetMarkerColor(ROOT.kBlue+2)
h_zc_BElPt.Draw("same")

h_zb_CElPt.SetMarkerColor(ROOT.kGreen+2)
h_zb_CElPt.Draw("same")

h_zc_CElPt.SetMarkerColor(ROOT.kYellow+2)
h_zc_CElPt.Draw("same")

h_zl_BElPt.SetMarkerColor(ROOT.kViolet+2)
h_zl_BElPt.Draw("same")

h_zl_CElPt.SetMarkerColor(ROOT.kAzure+2)
h_zl_CElPt.Draw("same")

leg = ROOT.TLegend(0.6,0.6,0.8,0.8)
leg.AddEntry(h_zb_BElPt,"Z+bb B El")
leg.AddEntry(h_zb_CElPt,"Z+bb C El")
leg.AddEntry(h_zc_BElPt,"Z+cc B El")
leg.AddEntry(h_zc_CElPt,"Z+cc C El")
leg.AddEntry(h_zl_BElPt,"Z+l B El")
leg.AddEntry(h_zl_CElPt,"Z+l C El")
leg.Draw("same")

ROOT.gPad.SetLogy(1)

###########################################
c4 = ROOT.TCanvas("BMult","BMult")
c4.Divide(2,2)
c4.cd(1)

ROOT.SetOwnership(c4,False)
h_zb_BMult_5.Draw()
h_zb_BMult_10.Draw("same")
h_zb_BMult_15.Draw("same")
h_zb_BMult_20.Draw("same")
h_zb_BMult_25.Draw("same")
h_zb_BMult_30.Draw("same")

leg = ROOT.TLegend(0.6,0.6,0.8,0.8)
leg.AddEntry(h_zb_BMult_5,"pt cut = 5 GeV")
leg.AddEntry(h_zb_BMult_10,"pt cut = 10 GeV")
leg.AddEntry(h_zb_BMult_15,"pt cut =15 GeV")
leg.AddEntry(h_zb_BMult_20,"pt cut =20 GeV")
leg.AddEntry(h_zb_BMult_25,"pt cut = 25 GeV")
leg.AddEntry(h_zb_BMult_30,"pt cut = 30 GeV")
leg.Draw("same")

c4.cd(2)
ROOT.SetOwnership(c5,False)
h_zc_BMult_5.Draw()
h_zc_BMult_10.Draw("same")
h_zc_BMult_15.Draw("same")
h_zc_BMult_20.Draw("same")
h_zc_BMult_25.Draw("same")
h_zc_BMult_30.Draw("same")

leg = ROOT.TLegend(0.6,0.6,0.8,0.8)
leg.AddEntry(h_zc_BMult_5,"pt cut = 5 GeV")
leg.AddEntry(h_zc_BMult_10,"pt cut = 10 GeV")
leg.AddEntry(h_zc_BMult_15,"pt cut =15 GeV")
leg.AddEntry(h_zc_BMult_20,"pt cut =20 GeV")
leg.AddEntry(h_zc_BMult_25,"pt cut = 25 GeV")
leg.AddEntry(h_zc_BMult_30,"pt cut = 30 GeV")
leg.Draw("same")

c4.cd(3)
h_zl_BMult_5.Draw()
h_zl_BMult_10.Draw("same")
h_zl_BMult_15.Draw("same")
h_zl_BMult_20.Draw("same")
h_zl_BMult_25.Draw("same")
h_zl_BMult_30.Draw("same")

leg = ROOT.TLegend(0.6,0.6,0.8,0.8)
leg.AddEntry(h_zl_BMult_5,"pt cut = 5 GeV")
leg.AddEntry(h_zl_BMult_10,"pt cut = 10 GeV")
leg.AddEntry(h_zl_BMult_15,"pt cut =15 GeV")
leg.AddEntry(h_zl_BMult_20,"pt cut =20 GeV")
leg.AddEntry(h_zl_BMult_25,"pt cut = 25 GeV")
leg.AddEntry(h_zl_BMult_30,"pt cut = 30 GeV")
leg.Draw("same")
