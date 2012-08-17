#include "ZeeB/HerwigTruthClassifier.h"


//cpp include
#include <algorithm>

//HepMC classes
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "HepMC/SimpleVector.h"

/// the Electron
#include "egammaEvent/egamma.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"

#include "GeneratorObjects/McEventCollection.h"

HerwigTruthClassifier::HerwigTruthClassifier(Analysis::Electron* electron)
{
    //create a vector of all final state electrons generated
    const HepMC::GenEvent* GenEvent = *(m_mcEventCollection->begin());
    HepMC::GenEvent::particle_const_iterator  pitr = GenEvent->particles_begin();
    
    std::vector<const HepMC::GenParticle*> genElVector;
    for(; pitr !=  GenEvent->particles_end(); ++pitr)
    {
        const HepMC::GenParticle* part = (*pitr);
        if(part->barcode() > 10000)
            continue;
        if(std::abs(part->pdg_id()) ==11)
        {
            if(this->getChildren(part).size() !=0)
            {
                //A stable electron is the last electron in the decay chain
                continue;
            }
            else
            {
                genElVector.push_back(part);
            }
        }
    }

    //reco electron
    m_recoElectron  = electron;
    if(m_recoElectron->cluster())
    {
        double elClEta  = m_recoElectron->cluster()->eta();
        double elClPhi  = m_recoElectron->cluster()->phi();
    }
}

const HepMC::GenParticle* HerwigTruthClassifier::getTruthParent()
{

}
