myDatasets  = [\
        'mc11_7TeV.109300.AlpgenJimmyZeebbNp0_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/'\
        #'mc11_7TeV.109301.AlpgenJimmyZeebbNp1_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/'\
        #'mc11_7TeV.109302.AlpgenJimmyZeebbNp2_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/'\
        #'mc11_7TeV.109303.AlpgenJimmyZeebbNp3_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/'\
        #'mc11_7TeV.107650.AlpgenJimmyZeeNp0_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'\
        #'mc11_7TeV.107651.AlpgenJimmyZeeNp1_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'\
        #'mc11_7TeV.107652.AlpgenJimmyZeeNp2_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'\
        #'mc11_7TeV.107653.AlpgenJimmyZeeNp3_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'\
        #'mc11_7TeV.107654.AlpgenJimmyZeeNp4_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'\
        #'mc11_7TeV.107655.AlpgenJimmyZeeNp5_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/'\
        ]

j = Job()
j.application = Athena()
j.application.atlas_dbrelease = 'LATEST'
j.application.option_file = ['AnalysisMaster.py' ]
j.name = 'ZeeBNp0_AlpgenJimmy'
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
j.backend.requirements.enableMerge = True
j.submit()
