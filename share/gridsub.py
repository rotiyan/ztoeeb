import sys,os
#Dictionary of datasets
zbbDict  = {\
        'zbbnp0': 'mc11_7TeV.109300.AlpgenJimmyZeebbNp0_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/',
        'zbbnp1': 'mc11_7TeV.109301.AlpgenJimmyZeebbNp1_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/',
        'zbbnp2': 'mc11_7TeV.109302.AlpgenJimmyZeebbNp2_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/',
        'zbbnp3': 'mc11_7TeV.109303.AlpgenJimmyZeebbNp3_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/'}

ztautaubbDict= {
        'ztautaubbnp0':'mc11_7TeV.109310.AlpgenJimmyZtautaubbNp0_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/',
        'ztautaubbnp1':'mc11_7TeV.109311.AlpgenJimmyZtautaubbNp1_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/',
        'ztautaubbnp2':'mc11_7TeV.109312.AlpgenJimmyZtautaubbNp2_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/',
        'ztautaubbnp3':'mc11_7TeV.109313.AlpgenJimmyZtautaubbNp3_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/'}

ztautauincDict={\
        'ztautauNp0':'mc11_7TeV.128520.AlpgenJimmyZtautauNp0_Mll250to400_pt20.merge.AOD.e997_s1372_s1370_r3043_r2993/',
        'ztautauNp1':'mc11_7TeV.128521.AlpgenJimmyZtautauNp1_Mll250to400_pt20.merge.AOD.e997_s1372_s1370_r3043_r2993/',
        'ztautauNp2':'mc11_7TeV.128522.AlpgenJimmyZtautauNp2_Mll250to400_pt20.merge.AOD.e997_s1372_s1370_r3043_r2993/',
        'ztautauNp3':'mc11_7TeV.128523.AlpgenJimmyZtautauNp3_Mll250to400_pt20.merge.AOD.e997_s1372_s1370_r3043_r2993/',
        'ztautauNp4':'mc11_7TeV.128524.AlpgenJimmyZtautauNp4_Mll250to400_pt20.merge.AOD.e997_s1372_s1370_r3043_r2993/',
        'ztautauNp5':'mc11_7TeV.128525.AlpgenJimmyZtautauNp5_Mll250to400_pt20.merge.AOD.e997_s1372_s1370_r3043_r2993/'}

zljDict = {\
        'znp0': 'mc11_7TeV.107650.AlpgenJimmyZeeNp0_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'znp1': 'mc11_7TeV.107651.AlpgenJimmyZeeNp1_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'znp2': 'mc11_7TeV.107652.AlpgenJimmyZeeNp2_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'znp3': 'mc11_7TeV.107653.AlpgenJimmyZeeNp3_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'znp4': 'mc11_7TeV.107654.AlpgenJimmyZeeNp4_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'znp5': 'mc11_7TeV.107655.AlpgenJimmyZeeNp5_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'}

wenuDict ={\
        'wenunp0':'mc11_7TeV.107680.AlpgenJimmyWenuNp0_pt20.merge.AOD.e825_s1299_s1300_r3043_r2993/',
        'wenunp1':'mc11_7TeV.107681.AlpgenJimmyWenuNp1_pt20.merge.AOD.e825_s1299_s1300_r3043_r2993/',
        'wenunp2':'mc11_7TeV.107682.AlpgenJimmyWenuNp2_pt20.merge.AOD.e825_s1299_s1300_r3043_r2993/',
        'wenunp3':'mc11_7TeV.107683.AlpgenJimmyWenuNp3_pt20.merge.AOD.e825_s1299_s1300_r3043_r2993/',
        'wenunp4':'mc11_7TeV.107684.AlpgenJimmyWenuNp4_pt20.merge.AOD.e825_s1299_s1300_r3043_r2993/',
        'wenunp5':'mc11_7TeV.107685.AlpgenJimmyWenuNp5_pt20.merge.AOD.e825_s1299_s1300_r3043_r2993/'}

wtaunuDict={\
        'wtaununp0':'mc11_7TeV.107700.AlpgenJimmyWtaunuNp0_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'wtaununp1':'mc11_7TeV.107701.AlpgenJimmyWtaunuNp1_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'wtaununp2':'mc11_7TeV.107702.AlpgenJimmyWtaunuNp2_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'wtaununp3':'mc11_7TeV.107703.AlpgenJimmyWtaunuNp3_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'wtaununp4':'mc11_7TeV.107704.AlpgenJimmyWtaunuNp4_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'wtaununp5':'mc11_7TeV.107705.AlpgenJimmyWtaunuNp5_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'}

zccDict = {\
        'zccnp0': 'mc11_7TeV.126414.AlpgenJimmyZeeccNp0_nofilter.merge.AOD.e1008_s1310_s1300_r3043_r2993/',
        'zccnp1': 'mc11_7TeV.126415.AlpgenJimmyZeeccNp1_nofilter.merge.AOD.e1008_s1310_s1300_r3043_r2993/',
        'zccnp2': 'mc11_7TeV.126416.AlpgenJimmyZeeccNp2_nofilter.merge.AOD.e1008_s1310_s1300_r3043_r2993/',
        'zccnp3': 'mc11_7TeV.126417.AlpgenJimmyZeeccNp3_nofilter.merge.AOD.e1008_s1310_s1300_r3043_r2993/'}

wwDict  = {\
        'wwlnulnunp0': 'mc11_7TeV.107100.AlpgenJimmyWWlnulnuNp0.merge.AOD.e835_s1372_s1370_r3043_r2993/',
        'wwlnulnunp1': 'mc11_7TeV.107101.AlpgenJimmyWWlnulnuNp1.merge.AOD.e835_s1372_s1370_r3043_r2993/',
        'wwlnulnunp2': 'mc11_7TeV.107102.AlpgenJimmyWWlnulnuNp2.merge.AOD.e835_s1372_s1370_r3043_r2993/',
        'wwlnulnunp3': 'mc11_7TeV.107103.AlpgenJimmyWWlnulnuNp3.merge.AOD.e835_s1372_s1370_r3043_r2993/'}

zzDict  ={\
        'zznp0': 'mc11_7TeV.107108.AlpgenJimmyZZincllNp0.merge.AOD.e995_s1372_s1370_r3043_r2993/',
        'zznp1': 'mc11_7TeV.107109.AlpgenJimmyZZincllNp1.merge.AOD.e995_s1372_s1370_r3043_r2993/',
        'zznp2': 'mc11_7TeV.107110.AlpgenJimmyZZincllNp2.merge.AOD.e995_s1372_s1370_r3043_r2993/',
        'zznp3': 'mc11_7TeV.107111.AlpgenJimmyZZincllNp3.merge.AOD.e995_s1372_s1370_r3043_r2993/',
        }

herwigzzDict={\
        'zzqqqq':'mc11_7TeV.126759.HerwigZZ_qqqq_pt100.merge.AOD.e1143_s1372_s1370_r3043_r2993/',
        'zzqqbb':'mc11_7TeV.126761.HerwigZZ_qqbb_pt100.merge.AOD.e1143_s1372_s1370_r3043_r2993/'
        }

herwigwzDict={\
        'wz':'mc11_7TeV.116125.HerwigWZ_pt100.merge.AOD.e825_s1310_s1300_r3043_r2993/',
        'wzqqqq':'mc11_7TeV.126758.HerwigWZ_qqqq_pt100.merge.AOD.e1143_s1372_s1370_r3043_r2993/',
        'wzqqbb':'mc11_7TeV.126758.HerwigWZ_qqqq_pt100.merge.AOD.e1143_s1372_s1370_r3043_r2993/'
        }

herwigwwDict={\
        'ww':'mc11_7TeV.116124.HerwigWW_pt100.merge.AOD.e825_s1310_s1300_r3043_r2993/',
        'wwqqqq':'mc11_7TeV.126757.HerwigWW_qqqq_pt100.merge.AOD.e1143_s1372_s1370_r3043_r2993/'
        }


wzDict  = {\
        'wznp0': 'mc11_7TeV.107104.AlpgenJimmyWZincllNp0.merge.AOD.e995_s1372_s1370_r3043_r2993/',
        'wznp1': 'mc11_7TeV.107105.AlpgenJimmyWZincllNp1.merge.AOD.e995_s1372_s1370_r3043_r2993/',
        'wznp2': 'mc11_7TeV.107106.AlpgenJimmyWZincllNp2.merge.AOD.e995_s1372_s1370_r3043_r2993/',
        'wznp3': 'mc11_7TeV.107107.AlpgenJimmyWZincllNp3.merge.AOD.e995_s1372_s1370_r3043_r2993/'}

ttbarDict = {\
        'ttbar':'mc11_7TeV.105200.T1_McAtNlo_Jimmy.merge.AOD.e835_s1272_s1274_r3043_r2993/'}

singleTopDict = {\
        'singleTop':'mc11_7TeV.117316.singletop_wtch_DR_PowHeg_Pythia.merge.AOD.e887_s1310_s1300_r3043_r2993/'}

jf17Dict  = {\
        'jf17':'mc11_7TeV.105830.JF17_herwig_jet_filter.merge.AOD.e995_s1372_s1370_r3043_r2993/'}

periodD = {'PeriodD' :'data11_7TeV.periodD.physics_Egamma.PhysCont.AOD.pro10_v01/'}
periodE = {'PeriodE' :'data11_7TeV.periodE.physics_Egamma.PhysCont.AOD.pro10_v01/'}
periodF = {'PeriodF' : 'data11_7TeV.periodF.physics_Egamma.PhysCont.AOD.pro10_v01/'}
periodG = {'PeriodG' : 'data11_7TeV.periodG.physics_Egamma.PhysCont.AOD.pro10_v01/'}
periodH = {'PeriodH' : 'data11_7TeV.periodH.physics_Egamma.PhysCont.AOD.pro10_v01/'}
periodI = {'PeriodI' : 'data11_7TeV.periodI.physics_Egamma.PhysCont.AOD.pro10_v01/'}
periodJ = {'PeriodJ' : 'data11_7TeV.periodJ.physics_Egamma.PhysCont.AOD.pro10_v01/'}
periodK = {'PeriodK' : 'data11_7TeV.periodK.physics_Egamma.PhysCont.AOD.pro10_v01/'}
periodL = {'PeriodL' : 'data11_7TeV.periodL.physics_Egamma.PhysCont.AOD.pro10_v01/'}
periodM = {'PeriodM' : 'data11_7TeV.periodM.physics_Egamma.PhysCont.AOD.pro10_v01/'}



dsDict      = {\
        "zbb"       : zbbDict,
        "zinc"      : zljDict,
        "zcc"       : zccDict,
        "ztautaubb" : ztautaubbDict,
        "ztautauinc": ztautauincDict,
        "wenu"      : wenuDict,
        "wtaunu"    : wtaunuDict,
        "ww"        : wwDict,
        "herwigww"  : herwigwwDict,
        "herwigzz"  : herwigzzDict,
        "herwigwz"  : herwigwzDict,
        "zz"        : zzDict,
        "wz"        : wzDict,
        "ttbar"     : ttbarDict,
        "singleTop" : singleTopDict,
        "jf17"      : jf17Dict,
        "periodD"   : periodD,
        "periodE"   : periodE,
        "periodF"   : periodF,
        "periodG"   : periodG,
        "periodH"   : periodH,
        "periodI"   : periodI,
        "periodJ"   : periodJ,
        "periodK"   : periodK,
        "periodL"   : periodL,
        "periodM"   : periodM}

#Map the hfor type to each dataset
hforDict    = {\
        "zbb"       : "isBB",
        "zinc"      : "isLightFlavor",
        "zcc"       : "isCC",
        "ztautaubb" : "isBB",
        "ztautauinc": "isLightFlavor",
        "wenu"      : "", 
        "wtaunu"    : "",
        "ww"        : "isLightFlavor",
        "herwigww"  : "",
        "herwigzz"  : "",
        "herwigwz"  : "",
        "zz"        : "isLightFlavor",
        "wz"        : "isLightFlavor",
        "ttbar"     : "",
        "singleTop" : "",
        "jf17"      : "",
        "periodD"   : "",
        "periodE"   : "",
        "periodF"   : "",
        "periodG"   : "",
        "periodH"   : "",
        "periodI"   : "",
        "periodJ"   : "",
        "periodK"   : "",
        "periodL"   : "",
        "periodM"   : ""}
#Hfor doens't work for Zcc sample !!

def gangaSub(dsDictName,dsName,dslist):
    j = Job()
    j.application = Athena()
    j.application.atlas_dbrelease = 'LATEST'
    j.application.option_file = ['AnalysisMaster.py' ]
    optionString = '-c \'from ZeeB.ZeeBConf import SoftElectron; SoftElectron.HforType=\"'+hforDict[dsDictName]+'\" \' '
    print "Athena Option: ",optionString
    j.application.options= optionString
    j.name = dsName
    j.application.prepare()
    j.inputdata = DQ2Dataset()
    j.inputdata.dataset = dslist
    #j.inputdata.goodRunListXML = File('/afs/cern.ch/user/n/narayan/public/data11_7TeV.periodAllYear_DetStatus-v36-pro10_CoolRunQuery-00-04-08_WZjets_allchannels.xml')
    j.outputdata = DQ2OutputDataset()
    j.splitter = DQ2JobSplitter()
    j.splitter.numsubjobs = 200
    j.merger = AthenaOutputMerger()

    j.application.athena_compile = False
    j.backend = Panda()
    j.backend.nobuild=True
    j.backend.requirements = PandaRequirements()
    j.backend.requirements.cloud = 'DE'
    j.backend.requirements.excluded_sites=["ANALY_SLAC","ANALY_SLAC_LMEM"]
    #j.backend.requirements.enableMerge = True
    j.submit()



if(len(sys.argv)==2):
    if(sys.argv[1] =="-1"):
        #Submit All datasets
        li = [(dsDictName,dsName,ds) for dsDictName,dataDict in dsDict.iteritems() for dsName,ds in dataDict.iteritems()]
        for dsTuple in li:
            gangaSub(dsTuple[0],dsTuple[1],[dsTuple[2]])
    elif(dsDict[sys.argv[1]]):
        inDsDict = dsDict[sys.argv[1]]
        inDsDictName = sys.argv[1]
        
        for inDsname, ds in inDsDict.iteritems():
            print inDsname,ds
            gangaSub(inDsDictName,inDsname,[ds])
    
    else:
        print "Dataset Collection ",sys.argv[1]," not found"

elif(len(sys.argv) >2):
    li = [(dsDictName,dsName,ds) for dsDictName,dataDict in dsDict.iteritems() for dsName,ds in dataDict.iteritems() if dsDictName in sys.argv]
    for dsTuple in li:
        gangaSub(dsTuple[0],dsTuple[1],dsTuple[2])
    
    
else:
    for name, ds in dsDict.iteritems():
        print name,"\n",ds,"\n"
        
    inDsDictName = raw_input("Enter data set dictionary: ")
    inDsDict =  dsDict[inDsDictName]
    print [val for name,val in inDsDict.iteritems()]

    for inDsname, ds in inDsDict.iteritems():
        print inDsname,ds
        gangaSub(inDsDictName,inDsname,[ds])
