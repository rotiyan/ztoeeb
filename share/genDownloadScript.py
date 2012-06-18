


ds ={} # Dictionary of dataset name and outputdataset
for j in jobs:
    ds[j.name] = j.outputdata.datasetname

f = open("dq2get.sh","w")

print ds
for dir,data in ds.iteritems():
    f.write("(mkdir -p "+dir+"; cd "+dir+"; rm -rf *; dq2-get -f \*root\* "+ data +"; mv user*/*root* .; rmdir user*; rm *log) &\n")

f.close()
