import ROOT, os
from ROOT import*
import sys

def getProofFiles(Name):
    if (os.path.isfile(Name)):
        return [os.path.abspath(Name)]
    else: 
        mylist = [os.path.abspath(os.path.join(Name, file)) for file in os.listdir(Name) if "root" in file]
        return mylist

if len(sys.argv)> 1 :
    poolFiles = getProofFiles(sys.argv[1])
    #initialize proof
    proof = TProof.Open("")
    proof.SetParallel(6)
    mgr = TProof.Mgr("")
    testChain = TChain("physics")
    chain = TChain("physics")
    for file in poolFiles:
        chain.Add(file) 
    
    chain.SetProof(kTRUE,kTRUE)
    chain.Process("ElectronID.C+")

else:
    print "Give path to the files: "


