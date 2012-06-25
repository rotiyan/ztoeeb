
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
zbb     = [x for x in os.listdir(".") if os.path.isdir(x) and  "zbbnp" in x  ]
print "Zb: ",zbb
zcc     = [x for x in os.listdir(".") if os.path.isdir(x) and  "zccnp" in x  ]
print"Zc: ",zcc
zlight  = [x for x in os.listdir(".") if os.path.isdir(x) and  "znp" in x    ]
print "Zlight: ",zlight
data_D  = [x for x in os.listdir(".") if os.path.isdir(x) and "periodD" in x]
print "Data Period D",data_D


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
# z boson mass
h_zb_Z      = getMergedHist(zbb,"Z Boson Mass")
h_zc_Z      = getMergedHist(zcc,"Z Boson Mass")
h_zl_Z      = getMergedHist(zlight,"Z Boson Mass")
h_data_d_Z  = getMergedHist(data_D,"Z Boson Mass")

#########################################################
# Z lepton kinematics
h_zb_ZEminusPt  = getMergedHist(zbb,"Boson Candidate EMinus pt")
h_zc_ZEminusPt  = getMergedHist(zcc,"Boson Candidate EMinus pt")
h_zl_ZEminusPt  = getMergedHist(zlight,"Boson Candidate EMinus pt")
h_data_ZEminusPt= getMergedHist(data_D,"Boson Candidate EMinus pt")

h_zb_ZEplusPt   = getMergedHist(zbb,"Boson Candidate Eplus pt")
h_zc_ZEplusPt   = getMergedHist(zcc,"Boson Candidate Eplus pt")
h_zl_ZEplusPt   = getMergedHist(zlight,"Boson Candidate Eplus pt")
h_data_ZEplusPt = getMergedHist(data_D,"Boson Candidate Eplus pt")

###
# Electron kinematics
h_zb_ElAuthorPt    = getMergedHist(zbb,"ElAuthorPt")
h_zc_ElAuthorPt    = getMergedHist(zcc,"ElAuthorPt")
h_zl_ElAuthorPt    = getMergedHist(zlight,"ElAuthorPt")
h_data_ElAuthorPt   = getMergedHist(data_D,"ElAuthorPt")

h_zb_ElAuthorEta    = getMergedHist(zbb,"ElAuthorEta")
h_zc_ElAuthorEta    = getMergedHist(zcc,"ElAuthorEta")
h_zl_ElAuthorEta    = getMergedHist(zlight,"ElAuthorEta")
h_data_ElAuthorEta  = getMergedHist(data_D,"ElAuthorEta")

h_zb_ElAuthorPhi    = getMergedHist(zbb,"ElAuthorPhi")
h_zc_ElAuthorPhi    = getMergedHist(zcc,"ElAuthorPhi")
h_zl_ElAuthorPhi    = getMergedHist(zlight,"ElAuthorPhi")
h_data_ElAuthorPhi  = getMergedHist(data_D,"ElAuthorPhi")

'''
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
ROOT.SetOwnership(c4,False)
c4.Divide(2,2)

c4.cd(1)
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
'''

c5 = ROOT.TCanvas("zcand","zcand")
ROOT.SetOwnership(c5,False)
c5.Divide(2,2)

c5.cd(1)
ElPtStack   = ROOT.THStack("ElPtStack","ElPtStack")
h_zl_ElAuthorPt.SetFillColor(ROOT.kBlue+1)
h_zl_ElAuthorPt.SetLineColor(ROOT.kBlue+1)

h_zb_ElAuthorPt.SetFillColor(ROOT.kRed+1)
h_zb_ElAuthorPt.SetLineColor(ROOT.kRed+1)

h_zc_ElAuthorPt.SetFillColor(ROOT.kGreen+1)
h_zc_ElAuthorPt.SetLineColor(ROOT.kGreen+1)

ElPtStack.Add(h_zb_ElAuthorPt)
ElPtStack.Add(h_zc_ElAuthorPt)
ElPtStack.Add(h_zl_ElAuthorPt)

ElPtStack.Draw("HIST")
h_data_ElAuthorPt.Scale(10)
h_data_ElAuthorPt.Draw("same")

ROOT.gPad.SetLogy(1)

c5.cd(2)

ElEtaStack  = ROOT.THStack("ElEtaStack","ElEtaStack")
h_zl_ElAuthorEta.SetFillColor(ROOT.kBlue+1)
h_zl_ElAuthorEta.SetLineColor(ROOT.kBlue+1)

h_zb_ElAuthorEta.SetFillColor(ROOT.kRed+1)
h_zb_ElAuthorEta.SetLineColor(ROOT.kRed+1)

h_zc_ElAuthorEta.SetFillColor(ROOT.kGreen+1)
h_zc_ElAuthorEta.SetLineColor(ROOT.kGreen+1)

ElEtaStack.Add(h_zb_ElAuthorEta)
ElEtaStack.Add(h_zc_ElAuthorEta)
ElEtaStack.Add(h_zl_ElAuthorEta)

ElEtaStack.Draw("HIST")

h_data_ElAuthorEta.Scale(10)
h_data_ElAuthorEta.Draw("same")

ROOT.gPad.SetLogy(1)

c5.cd(3)
ElPhiStack  = ROOT.THStack("ElPhiStack","ElPhiStack")
h_zl_ElAuthorPhi.SetFillColor(ROOT.kBlue+1)
h_zl_ElAuthorPhi.SetLineColor(ROOT.kBlue+1)

h_zb_ElAuthorPhi.SetFillColor(ROOT.kRed+1)
h_zb_ElAuthorPhi.SetLineColor(ROOT.kRed+1)

h_zc_ElAuthorPhi.SetFillColor(ROOT.kGreen+1)
h_zc_ElAuthorPhi.SetLineColor(ROOT.kGreen+1)

ElPhiStack.Add(h_zb_ElAuthorPhi)
ElPhiStack.Add(h_zc_ElAuthorPhi)
ElPhiStack.Add(h_zl_ElAuthorPhi)

ElPhiStack.Draw("HIST")

h_data_ElAuthorPhi.Scale(10)
h_data_ElAuthorPhi.Draw("same")

ROOT.gPad.SetLogy(1)


c5.cd(4)
mStack = ROOT.THStack("MassStack","MassStack")
h_zl_Z.SetFillColor(ROOT.kBlue+1)
h_zl_Z.SetLineColor(ROOT.kBlue+1)

h_zb_Z.SetFillColor(ROOT.kRed+1)
h_zb_Z.SetLineColor(ROOT.kRed+1)

h_zc_Z.SetFillColor(ROOT.kGreen+1)
h_zc_Z.SetLineColor(ROOT.kGreen+1)

mStack.Add(h_zb_Z)
mStack.Add(h_zc_Z)
mStack.Add(h_zl_Z)
mStack.Draw("HIST")

h_data_d_Z.Scale(10)
h_data_d_Z.Draw("same")

ROOT.gPad.SetLogy(1)
