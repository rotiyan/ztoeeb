############################################################
# AnalysisMaster.py
# These are the master job options for running your analysis
# Include your own job options as shown below ensuring you
############################################################

# We use "jp" to pass the input file to the userAlgs jO file defined below
# if you want to run over multiple files locally, use glob as shown below

import glob
from AthenaCommon.AthenaCommonFlags import jobproperties as jp
#jp.AthenaCommonFlags.FilesInput = glob.glob('/tmp/narayan/mc11_7TeV.126414.AlpgenJimmyZeeccNp0_nofilter.merge.AOD.e1008_s1310_s1300_r3043_r2993_tid679749_00/*root*')
jp.AthenaCommonFlags.FilesInput = glob.glob('/tmp/narayan/data11_7TeV.00180124.physics_Egamma.merge.AOD.r2603_p659_tid497212_00/*root*')

# to run over multiple input files
##################
#DATAPATH = '/afs/cern.ch/atlas/maxidisk/d49/'
#from glob import glob
#INPUT = glob(DATAPATH + 'AOD*.root*')
#print INPUT
#jp.AthenaCommonFlags.FilesInput = INPUT

##################
#
from RecExConfig.RecFlags import rec

jp.AthenaCommonFlags.EvtMax=500 # number of event to process

# include your algorithm job options here

#rec.UserAlgs=[ "ttbarExample_jobOptions_NEW.py" ] 
rec.UserAlgs=[ "SoftElectron_jobOptions.py"]

# Output log setting; this is for the framework in general
# You may over-ride this in your job options for your algorithm
rec.OutputLevel = INFO

# Control the writing of your own n-tuple in the alg's job options
# The following line only turns off the standard CBNT made by RecExCommon.
# and has no bearing on the ntuple that you make in your user job Options file.
#
rec.doCBNT = False

# for analysis you don't need to write out anything
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)

# to turn off perfmon - avoids writing out big ntuples

rec.doPerfMon=False

# main jobOption - must always be included
include ("RecExCommon/RecExCommon_topOptions.py")
