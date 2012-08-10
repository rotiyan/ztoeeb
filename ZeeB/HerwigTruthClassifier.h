#ifndef HERWIGTRUTHCLASSIFIER_H
#define HERWIGTRUTHCLASSIFIER_H 


class HepMC::GenEvent;
class HepMC::GenVertex;
class HepMC::SimpleVector;

class Analysis::Electron;
class 

class HerwigTruthClassifier:
{
    public:
        HerwigTruthClassifier(Analysis::Electron* electron);
        ~HerwigTruthClassifier();

        const HepMC::GenParticle* getTruthParent(const Analysis::Electron*);

    private:
        Analysis::Electron*     m_recoElectron;
        HepMC::GenParticle*     m_truthElectron;
};

#endif

