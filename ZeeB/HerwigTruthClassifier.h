#ifndef HERWIGTRUTHCLASSIFIER_H
#define HERWIGTRUTHCLASSIFIER_H 


class HepMC::GenEvent;
class HepMC::GenVertex;
class HepMC::SimpleVector;

class Analysis::Electron;
class McEventCollection;


class HerwigTruthClassifier:
{
    public:
        HerwigTruthClassifier(Analysis::Electron* electron);
        ~HerwigTruthClassifier();

        const HepMC::GenParticle* getTruthParent(const Analysis::Electron*);
        bool isBElectron();
        bool isCElectron();

    private:
        std::vector<const HepMC::GenParticle*> getChildren(const HepMC::GenParticle* part);
        double deltaR(const HepMC::GenParticle* part);

    private:
        Analysis::Electron*     m_recoElectron;
        HepMC::GenParticle*     m_truthElectron;
        McEventCollection*      m_mcEventCollection;
};

#endif

