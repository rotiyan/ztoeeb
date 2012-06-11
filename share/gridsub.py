myDatasets  = [\
        #'mc11_7TeV.109300.AlpgenJimmyZeebbNp0_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/'\
        #'mc11_7TeV.109301.AlpgenJimmyZeebbNp1_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/'\
        #'mc11_7TeV.109302.AlpgenJimmyZeebbNp2_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/'\
        #'mc11_7TeV.109303.AlpgenJimmyZeebbNp3_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/'\
        #'mc11_7TeV.107650.AlpgenJimmyZeeNp0_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'\
        #'mc11_7TeV.107651.AlpgenJimmyZeeNp1_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'\
        #'mc11_7TeV.107652.AlpgenJimmyZeeNp2_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'\
        #'mc11_7TeV.107653.AlpgenJimmyZeeNp3_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'\
        #'mc11_7TeV.107654.AlpgenJimmyZeeNp4_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'\
        #'mc11_7TeV.107655.AlpgenJimmyZeeNp5_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'\
        #'mc11_7TeV.126414.AlpgenJimmyZeeccNp0_nofilter.merge.AOD.e1008_s1310_s1300_r3043_r2993/'\
        #'mc11_7TeV.126415.AlpgenJimmyZeeccNp1_nofilter.merge.AOD.e1008_s1310_s1300_r3043_r2993/'\
        #'mc11_7TeV.126416.AlpgenJimmyZeeccNp2_nofilter.merge.AOD.e1008_s1310_s1300_r3043_r2993/'\
        #'mc11_7TeV.126417.AlpgenJimmyZeeccNp3_nofilter.merge.AOD.e1008_s1310_s1300_r3043_r2993/'\
        #'mc11_7TeV.107100.AlpgenJimmyWWlnulnuNp0.merge.AOD.e835_s1372_s1370_r3043_r2993/'\
        #'mc11_7TeV.107101.AlpgenJimmyWWlnulnuNp1.merge.AOD.e835_s1372_s1370_r3043_r2993/'\
        #'mc11_7TeV.107102.AlpgenJimmyWWlnulnuNp2.merge.AOD.e835_s1372_s1370_r3043_r2993/'\
        #'mc11_7TeV.107103.AlpgenJimmyWWlnulnuNp3.merge.AOD.e835_s1372_s1370_r3043_r2993/'\
        #'mc11_7TeV.107108.AlpgenJimmyZZincllNp0.merge.AOD.e995_s1372_s1370_r3043_r2993/'\
        #'mc11_7TeV.107109.AlpgenJimmyZZincllNp1.merge.AOD.e995_s1372_s1370_r3043_r2993/'\
        #'mc11_7TeV.107110.AlpgenJimmyZZincllNp2.merge.AOD.e995_s1372_s1370_r3043_r2993/'\
        #'mc11_7TeV.107111.AlpgenJimmyZZincllNp3.merge.AOD.e995_s1372_s1370_r3043_r2993/'\
        #'mc11_7TeV.107107.AlpgenJimmyWZincllNp3.merge.AOD.e995_s1372_s1370_r3043_r2993/'\
        #'mc11_7TeV.107106.AlpgenJimmyWZincllNp2.merge.AOD.e995_s1372_s1370_r3043_r2993/'\
        #'mc11_7TeV.107105.AlpgenJimmyWZincllNp1.merge.AOD.e995_s1372_s1370_r3043_r2993/'\
        #'mc11_7TeV.107104.AlpgenJimmyWZincllNp0.merge.AOD.e995_s1372_s1370_r3043_r2993/'\
        ]

j = Job()
j.application = Athena()
j.application.atlas_dbrelease = 'LATEST'
j.application.option_file = ['AnalysisMaster.py' ]
j.name = 'DS IDENTIFIER'
j.application.prepare()
j.inputdata = DQ2Dataset()
j.inputdata.dataset = myDatasets
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
