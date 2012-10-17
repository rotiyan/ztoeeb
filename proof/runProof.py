import ROOT, os
from ROOT import*
import sys

def getProofFiles(dirName):
    mylist = [os.path.abspath(os.path.join(dirName, file)) for file in os.listdir(dirName) if "root" in file]
    return mylist

if len(sys.argv)> 1 :
    poolFiles = getProofFiles(sys.argv[1])
    #initialize proof
    proof = TProof.Open("")
    #proof.Load("histManager/HistManager.cxx")
    proof.SetParallel(12)
    mgr = TProof.Mgr("")
    testChain = TChain("elId")
    chain = TChain("elId")
    for file in poolFiles:
        chain.Add(file)

    chain.SetProof(kTRUE,kTRUE)
    chain.Process("ElectronID.C+")

else:
    print "Give path to the files: "

