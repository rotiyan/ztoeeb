
''' A class to supply the lumi scale factors while running over ntuples.
The class reads the data from an xml file which has to be populated before 
you ask for the scale factors. The xml file can be generated using writeXML() 
member function of the same class'''

from xml.dom.minidom import Document, parseString
import os

class DataInfo:
    def __init__(self,dataLumi):
        self.datLumi = dataLumi

        self.datainfoElement    = "datainfo"
        self.datasetElement     = "dataset"
        self.nameElement        = "name"    
        self.nEventElement      = "nEvents"
        self.sigmaMinElement    = "sigmaMin"
        self.sigmaMaxElement    = "sigmaMax"
        self.sigmaGivenElement  = "sigmaGiven"

        self.sigmaMinLumiDict   = {}
        self.sigmaMaxLumiDict   = {}
        self.sigmaGivenLumiDict = {}
        self.sigmaCalcLumiDict  = {}

        self.xmlFileName        = os.path.dirname(os.path.realpath(__file__))+'/dataInfo.xml'
        if(os.path.exists(self.xmlFileName)):
            self.CalcLumiSF(dataLumi)
        else:
            print "File",self.xmlFileName,"Does Not Exist!!. Populate the XML file with ATLAS Metadata Interface"

            print "Calling WriteXML() function.."
            self.WriteXML()
        
    def WriteXML(self):
        #Create the base element
        
        doc = Document()
        datainfo = doc.createElement(self.datainfoElement)
        doc.appendChild(datainfo)


        for i in xrange(100000):
            #create the main element
            dataset  = doc.createElement(self.datasetElement)
            datainfo.appendChild(dataset)

            name   = doc.createElement(self.nameElement)
            dataset.appendChild(name)

            nameText= doc.createTextNode(raw_input("DataSetName:"))
            name.appendChild(nameText)

            #create nnetries
            nEntries = doc.createElement(self.nEventElement)
            dataset.appendChild(nEntries)

            nEntrytext = doc.createTextNode(raw_input("NEvents:"))
            nEntries.appendChild(nEntrytext)

            sigmaMin  = doc.createElement(self.sigmaMinElement)
            dataset.appendChild(sigmaMin)

            sigmaMinText = doc.createTextNode(raw_input("Sigma Min:"))
            sigmaMin.appendChild(sigmaMinText)

            sigmaMax     = doc.createElement(self.sigmaMaxElement)
            dataset.appendChild(sigmaMax)

            sigmaMaxText = doc.createTextNode(raw_input("Sigma Max:"))
            sigmaMax.appendChild(sigmaMaxText)

            sigmaGiven = doc.createElement(self.sigmaGivenElement)
            dataset.appendChild(sigmaGiven)

            sigmaGivenText = doc.createTextNode(raw_input("Sigma Given: "))
            sigmaGiven.appendChild(sigmaGivenText)

            decision = raw_input("Do you want to continue(y/n):")
            if(decision =='n'):
                break

        f = open(self.xmlFileName,'w')
        f.write(doc.toprettyxml(indent="    "))
        
        
    def CalcLumiSF(self,dataLumi):
        #Read File
        f = open(self.xmlFileName,'r')
        data = f.read()
        f.close()

        dom = parseString(data)

        ds = dom.getElementsByTagName('dataset')
        for node in ds:
            name        = str(node.getElementsByTagName(self.nameElement)[0].childNodes[0].nodeValue.replace('\n','').replace(" ",'').encode('ascii'))
            nEvents     = float(node.getElementsByTagName(self.nEventElement)[0].childNodes[0].nodeValue.encode('ascii'))
            sigmaMin    = float(node.getElementsByTagName(self.sigmaMinElement)[0].childNodes[0].nodeValue.encode('ascii'))
            sigmaMax    = float(node.getElementsByTagName(self.sigmaMaxElement)[0].childNodes[0].nodeValue.encode('ascii'))
            sigmaGiven  = float(node.getElementsByTagName(self.sigmaGivenElement)[0].childNodes[0].nodeValue.encode('ascii'))
            sigmaCalc   = (sigmaMin + sigmaMax)/2

            self.sigmaMinLumiDict[name]     = dataLumi/(nEvents/sigmaMin/1e6)
            self.sigmaMaxLumiDict[name]     = dataLumi/(nEvents/sigmaMax/1e6)
            self.sigmaGivenLumiDict[name]   = dataLumi/(nEvents/sigmaGiven/1e6)
            self.sigmaCalcLumiDict[name]    = dataLumi/(nEvents/sigmaCalc/1e6)

    def GetSigmaMinSF(self):
        return self.sigmaMinLumiDict

    def GetSigmaMaxSF(self):
        return self.sigmaMaxLumiDict

    '''Sigma given in AMI'''
    def GetSigmaGivenSF(self):
        return self.sigmaGivenLumiDict

    def GetSigmaCalcSF(self):
        return self.sigmaCalcLumiDict
