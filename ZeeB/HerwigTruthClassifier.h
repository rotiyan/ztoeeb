#ifndef HerwigTruthClassifer
#define HerwigTruthClassifer

namespace HepMC{
    class GenParticle;
}
class HerwigTruthClassifer
{
    public:
        HerwigTruthClassifer(HepMC::GenParticle* part);
        ~HerwigTruthClassifer();

    public:
        HepMC::GenParticle* GetParent();

    private:
        std::vector<HepMC::GenParticle* >* m_parntVec;
};
#endif
