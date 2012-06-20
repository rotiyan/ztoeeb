import sys,os
#Dictionary of datasets
zbbDict  = {\
        'zbbnp0': 'mc11_7TeV.109300.AlpgenJimmyZeebbNp0_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/',
        'zbbnp1': 'mc11_7TeV.109301.AlpgenJimmyZeebbNp1_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/',
        'zbbnp2': 'mc11_7TeV.109302.AlpgenJimmyZeebbNp2_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/',
        'zbbnp3': 'mc11_7TeV.109303.AlpgenJimmyZeebbNp3_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/'}

zljDict = {\
        'znp0': 'mc11_7TeV.107650.AlpgenJimmyZeeNp0_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'znp1': 'mc11_7TeV.107651.AlpgenJimmyZeeNp1_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'znp2': 'mc11_7TeV.107652.AlpgenJimmyZeeNp2_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'znp3': 'mc11_7TeV.107653.AlpgenJimmyZeeNp3_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'znp4': 'mc11_7TeV.107654.AlpgenJimmyZeeNp4_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/',
        'znp5': 'mc11_7TeV.107655.AlpgenJimmyZeeNp5_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'}

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
        'zznp3': 'mc11_7TeV.107111.AlpgenJimmyZZincllNp3.merge.AOD.e995_s1372_s1370_r3043_r2993/'}


wzDict  = {\
        'wznp0': 'mc11_7TeV.107104.AlpgenJimmyWZincllNp0.merge.AOD.e995_s1372_s1370_r3043_r2993/',
        'wznp1': 'mc11_7TeV.107105.AlpgenJimmyWZincllNp1.merge.AOD.e995_s1372_s1370_r3043_r2993/',
        'wznp2': 'mc11_7TeV.107106.AlpgenJimmyWZincllNp2.merge.AOD.e995_s1372_s1370_r3043_r2993/',
        'wznp3': 'mc11_7TeV.107107.AlpgenJimmyWZincllNp3.merge.AOD.e995_s1372_s1370_r3043_r2993/'}

data    = {\
        'PeriodD' : 'data11_7TeV.periodD.physics_Egamma.PhysCont.AOD.pro10_v01/',
        'PeriodE' : 'data11_7TeV.periodE.physics_Egamma.PhysCont.AOD.pro10_v01/',
        #'PeriodF' : 'data11_7TeV.periodF.physics_Egamma.PhysCont.AOD.pro10_v01/',
        'PeriodG' : 'data11_7TeV.periodG.physics_Egamma.PhysCont.AOD.pro10_v01/',
        'PeriodH' : 'data11_7TeV.periodH.physics_Egamma.PhysCont.AOD.pro10_v01/',
        'PeriodI' : 'data11_7TeV.periodI.physics_Egamma.PhysCont.AOD.pro10_v01/',
        'PeriodJ' : 'data11_7TeV.periodJ.physics_Egamma.PhysCont.AOD.pro10_v01/',
        'PeriodK' : 'data11_7TeV.periodK.physics_Egamma.PhysCont.AOD.pro10_v01/',
        'PeriodL' : 'data11_7TeV.periodL.physics_Egamma.PhysCont.AOD.pro10_v01/',
        'PeriodM' : 'data11_7TeV.periodM.physics_Egamma.PhysCont.AOD.pro10_v01/'}



dsDict      = {"zbb" : zbbDict,"zinc" : zljDict,"zcc" : zccDict,"ww" : wwDict,"zz" : zzDict,"wz" : wzDict,"data",data}

#Map the hfor type to each dataset
hforDict    = {"zbb" : "isBB", "zinc": "isLightFlavor", "zcc": "", "ww": "isLightFlavor","zz":"isLightFlavor","wz": "isLightFlavor"}
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
    j.outputdata = DQ2OutputDataset()
    j.splitter = DQ2JobSplitter()
    j.splitter.numsubjobs = 10
    j.merger = AthenaOutputMerger()

    j.application.athena_compile = False
    j.backend = Panda()
    j.backend.nobuild=True
    j.backend.requirements = PandaRequirements()
    j.backend.requirements.cloud = 'DE'
    j.backend.requirements.excluded_sites=["ANALY_SLAC","ANALY_SLAC_LMEM"]
    #j.backend.requirements.enableMerge = True
    j.submit()



if(len(sys.argv)>1):
    if(sys.argv[1] =="-1"):
        #Submit All datasets
        li = [(dsDictName,dsName,ds) for dsDictName,dsDict in dsDict.iteritems() for dsName,ds in dsDict.iteritems()]
        for dsTuple in li:
            gangaSub(dsTuple[0],dsTuple[1],[dsTuple[2]])
    elif(dsDict[sys.argv[1]]):
        inDsDict = dsDict[sys.argv[1]]
        inDsDictName = dsDict[inDsDictName]
        
        for inDsname, ds in inDsDict.iteritems():
            print inDsname,ds
            gangaSub(inDsDictName,inDsname,[ds])
    
    else:
        print "Dataset Collection ",sys.argv[1]," not found"
    
else:
    for name, ds in dsDict.iteritems():
        print name,"\n",ds,"\n"
        
        inDsDictName = raw_input("Enter data set dictionary: ")
        inDsDict =  dsDict[inDsDictName]
        print [val for name,val in inDsDict.iteritems()]

        for inDsname, ds in inDsDict.iteritems():
            print inDsname,ds
            gangaSub(inDsDictName,inDsname,[ds])
