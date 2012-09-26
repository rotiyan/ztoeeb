import ROOT 

# global file variables
bfile   = ROOT.TFile("zbb_output.root")
cfile   = ROOT.TFile("zcc_output.root")
zfile   = ROOT.TFile("zinc_output.root")
qfile   = ROOT.TFile("qcd_output.root")

def makePlots(histname):
    bhist           = bfile.Get(histname)
    chist           = cfile.Get(histname)
    zhist           = zfile.Get(histname)
    qhist           = qfile.Get(histname)

    '''Beautify the histograms'''
    bhist.Scale(1/bhist.Integral())
    bhist.SetLineColor(ROOT.kRed+1)
    chist.Scale(1/chist.Integral())
    chist.SetLineColor(ROOT.kGreen+1)
    zhist.Scale(1/zhist.Integral())
    zhist.SetLineColor(ROOT.kBlue+1)
    qhist.Scale(1/qhist.Integral())

    '''Add histograms to the legend'''
    leg             = ROOT.TLegend(0.6,0.7,0.85,0.85)

    leg.SetLineColor(0)
    leg.SetFillColor(0)

    leg.AddEntry(bhist,"zbb "+histname)
    leg.AddEntry(chist,"zcc "+histname)
    leg.AddEntry(zhist,"zinc "+histname)
    leg.AddEntry(qhist,"jf17 "+histname)

    bhist.Draw()
    chist.Draw("same")
    zhist.Draw("same")
    qhist.Draw("same")
    leg.Draw("same")
    ROOT.gPad.SetLogy(1)
    ROOT.gPad.Print(histname+".pdf")
 
unMatchedHists = ["nBLayerHits","nPixelHits","nSCTHits","nTRTHits","nTRTHtHits","trtHtHitFraction","reta","elAuthor","hadLeak","hadLeak1","eRatio","elPt","elEta","elPhi","elEt"]
bMatchedHists  = ["b_nBLayerHits","b_nPixelHits","b_nSCTHits","b_nTRTHits","b_nTRTHtHits","b_trtHtHitFraction","b_reta","b_elAuthor","b_hadLeak","b_hadLeak1","b_eRatio","b_elPt","b_elEta","b_elPhi","b_elEt"]
cMatchedHists  = ["c_nBLayerHits","c_nPixelHits","c_nSCTHits","c_nTRTHits","c_nTRTHtHits","c_trtHtHitFraction","c_reta","c_elAuthor","c_hadLeak","c_hadLeak1","c_eRatio","c_elPt","c_elEta","c_elPhi","c_elEt"]
zMatchedHists  = ["z_nBLayerHits","z_nPixelHits","z_nSCTHits","z_nTRTHits","z_nTRTHtHits","z_trtHtHitFraction","z_reta","z_elAuthor","z_hadLeak","z_hadLeak1","z_eRatio","z_elPt","z_elEta","z_elPhi","z_elEt"]


[makePlots(x) for x in unMatchedHists]
[makePlots(x) for x in bMatchedHists]
[makePlots(x) for x in cMatchedHists]
[makePlots(x) for x in zMatchedHists]
