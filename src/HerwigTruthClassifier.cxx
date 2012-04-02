#include "ZeeB/HerwigTruthClassifier.h"


#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

#include <iostream> 
#include <cstdlib>

HerwigTruthClassifier::HerwigTruthClassifier(const HepMC::GenParticle *part)
{
    HepMC::GenParticle *parent   = this->GetMother(part);
    HepMC::GenParticle* grndParent = 0;

    int parentPdg = parent->pdg_id();
    do
    {
        grndParent = this->GetMother(const_cast<HepMC::GenParticle*>(parent));
        if(!grndParent)
            break;
        else
            parent = grndParent;

    }while(parentPdg == grndParent->pdg_id());

    m_parent = parent;
}

HerwigTruthClassifier::~HerwigTruthClassifier()
{}

HepMC::GenParticle* HerwigTruthClassifier::GetParent()
{
    return m_parent;
}

HepMC::GenParticle* HerwigTruthClassifier::GetMother(const HepMC::GenParticle* part)
{
    HepMC::GenVertex *prodVtx    = part->production_vertex();
 
    HepMC::GenParticle* parent = 0;
    std::vector<HepMC::GenParticle* > Myvector;
    if(prodVtx)
    {
        HepMC::GenVertex::particle_iterator pin = prodVtx->particles_begin(HepMC::parents);
        int counter =0;
        for(; pin != prodVtx->particles_end(HepMC::parents); ++pin)
        {
            counter++;
            parent   = (*pin);
            Myvector.push_back(parent);
        }
        if(counter>1)
        {
            std::cout <<"WARNING:: MORE THAN ONE PARENT :"<<counter <<std::endl;
            std::cout <<"Looping throught the parents" <<std::endl;
            std::vector<HepMC::GenParticle*>::iterator Iter = Myvector.begin();
            for(; Iter != Myvector.end(); ++ Iter)
            {
                std::cout << (*Iter)->pdg_id()<<","<< (*Iter)->status() <<std::endl;
            }
            std::cout <<"End of the loop " << std::endl;
        }
    }
    return parent;
}
