# Python job to configure SoftElectron
# Laurent Vacavant 2005/10/12

if not "ZeeB" in theApp.Dlls:
	theApp.Dlls += [ "ZeeB" ]

from TruthUtils.TruthUtilsConf import HforTool
myhforTool = HforTool("hforTool");
ToolSvc += myhforTool
ToolSvc.hforTool.RemovalSchema = "angularbased"     # "jetbased","angularbased","mc08"
# parameters used for the jetbased method
#ToolSvc.hforTool.MinJetPt = 15000

import MCTruthClassifier.MCTruthClassifierBase
from  MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
ToolSvc += MCTruthClassifier()

#Loose cuts
MCTruthClassifier.deltaRMatchCut   = 20
MCTruthClassifier.deltaPhiMatchCut = 20
MCTruthClassifier.NumOfSiHitsCut   = 3

from ZeeB.ZeeBConf import SoftElectron
MySoftElectron = SoftElectron(
	name="MySoftElectron",
        #SelectBFromRun = run_number # use only u-jets from this run
        #SelectUFromRun = run_number # use only u-jets from this run
        PrimaryVertexContainer = "VxPrimaryCandidate",
        MCParticleContainer = "SpclMC",
        MCEventContainer = "GEN_AOD",
        HardElLowPtcut  = 25,
        SoftElHighPtcut = 25,
        SoftElLowPtcut = 5
        )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += MySoftElectron
		
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='softElectron.root' OPT='RECREATE'"]
