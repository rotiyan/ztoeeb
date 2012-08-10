#include "ZeeB/HerwigTruthClassifier.h"

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


HerwigTruthClassifier::HerwigTruthClassifier(Analysis::Electron* electron)
{
    m_recoElectron  = electron;

    if(m_recoElectron->cluster())
    {
        double elClEta  = m_recoElectron->cluster()->eta();
        double elClPhi  = m_recoElectron->cluster()->phi();
    }
}

HerwigTruthClassifier::getTruthParent()
{

}
