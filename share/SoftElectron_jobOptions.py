# Python job to configure SoftElectron

include("ZeeB/Helper.py")

if not "ZeeB" in theApp.Dlls:
	theApp.Dlls += [ "ZeeB" ]


decisionGenInfo     = False
decisionTruthMatch  = False

if( not IsData()):
    decisionGenInfo     = True
    decisionTruthMatch  = True
    from TruthUtils.TruthUtilsConf import HforTool
    myhforTool = HforTool("hforTool");
    ToolSvc += myhforTool
    ToolSvc.hforTool.RemovalSchema = "angularbased"     # "jetbased","angularbased","mc08"
    # parameters used for the jetbased method
    #ToolSvc.hforTool.MinJetPt = 15000

    import MCTruthClassifier.MCTruthClassifierBase
    from  MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
    ToolSvc += MCTruthClassifier()

    #MCTruthClassifier.deltaRMatchCut   = 0.1
    #MCTruthClassifier.deltaPhiMatchCut = 0.1
    #MCTruthClassifier.NumOfSiHitsCut   = 4

from ZeeB.ZeeBConf import SoftElectron
MySoftElectron = SoftElectron(
        name="MySoftElectron",
        #SelectBFromRun = run_number # use only u-jets from this run
        #SelectUFromRun = run_number # use only u-jets from this run
        #HforType  = "isCC",
        FillGenInfo     =   decisionGenInfo)

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += MySoftElectron
		
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='softElectron.root' OPT='RECREATE'"]
