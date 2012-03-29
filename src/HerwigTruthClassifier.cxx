#include "ZeeB/HerwigTruthClassifier.h"


#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

HerwigTruthClassifier::HerwigTruthClassifier(HepMC::GenParticle *part):
{
    m_parntVec->clear()
    const HepMC::GenVertex *prodVtx         = part->production_vertex();
    HepMC::GenVertex::particle_iterator pin = prodvtx->particles_begin(HepMC::parents) ;

    for(; pin != prodvtx->end(); ++pin)
    {
        HepMC::GenParticle *parent   = (*pin);
        m_parntVec.push_back(parent);
    }
}

HerwigTruthClassifier::~HerwigTruthClassifier()
{}

HerwigTruthClassifier::GetParent()
{
    return m_parntVec;
}

