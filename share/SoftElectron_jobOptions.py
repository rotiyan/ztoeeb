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

from ZeeB.ZeeBConf import SoftElectron
MySoftElectron = SoftElectron(
	name="MySoftElectron",
        #SelectBFromRun = run_number # use only u-jets from this run
        #SelectUFromRun = run_number # use only u-jets from this run
        JetContainer= "AntiKt4TruthJets",
        PrimaryVertexContainer = "VxPrimaryCandidate",
        MCParticleContainer = "SpclMC",
        MCEventContainer = "GEN_AOD"
        )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += MySoftElectron
		
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["SoftElectron DATAFILE='softElectron.root' OPT='RECREATE'"]
