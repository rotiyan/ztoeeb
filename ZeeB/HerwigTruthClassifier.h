#ifndef ZeeB_HerwigTruthClassifier
#define ZeeB_HerwigTruthClassifier

namespace HepMC{
    class GenParticle;
}
class HerwigTruthClassifier
{
    public:
        HerwigTruthClassifier(const HepMC::GenParticle* );
        ~HerwigTruthClassifier();

    public:
        HepMC::GenParticle* GetParent();

    private:
        HepMC::GenParticle* GetMother(const HepMC::GenParticle*);

    private:
        HepMC::GenParticle*  m_parent;
};

#endif
