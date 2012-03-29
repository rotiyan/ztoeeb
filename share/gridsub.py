myDatasets  = ['mc11_7TeV.109300.AlpgenJimmyZeebbNp0_nofilter.merge.AOD.e835_s1310_s1300_r3043_r2993/'] #list of AMI datasets.

j = Job()
j.application = Athena()
j.application.atlas_dbrelease = 'LATEST'
j.application.option_file = ['AnalysisMaster.py' ]
j.name = 'ZeeB'
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
