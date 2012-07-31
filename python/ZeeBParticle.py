import ROOT

class ZeeBParticle:
    def __init__(self,chrg=0):
        self.myVector = ROOT.TLorentzVector()
        self.charge = chrg

    """setters"""
    def setPtEtaPhiE(self,pt,eta,phi,enrgy):
        self.myVector.SetPtEtaPhiE(pt,eta,phi,enrgy)

    def setPtEtaPhiM(self,pt,eta,phi,mass):
        self.myVector.SetPtEtaPhiM(pt,eta,phi,mass)

    def setCharge(self,chrg):
        self.charge = chrg

    """ getters"""
    def getPt(self):
        return self.myVector.Perp()

    def getEta(self):
        return self.myVector.Eta()

    def getPhi(self):
        return self.myVector.Phi()

    def getMass(self):
        return self.myVector.M()

    def deltaR(self,b):
        return self.myVector.DeltaR(b)

    def getCharge(self):
        return self.charge

    def getLorentzVector(self):
        return self.myVector

    """Overloading operators"""
    def __gt__(self,x):
        return self.getPt() > x.getPt()

    def __lt__(self,x):
        return self.getPt() < x.getPt()

    def __ne__(self,x):
        return self.myVector != x.myVector

class ZeeBBoson(ZeeBParticle):
    def __init__(self,pt,eta,phi,energy):
        x   = ROOT.TLorentzVector()
        x.SetPtEtaPhiM(pt,eta,phi,mass)
        self.myVector = x

class ZeeBElectron(ZeeBParticle):
    def __init__(self,pt=0,eta=0,phi=0,enrgy=0,chrg=0):
        ZeeBParticle.__init__(self,chrg)
        self.setPtEtaPhiE(pt,eta,phi,enrgy)

        self.parentPDG  = 0
        self.grndPrntPDG= 0
        
        self.author     = False
        self.softe      = False 

        self.looseID    = False
        self.loosePPID  = False
        self.mediumID   = False
        self.mediumPPID = False
        self.tightID    = False
        self.tightPPID  = False

    """Setters"""
    def setAuthor(self,val):
        if val == 1: 
            self.author =   True

    def setSofte(self,val):
        if val == 1:
            self.softe  =   True

    def setLoose(self,val):
        if val == 1:
            self.looseID =  True

    def setLoosePP(self,val):
        if val == 1:
            self.loosePPID = True

    def setMedium(self,val):
        if val ==1 : 
            self.mediumID= True

    def setMediumPP(self,val):
        if val == 1:
            self.mediumPPID = True

    def setTight(self,val):
        if val == 1:
            self.tightID    = True

    def setTightPP(self,val):
        if val == 1:
            self.tightPPID   = True

    def setParentPDG(self,val):
        if val == 1:
            self.parentPDG = True

    def setGrandPrntPDG(self,val):
        self.grndPrntPDG = val

    def isZElectron(self):
        if(self.author or (self.author and self.softe) and self.isMediumPP()):
            return True
        else:
            return False

    """getters"""
    def isAuthor(self):
        return self.author

    def isSofte(self):
        return self.softe

    def isAuthorSofte(self):
        return self.author and self.softe

    def isLoose(self):
        return self.looseID

    def isLoosePP(self):
        return self.loosePPID

    def isMedium(self):
        return self.mediumID

    def isMediumPP(self):
        return self.mediumPPID

    def isTight(self):
        return self.tightID

    def isTightPP(self):
        return self.tightPPID

    def getParentPDG(self):
        return self.parentPDG

    def getGrandparentPDG(self):
        return self.grndPrntPDG
