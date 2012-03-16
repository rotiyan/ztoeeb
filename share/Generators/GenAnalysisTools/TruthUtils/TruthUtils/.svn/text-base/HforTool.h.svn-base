#ifndef HFORTOOL_H
#define HFORTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TruthUtils/IHforTool.h"
#include "HepMC/GenEvent.h"

#include "EventInfo/EventID.h"

#include "JetEvent/JetCollection.h"

#include <string>
#include <vector>
#include <map>

class StoreGateSvc;
class McEventCollection;
class Jet;



/**
@class     HforTool.h 

@brief     tool for removal of heavy flavor overlap in Alpgen samples

@author    Dominic Hirschbuehl
           Bergische Universitaet Wuppertal
	   Michiel Sanders
	   Ludwig-Maximilans-Universitaet Muenchen

           April, 2010
*/
class HforTool : virtual public IHforTool, public AthAlgTool {
  
 public:
  
  HforTool(const std::string&, const std::string&, const IInterface*);
  ~HforTool();
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize(); 
 
  void                    SetGenEvent     (HepMC::GenEvent*);
  HepMC::GenEvent*        GetGenEvent     ();
  
  // functions to retrieve the b/c quarks
  const std::vector<HepMC::FourVector> & get_bQuarks_MPI() ;
  const std::vector<HepMC::FourVector> & get_bQuarks_GS() ;
  const std::vector<HepMC::FourVector> & get_bQuarks_ME() ;
  const std::vector<HepMC::FourVector> & get_bQuarks_MEin() ;
  const std::vector<HepMC::FourVector> & get_bQuarks_PDF() ;
  const std::vector<HepMC::FourVector> & get_bQuarks_topdecay() ;
  const std::vector<HepMC::FourVector> & get_bQuarks_unknown() ;
  const std::vector<HepMC::FourVector> & get_cQuarks_MPI() ;
  const std::vector<HepMC::FourVector> & get_cQuarks_GS() ;
  const std::vector<HepMC::FourVector> & get_cQuarks_ME() ;
  const std::vector<HepMC::FourVector> & get_cQuarks_MEin() ;
  const std::vector<HepMC::FourVector> & get_cQuarks_PDF() ;
  const std::vector<HepMC::FourVector> & get_cQuarks_topdecay() ;
  const std::vector<HepMC::FourVector> & get_cQuarks_unknown() ;
  
  // function to retrieve the decision
  const std::string getDecision(const std::string schema = "");

 private:
  
  std::string              m_schema;
  double                   m_minjetpt;
  double                   m_matchingcone;
  bool                     m_isAtlfast;
  std::string              m_JetContainer;

  std::vector<std::string> m_McEventCollectionKeys;


  
  HepMC::GenEvent*	m_evt;
  McEventCollection*	m_mceventTESout;	
  StoreGateSvc*		m_storeGate;
  const JetCollection*  m_jets;
 
  // variable to classify the sample
  std::string m_sampleType;
  bool m_isZinclusive;

  // variable to keep the decision
  std::string m_result;

  // run/event number for the previously processed event, so that the 
  // GenEvent doesn't get read twice for one event
  EventID::number_type m_prev_run_number;
  EventID::number_type m_prev_event_number;

  // List of four-vectors for b/c quarks from MPI / gluon splitting / 
  // MatrixElement (processed by parton shower) / MatrixElement (not processed) /
  // unknown origin
  // The int key is the absolute pdgId
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_MPI ;
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_GS ;
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_ME ;
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_MEin ;
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_topdecay ;
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_PDF ;
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_unknown ;

  // StoreGate key of the McEventCollection to use; taken from the first 
  // collection found in the first event
  std::string m_McEventCollectionKey ;


  // function to loop over the generated event record, find the b/c quarks,
  // and set the corresponding lists of four-vectors
  void findHFQuarks() ;


  void jetBasedRemoval();  
  void angularBasedRemoval();  
  void keepAllRemoval();  
  
  void checkSampleType();  


  int matchdR(std::vector<HepMC::FourVector>* quarks);


  inline double deltaR(const HepMC::FourVector & v1, const Jet & v2) {
    double dphi = std::fabs(v1.phi()-v2.phi()) ;
    dphi = (dphi<=M_PI)? dphi : 2*M_PI-dphi;
    double deta = std::fabs(v1.eta()-v2.eta()) ;
    return std::sqrt(dphi*dphi+deta*deta) ;
  }

  inline double deltaR(const HepMC::FourVector & v1, const HepMC::FourVector & v2) {
    double dphi = std::fabs(v1.phi()-v2.phi()) ;
    dphi = (dphi<=M_PI)? dphi : 2*M_PI-dphi;
    double deta = std::fabs(v1.eta()-v2.eta()) ;
    return std::sqrt(dphi*dphi+deta*deta) ;
  }
  
};


inline void            	HforTool::SetGenEvent    (HepMC::GenEvent* p_evt){ m_evt = p_evt; }
inline HepMC::GenEvent*	HforTool::GetGenEvent    () 			{ return m_evt; }


#endif 
