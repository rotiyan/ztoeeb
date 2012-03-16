/////////////////////////////////////////////////////////////
//
//      Tool for Alpgen heavy flavor overlap removal
//
//      authors: Dominic Hirschbuehl
//               Bergische Universitaet Wuppertal
//               Michiel Sanders
//               Ludwig-Maximilans-Universitaet Muenchen
//
//      date:   April 2010
//      
/////////////////////////////////////////////////////////////

#include "GaudiKernel/Property.h"

#include "StoreGate/StoreGateSvc.h"
// #include "DataModel/DataVector.h"
#include "GeneratorObjects/McEventCollection.h"

// #include "HepMC/GenEvent.h"
#include "TruthUtils/HforTool.h"
#include "JetEvent/JetCollection.h"

#include "EventInfo/EventInfo.h"

// new for release 17 -> new way to get the correct MC run number
#include "EventInfo/EventType.h"

//#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>

HforTool::HforTool(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
	: AthAlgTool(type, name, parent), 
	  m_schema(""),m_minjetpt(15000),m_matchingcone(0.4),
          m_evt(NULL),m_sampleType(""),m_isZinclusive(false),m_result(""),
	  m_McEventCollectionKey("")
{	
         declareInterface<IHforTool>(this); 
         declareProperty("RemovalSchema",	m_schema    = "angularbased"); 
         declareProperty("MinJetPt",    	m_minjetpt = 20000); 
         declareProperty("MatchingCone",	m_matchingcone = 0.4); 
	 declareProperty("IsAtlfastData",       m_isAtlfast =  false);
         declareProperty("JetContainer",	m_JetContainer ="AntiKt4TruthJets"); 

	 declareProperty("McEventCollectionKeys", m_McEventCollectionKeys, "Vector of possible SGKey to retrieve the McEventCollection") ;
	 m_McEventCollectionKeys.push_back("GEN_EVENT") ; // EVNT files
	 m_McEventCollectionKeys.push_back("GEN_AOD") ;   // AOD files
}

StatusCode HforTool::initialize() {
	ATH_MSG_INFO("Initializing Hfor Tool "); 

	StatusCode sc = AthAlgTool::initialize();
	if(sc.isFailure()) return sc;


	//retrieve StoreGate
	sc = service("StoreGateSvc", m_storeGate);
	if (sc.isFailure()) {
     		ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc"); 
    		return sc;
  	}
		
	//check if removal schema is supported
	if(m_schema != "jetbased" && m_schema != "angularbased" && m_schema != "strict" )	{
		ATH_MSG_ERROR("Requested removal schema is not supported, please use: jetbased, ptbased or strict");
		return StatusCode::FAILURE;
	}

	ATH_MSG_INFO(" Using " << m_schema << " removal schema");
	return StatusCode::SUCCESS;
}



StatusCode HforTool::execute() {
 	
	std::string jet = "Jets";
	if ( m_isAtlfast ) jet = "Jet";
	ATH_MSG_DEBUG(" Execute Hfor Tool ");

 	checkSampleType();
 	if (m_sampleType == "") {
	  ATH_MSG_DEBUG("Used sample doesn't need any heavy flavor overlap removal!");
	  return StatusCode::FAILURE;
 	}
	
	ATH_MSG_DEBUG(" Sample Type "<<m_sampleType);

	findHFQuarks() ;

	ATH_MSG_DEBUG("ME() =============================================");
	ATH_MSG_DEBUG("GS(b) = " << m_Quarks_GS[5].size()
	    << " ME(b) = "<<m_Quarks_ME[5].size()
	    << " MPI(b) = "<<m_Quarks_MPI[5].size()
            << " TOP(b) = "<<m_Quarks_topdecay[5].size());

	ATH_MSG_DEBUG("GS(c) = " << m_Quarks_GS[4].size()
	    << " ME(c) = "<<m_Quarks_ME[4].size()
	    << " MPI(c) = "<<m_Quarks_MPI[4].size()
            << " TOP(c) = "<<m_Quarks_topdecay[4].size());	

	if (m_schema == "jetbased") {
	  ATH_MSG_DEBUG(" Using "<<m_JetContainer<<" jet collection");
	  const JetCollection * aod_jets = 0;
	  StatusCode sc = m_storeGate->retrieve( aod_jets, m_JetContainer );
	  if ( sc.isFailure() ) {
	    ATH_MSG_WARNING("No ESD/AOD/DPD jet container found: key = " <<m_JetContainer);
	    return StatusCode::SUCCESS ;
	  }
	  m_jets = aod_jets;
	  ATH_MSG_DEBUG(" Size of Jet Collection " << aod_jets->size());
	  //	JetCollection::const_iterator jetItr  = aod_jets->begin();
	  //	JetCollection::const_iterator jetItrE = aod_jets->end();
	  
	  jetBasedRemoval(); 
	}
	else if (m_schema == "angularbased")
	  angularBasedRemoval();
	else
	  keepAllRemoval(); 


	return StatusCode::SUCCESS;
}

StatusCode HforTool::finalize() {
  return AthAlgTool::finalize();
}

	
////////////////////////////////////////////////////////////////////////	
// return the q-quarks labeled as MPI, GS, ME, incoming ME or unknown
// should these functions be inline?
////////////////////////////////////////////////////////////////////////
const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_MPI() {
  findHFQuarks() ;
  return m_Quarks_MPI[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_GS() {
  findHFQuarks() ;
  return m_Quarks_GS[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_ME() {
  findHFQuarks() ;
  return m_Quarks_ME[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_MEin() {
  findHFQuarks() ;
  return m_Quarks_MEin[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_topdecay() {
  findHFQuarks() ;
  return m_Quarks_topdecay[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_PDF() {
  findHFQuarks() ;
  return m_Quarks_PDF[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_bQuarks_unknown() {
  findHFQuarks() ;
  return m_Quarks_unknown[5] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_MPI() {
  findHFQuarks() ;
  return m_Quarks_MPI[4] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_GS() {
  findHFQuarks() ;
  return m_Quarks_GS[4] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_ME() {
  findHFQuarks() ;
  return m_Quarks_ME[4] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_MEin() {
  findHFQuarks() ;
  return m_Quarks_MEin[4] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_topdecay() {
  findHFQuarks() ;
  return m_Quarks_topdecay[4] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_PDF() {
  findHFQuarks() ;
  return m_Quarks_PDF[4] ;
}
const std::vector<HepMC::FourVector> & HforTool::get_cQuarks_unknown() {
  findHFQuarks() ;
  return m_Quarks_unknown[4] ;
}

////////////////////////////////////////////////////////////////////////
const std::string HforTool::getDecision(const std::string schema) 
////////////////////////////////////////////////////////////////////////
{

  if (schema == "") {
    // using schema given in config file
    return m_result;
  }
  else if (schema == "jetbased") {
    jetBasedRemoval(); 
    return m_result;
  }
  else if (schema == "angularbased") {
    angularBasedRemoval();
    return m_result;
  }
  else {
    keepAllRemoval(); 
    return m_result;
  }

}




////////////////////////////////////////////////////////////////////////
// Find the Heavy Flavour Quarks in this event
void HforTool::findHFQuarks()
////////////////////////////////////////////////////////////////////////
{
  // Get the event / run number from StoreGate
  const EventInfo * currentEvent(NULL) ;
  StatusCode sc = m_storeGate->retrieve(currentEvent) ;
  if ( sc.isFailure() ) {
    ATH_MSG_INFO("Couldnt retrieve EventInfo from StoreGateSvc");
    return ;
  }

  // Make sure that we haven't processed this event before
  EventType* eventType = currentEvent->event_type();
  EventID::number_type run_number(eventType->mc_channel_number()) ;
  //EventID::number_type run_number(currentEvent->event_ID()->run_number()) ;
  EventID::number_type event_number(currentEvent->event_ID()->event_number()) ;
  if ( run_number == m_prev_run_number && 
       event_number == m_prev_event_number )
    return ;

  ATH_MSG_DEBUG("runnumber " << run_number << ", eventnumber " << event_number);

  m_prev_run_number = run_number ;
  m_prev_event_number = event_number ;

  // clean up from previous event
  m_Quarks_MPI.clear() ;
  m_Quarks_GS.clear() ;
  m_Quarks_ME.clear() ;
  m_Quarks_MEin.clear() ;
  m_Quarks_topdecay.clear() ;
  m_Quarks_PDF.clear() ;
  m_Quarks_unknown.clear() ;

  // AtlasReconstruction/15.6.6/PhysicsAnalysis/AnalysisCommon/ParticleJetTools/src/JetQuarkLabel.cxx
  // get the McEventCollection from StoreGate

  // if we don't know yet which store gate key to use, find it here
  if ( !m_McEventCollectionKey.size() ) {
    for ( std::vector<std::string>::const_iterator ikey = m_McEventCollectionKeys.begin();
	  !m_McEventCollectionKey.size() && ikey != m_McEventCollectionKeys.end(); 
	  ikey++ ) {
      ATH_MSG_DEBUG("SG key " << (*ikey));
      const McEventCollection * mymcevent(NULL) ;
      sc = m_storeGate->retrieve(mymcevent, (*ikey)) ;
      if (sc.isFailure())
	ATH_MSG_DEBUG("no McEventCollection found with key " << (*ikey)); 
      else {
	m_McEventCollectionKey = (*ikey) ;
	ATH_MSG_INFO("McEventCollection found with key " << m_McEventCollectionKey);
      }
    }
    //if we still don't know which key to use, return
    if ( !m_McEventCollectionKey.size() ) {
      ATH_MSG_INFO("McEventCollection key unknown");
      return ;
    }
  }

  // Get the McEventCollection
  const McEventCollection* myMcEventCollection(NULL);
  sc = m_storeGate->retrieve(myMcEventCollection,m_McEventCollectionKey);
  if (sc.isFailure()) {
    ATH_MSG_INFO("McEventCollection not found");
    return ;
  }

  // vectors with the initial and final state b/c quarks; ie, initial or final 
  // in the parton shower; ignore b/c quarks from b/c-hadron decays
  std::map< int,std::vector<const HepMC::GenParticle*> > finalstate_q ;

  // get the GenEvent, assume it's the first one in the McEventCollection
  const HepMC::GenEvent* GenEvent = *(myMcEventCollection->begin());
  if(!GenEvent){
    ATH_MSG_INFO("GenEvent in McEventCollection not found");
    return ;
  }
  HepMC::GenEvent::particle_const_iterator pitr = GenEvent->particles_begin();
  for (; pitr != GenEvent->particles_end(); ++pitr) {
    int pdg = (*pitr)->pdg_id();
    int apdg = std::abs(pdg) ;
    if (apdg == 5 || apdg == 4) { // b or c quark
      const HepMC::GenParticle * bcpart = (*pitr) ;
      ATH_MSG_INFO("pdg = " << pdg << ": " << *bcpart);
      
      // find the production vertex and parents
      HepMC::GenVertex * prodvtx = bcpart->production_vertex() ;
      bool hasbchadronparent(false) ;
      bool hasmpiparent(false) ;
      bool hastopparent(false) ;
      bool hasWparent(false) ;
      if ( prodvtx ) {
	ATH_MSG_DEBUG("  prod vtx: " << *prodvtx);
	
	// check that there is no b/c-hadron as a parent
	// also find mpi and top parents
	HepMC::GenVertex::particle_iterator pin = 
	  prodvtx->particles_begin(HepMC::parents) ;
	for (; pin != prodvtx->particles_end(HepMC::parents) && !hasbchadronparent; pin++) {
	  ATH_MSG_DEBUG("    incoming: " << *(*pin));
	  int pdgin(abs((*pin)->pdg_id())) ;
	  
	  if ( (pdgin%10000)/1000 == apdg || (pdgin%1000)/100 == apdg ) 
	    hasbchadronparent = true ;
	  // also reject the c-quarks from a b-quark/hadron decay
	  if ( apdg == 4 && ( pdgin == 5 || (pdgin%10000)/1000 == 5 || 
			      (pdgin%1000)/100 == 5 ) )
	    hasbchadronparent = true ;
	  if ( pdgin == 0 && (*pin)->status() == 120 )
	    hasmpiparent = true ;
	  if ( pdgin == 6 ) {
	    hastopparent = true ;
	    ATH_MSG_DEBUG("  b/c parton with a top parent");
	  }
	  if ( pdgin == 24 ) {
	    hasWparent = true ;
	    ATH_MSG_DEBUG("  b/c parton with a W parent");
	  }
	}
      } else {
	ATH_MSG_DEBUG("  b/c parton without production vertex");
      }
      
      if ( hasbchadronparent )
	ATH_MSG_DEBUG("  b/c parton with a b/c quark/hadron parent") ;
      
      // find the decay vertex and children
      HepMC::GenVertex * decayvtx = bcpart->end_vertex() ;
      bool hasbcquarkdaughter(false) ;
      if ( !hasbchadronparent && decayvtx ) {
	ATH_MSG_DEBUG("  decay vtx: " << *decayvtx);
	
	// check whether there are only non-b/c-quark daughters
	HepMC::GenVertex::particle_iterator pout =
	  decayvtx->particles_begin(HepMC::children) ;
	for (; pout != decayvtx->particles_end(HepMC::children) && !hasbcquarkdaughter; pout++) {
	  ATH_MSG_DEBUG("    outgoing: " << *(*pout));
	  int pdgout(abs((*pout)->pdg_id())) ;
	    if ( pdgout == apdg )
	      hasbcquarkdaughter = true ;
	}
	} else if ( !decayvtx ) {
	ATH_MSG_DEBUG("  b/c parton without decay vertex");
      }
      
	// if no b/c-hadron parent and no b/c-quark daughter, keep it!
      if (!hasbchadronparent && !hasbcquarkdaughter) {
	ATH_MSG_DEBUG("  final state b/c-quark, barcode = "<< bcpart->barcode());
	finalstate_q[apdg].push_back(bcpart) ;
      }
      
      // if no b/c-hadron parent, check it to see whether it comes from the ME
      // but ignore the ones with an MPI or top or W parent (these also have
      // status code 123 or 124)
      if (!hasbchadronparent && !hasmpiparent && !hastopparent && !hasWparent &&
	  ( bcpart->status() == 123 || bcpart->status() == 124 ) ) {
	  ATH_MSG_DEBUG("  b/c-quark from ME");
	  m_Quarks_MEin[apdg].push_back(bcpart->momentum()) ;
      }

    } // particle is a b or a c quark

  } // loop over all particles in the GenEvent

  // loop over all the final state b/c-quarks and find out where they come from
  // first loop over quarks flavours that were stored (b,c)
  for ( std::map< int,std::vector<const HepMC::GenParticle*> >::const_iterator ipdg = finalstate_q.begin() ;
	ipdg != finalstate_q.end(); ipdg++ ) {
    int apdg(ipdg->first) ;
    ATH_MSG_DEBUG("looking for ancestors of pdg " << apdg);

    // second loop over the final state quarks
    for ( std::vector<const HepMC::GenParticle*>::const_iterator ibcpart = finalstate_q[apdg].begin() ;
	  ibcpart != finalstate_q[apdg].end(); ibcpart++ ) {
      const HepMC::GenParticle * bcpart(*(ibcpart)) ;
      ATH_MSG_DEBUG("final state b/c " << *bcpart);
      HepMC::GenVertex * prodvtx(bcpart->production_vertex()) ;
      bool isMPI(false) ;
      bool isGS(false) ;
      bool isME(false) ;
      bool isPDF(false) ;
      bool isTopDecay(false) ;
      bool isWDecay(false) ; // subset of top-decays, for hadronic top-decays
      bool iscquarkfromb(false) ;
      // if the final state quark is a PDF parton, ignore it 
      // (in AOD, descendants of these partons may be filtered out)
      if ( bcpart->status() == 141 || bcpart->status() == 142 ) {
	ATH_MSG_DEBUG("PDF !!");
	isPDF = true ;
      }
      if ( !isPDF && prodvtx ) {
	HepMC::GenVertex::particle_iterator pin = prodvtx->particles_begin(HepMC::ancestors) ;
	for ( ; pin != prodvtx->particles_end(HepMC::ancestors) && !iscquarkfromb && !isPDF ; pin++ ) {
	  int apdgin = std::abs((*pin)->pdg_id()) ;
	  if (apdgin != apdg ) {
	    ATH_MSG_DEBUG("  non b/c parent " << *(*pin));
	    // if MPI as a non-b parent, label it
	    if ( apdgin == 0 && (*pin)->status() == 120 ) {
	      ATH_MSG_DEBUG("  MPI !!");
	      isMPI = true ;
	    }
	    // gluon splitting or ME origin: in evgen files, 
	    // proton (id 2212) seems to be saved in all events; not so in 
	    // AOD files... Thus look for non-HF origin with status 121 or 122
	    if ( (*pin)->status() == 121 || (*pin)->status() == 122 ) {
	      ATH_MSG_DEBUG("  GS !!");
	      isGS = true ;
	    }	  
	    // c quark from a b quark (in b-hadron decays)
	    if ( apdg == 4 && ( apdgin == 5 || (apdgin%10000)/1000 == 5 ||
				(apdgin%1000)/100 == 5 ) ) {
	      ATH_MSG_DEBUG("  c quark from b quark or b hadron");
	      iscquarkfromb = true ;
	    }
	    // b quark from a b-hadron decay 
	    // (b directly from b-hadron already rejected)
	    if ( apdg == 5 && ( (apdgin%10000)/1000 == 5 ||
				(apdgin%1000)/100 == 5 ) ) {
	      ATH_MSG_DEBUG("  b quark from b hadron");
	      iscquarkfromb = true ;
	    }
	    // top quark decay
	    if ( apdgin == 6 ) {
	      ATH_MSG_DEBUG("  TOP !!") ;
	      isTopDecay = true ;
	    }
	    // W boson decay
	    if ( apdgin == 24 ) {
	      ATH_MSG_DEBUG("  W !!") ;
	      isWDecay = true ;
	    }
	  } else {
	    ATH_MSG_DEBUG("  b/c parent " << *(*pin));
	    // if the status of a b-quark is 123 or 124, then it is a ME b-quark
	    if ( (*pin)->status() == 123 || (*pin)->status() == 124 ) {
	      ATH_MSG_DEBUG("  ME !!");
	      isME = true ;
	    }
	    // if status 141 or 142 then it came from the PDF, ignore those!!
	    if ( (*pin)->status() == 141 || (*pin)->status() == 142 ) {
	      ATH_MSG_DEBUG("  PDF !!");
	      isPDF = true ;
	    }
	  } // b/c or non-b/c quark as parent
	} // loop over all ancestors
      } // final state b/c-quark with a production vertex
      
      // TopDecay does not depend on status codes so it comes first
      // MPI output is also status 123,124 so MPI comes before the others
      // ME parents have status 121 or 122, so ME comes before GS
      if ( !iscquarkfromb && !isPDF ) {
	if ( isTopDecay ) {
	  // if a b or c appears in the shower of a b-parton from a top decay,
	  // it should be counted as coming from the top decay; however, the 
	  // b or c should not come from a W decay in a top decay
	  if ( !isWDecay )
	    m_Quarks_topdecay[apdg].push_back( bcpart->momentum() ) ;
	}
	else if ( isMPI )
	  m_Quarks_MPI[apdg].push_back( bcpart->momentum() ) ;
	else if ( isME )
	  m_Quarks_ME[apdg].push_back( bcpart->momentum() ) ;
	else if ( isGS ) {
	  // in AOD, incoming ME partons may look like GS partons if
	  // their descendants are filtered out
	  if ( ! (bcpart->status() == 123 || bcpart->status() == 124) )
	    m_Quarks_GS[apdg].push_back( bcpart->momentum() ) ;
	  else
	    ATH_MSG_DEBUG("ME b/c-quark identified as GS");
	}
	else {
	  ATH_MSG_DEBUG("Unidentified b/c-quark");
	  m_Quarks_unknown[apdg].push_back( bcpart->momentum() ) ;
	}
      } // not a c-quark from a b decay or a PDF c-quark
            
      else if ( !iscquarkfromb && isPDF ) {
	m_Quarks_PDF[apdg].push_back(bcpart->momentum()) ;
      }

    } // loop over final state b/c-quarks
  }   // loop over quark flavours

} // end of HforTool::findHFQuarks()


////////////////////////////////////////////////////////////////////////
// No overlap removal only migration of events
// to be used only Wbb samples with phase space cuts
void HforTool::keepAllRemoval()  
////////////////////////////////////////////////////////////////////////
{

  if (m_Quarks_GS[5].size() + m_Quarks_ME[5].size() > 0)
    m_result = "isBB";
  else if (m_Quarks_GS[4].size() + m_Quarks_ME[4].size() > 0)
    m_result = "isCC";
  else
    m_result = "isLightFlavor";

}




////////////////////////////////////////////////////////////////////////
// Do the jetbased overlap removal
void HforTool::jetBasedRemoval()  
////////////////////////////////////////////////////////////////////////
{

  // container to keep matched quarks
  // 0:GS, 1:ME
  bool hasCC[2] = {false,false};
  bool hasBB[2] = {false,false};

  int sum_match = 0;

  
  ATH_MSG_DEBUG("There are " << m_jets->size()<<" jets in this event");
  for ( JetCollection::const_iterator jetItr  = m_jets->begin(); 
	jetItr != m_jets->end(); jetItr++ ) {


    const Jet* thisjet = *jetItr;
    if (thisjet->pt() > m_minjetpt) {

      int match = 0;    
      for (unsigned int i=0; i<m_Quarks_GS[5].size(); i++) {
	double dR = deltaR(m_Quarks_GS[5][i],(*thisjet));   
	ATH_MSG_DEBUG("delta R = " << dR);
	if (dR < m_matchingcone){
	  ATH_MSG_DEBUG("quark pt = " << m_Quarks_GS[5][i].perp()/1000<<" matched");
	  ++match;
	}
      }
      ATH_MSG_DEBUG("number of matched b quarks from GS = " << match);
      if (match > 1) hasBB[0] = true;

      sum_match += match;
      match = 0;    
      for (unsigned int i=0; i<m_Quarks_ME[5].size(); i++) {
	double dR = deltaR(m_Quarks_ME[5][i],(*thisjet));   
	ATH_MSG_DEBUG("delta R = " << dR);
	if (dR < m_matchingcone){
	  ATH_MSG_DEBUG("quark pt = " << m_Quarks_ME[5][i].perp()/1000<<" matched");
	  ++match;
	}
      }
      ATH_MSG_DEBUG("number of matched b quarks from ME = " << match);     
      if (match > 1) hasBB[1] = true;


      match = 0;    
      for (unsigned int i=0; i<m_Quarks_GS[4].size(); i++) {
	double dR = deltaR(m_Quarks_GS[4][i],(*thisjet));   
	ATH_MSG_DEBUG("delta R = " << dR);
	if (dR < m_matchingcone){
	  ATH_MSG_DEBUG("c quark pt = " << m_Quarks_GS[4][i].perp()/1000<<" matched");  
	  ++match;
	}
      }
      ATH_MSG_DEBUG("number of matched c quarks from GS = " << match);
      if (match > 1) hasCC[0] = true;

      sum_match += match;
      match = 0;    
      for (unsigned int i=0; i<m_Quarks_ME[4].size(); i++) {
	double dR = deltaR(m_Quarks_ME[4][i],(*thisjet));   
	ATH_MSG_DEBUG("delta R = " << dR);
	if (dR < m_matchingcone){
	  ATH_MSG_DEBUG("c quark pt = " << m_Quarks_ME[4][i].perp()/1000<<" matched");
	  ++match;
	}
      }
      ATH_MSG_DEBUG("number of matched c quarks from ME = " << match);
      if (match > 1) hasCC[1] = true;


      sum_match += match;
    }
  }

  ATH_MSG_DEBUG("number of matched quarks = " << sum_match<<" # quarks = "<<m_Quarks_ME[5].size()+m_Quarks_GS[5].size()+m_Quarks_ME[4].size()+m_Quarks_GS[4].size());
  if (sum_match == 0) {
    if (m_Quarks_GS[5].size() == 2){
      double dR = deltaR(m_Quarks_GS[5][0],m_Quarks_GS[5][1]);
      ATH_MSG_DEBUG("GS b quarks dR = " << dR);
    }
    if (m_Quarks_ME[5].size() == 2){
      double dR = deltaR(m_Quarks_ME[5][0],m_Quarks_ME[5][1]);   
      ATH_MSG_DEBUG("ME b quarks dR = " << dR);
    }

    if (m_Quarks_GS[4].size() == 2){
      double dR = deltaR(m_Quarks_GS[4][0],m_Quarks_GS[4][1]);
      ATH_MSG_DEBUG("GS c quarks dR = " << dR);
    }
    if (m_Quarks_ME[4].size() == 2){
      double dR = deltaR(m_Quarks_ME[4][0],m_Quarks_ME[4][1]);   
      ATH_MSG_DEBUG("ME c quarks dR = " << dR);
    }

  }

  //light flavor samples
  if (m_sampleType == "isLightFlavor"){
    m_result = "isLightFlavor";
    

    if (! m_isZinclusive ) {
      //remove ME HF
      if ( (m_Quarks_ME[5].size()>0) || (m_Quarks_ME[4].size()>0) ) {
	m_result = "kill";
      } else if (m_Quarks_GS[5].size()>0){
	//remove unmatched HF from GS
	if (hasBB[0]) 
	  m_result = "isBB";
	else 
	  m_result = "kill";
      } else if (m_Quarks_GS[4].size()>0){
	//remove unmatched HF from GS
	if (hasCC[0]) 
	  m_result = "isCC";
	else 
	  m_result = "kill";
      }
    } else {

      // ===========================================================
      // special case for the Z inclusive sample
      // we only have to remove overlap with Zbb samples
      
      //remove ME HF
      if (m_Quarks_ME[5].size()>0) {
	m_result = "kill";
      } else if (m_Quarks_GS[5].size()>0){
	//remove unmatched HF from GS
	if (hasBB[0]) 
	  m_result = "isBB";
	else 
	  m_result = "kill";
      } else if (m_Quarks_GS[4].size() + m_Quarks_ME[4].size()>0){
	// nothing to remove in case of c quarks
	m_result = "isCC";
      }
      
      // ===========================================================
      
    }
  }


  // cc samples 
  if (m_sampleType == "isCC"){
    m_result = "isCC";

     //remove matched ME HF
    if (hasCC[1])
      m_result = "kill";
    else if (hasBB[0])
      m_result = "isBB";
    else if (m_Quarks_GS[5].size()>0)
      m_result = "kill";

  }


  // cc samples 
  if (m_sampleType == "isC"){
    m_result = "isC";

    // only look if matched bb
    if (hasBB[0])
      m_result = "isBB";
    else if (m_Quarks_GS[5].size()>0)
      m_result = "kill";

  }
    

  // bb samples -  we only "promote" events, 
  // therefore no c-quarks have to be considered   
  if (m_sampleType == "isBB"){
    m_result = "isBB";

    //remove matched ME HF
    if (hasBB[1])
      m_result = "kill";

  }


  


  
}



////////////////////////////////////////////////////////////////////////
// Do the jetbased overlap removal
void HforTool::angularBasedRemoval()  
////////////////////////////////////////////////////////////////////////
{

  // container to keep matched quarks
  // 0:GS, 1:ME
  bool hasCC[2] = {false,false};
  bool hasBB[2] = {false,false};


  ATH_MSG_DEBUG("Using angular matching method");

  int match_bGS = matchdR(&m_Quarks_GS[5]);
  ATH_MSG_DEBUG("number of matched (dR) b quarks from GS = " << match_bGS);
  if (match_bGS > 0) hasBB[0] = true;

  int match_bME = matchdR(&m_Quarks_ME[5]);
  ATH_MSG_DEBUG("number of matched (dR) b quarks from ME = " << match_bME);
  if (match_bME > 0) hasBB[1] = true;

  int match_cGS = matchdR(&m_Quarks_GS[4]);
  ATH_MSG_DEBUG("number of matched (dR) c quarks from GS = " << match_cGS);
  if (match_cGS > 0) hasCC[0] = true;

  int match_cME = matchdR(&m_Quarks_ME[4]);
  ATH_MSG_DEBUG("number of matched (dR) c quarks from ME = " << match_cME);
  if (match_cME > 0) hasCC[1] = true;



  //light flavor samples
  if (m_sampleType == "isLightFlavor"){
    m_result = "isLightFlavor";
    

    if (! m_isZinclusive ) {
      //remove ME HF
      if ( (m_Quarks_ME[5].size()>0) || (m_Quarks_ME[4].size()>0) ) {
        m_result = "kill";
      } else if (m_Quarks_GS[5].size()>0){
        //remove unmatched HF from GS
        if (hasBB[0]) 
          m_result = "isBB";
        else 
          m_result = "kill";
      } else if (m_Quarks_GS[4].size()>0){
        //remove unmatched HF from GS
        if (hasCC[0]) 
          m_result = "isCC";
        else 
          m_result = "kill";
      }
    } else {
      
      // ===========================================================
      // special case for the Z inclusive sample
      // we only have to remove overlap with Zbb samples
      
      
      //remove ME HF
      if (m_Quarks_ME[5].size()>0) {
	m_result = "kill";
      } else if (m_Quarks_GS[5].size()>0){
	//remove unmatched HF from GS
	if (hasBB[0]) 
	  m_result = "isBB";
	else 
	  m_result = "kill";
      } else if (m_Quarks_GS[4].size() + m_Quarks_ME[4].size()>0){
	// nothing to remove in case of c quarks
	m_result = "isCC";
      }
    }
    // ===========================================================
  }


  // cc samples 
  if (m_sampleType == "isCC"){
    m_result = "isCC";

     //remove matched ME HF
    if (hasCC[1])
      m_result = "kill";
    else if (hasBB[0])
      m_result = "isBB";
    else if (m_Quarks_GS[5].size()>0)
      m_result = "kill";

  }


  // cc samples 
  if (m_sampleType == "isC"){
    m_result = "isC";

    // only look if matched bb
    if (hasBB[0])
      m_result = "isBB";
    else if (m_Quarks_GS[5].size()>0)
      m_result = "kill";

  }
    

  // bb samples -  we only "promote" events, 
  // therefore no c-quarks have to be considered   
  if (m_sampleType == "isBB"){
    m_result = "isBB";

    //remove matched ME HF
    if (hasBB[1])
      m_result = "kill";

  }
  
  ATH_MSG_DEBUG("Result  = " << m_result);
}



////////////////////////////////////////////////////////////////////////
// Perform DeltaR matching between two quarks 
int HforTool::matchdR(std::vector<HepMC::FourVector>* quarks)  
////////////////////////////////////////////////////////////////////////
{
  int match = 0;    
  if (quarks->size() > 1) {
    for (unsigned int i=0; i<quarks->size(); i++) {
      for (unsigned int j=i+1; j<quarks->size(); j++) {
	double dR = deltaR(quarks->at(i),quarks->at(j));
	ATH_MSG_DEBUG("delta R( "<<i<<","<<j<<") = " << dR);
	if (dR < m_matchingcone){
	  ++match;
	}
      }
    }
  }
  return match;
}



////////////////////////////////////////////////////////////////////////
// Check which sample we are running over
void HforTool::checkSampleType()  
////////////////////////////////////////////////////////////////////////
{
  // Get the event / run number from StoreGate
  const EventInfo * currentEvent(NULL) ;
  StatusCode sc = m_storeGate->retrieve(currentEvent) ;
  if ( sc.isFailure() ) {
    ATH_MSG_INFO("Couldnt retrieve EventInfo from StoreGateSvc");
    return ;
  }
  EventType* eventType = currentEvent->event_type();
  EventID::number_type run_number(eventType->mc_channel_number()) ;
  
  //EventID::number_type run_number(currentEvent->event_ID()->run_number()) ;


  // poor man's solution for the time being
  m_isZinclusive = false;
  // W inclusive samples
  if ( (run_number >= 107680 && run_number <= 107685)
       || (run_number >= 107690 && run_number <= 107695)
       || (run_number >= 107700 && run_number <= 107705) 
       || (run_number >= 144018 && run_number <= 144020)   // np5 excl. 
       || (run_number >= 144022 && run_number <= 144024)   // np6 incl.
       || (run_number >= 144196 && run_number <= 144207)   // SUSY filtered
       || (run_number >= 105890 && run_number <= 105897)   // ttbar + Np
       || (run_number >= 117887 && run_number <= 117899)   // ttbar + Np
       )
    m_sampleType = "isLightFlavor";

  // Z inclusive sample
  if ( (run_number >= 107650 && run_number <= 107655)
       || (run_number >= 107660 && run_number <= 107665)
       || (run_number >= 107670 && run_number <= 107675)
       || (run_number >= 107710 && run_number <= 107715)
       || (run_number >= 144192 && run_number <= 144195)
       || (run_number == 144021)
       )
    {
      m_sampleType = "isLightFlavor";
      m_isZinclusive = true;
    }

  if (run_number >= 117288 && run_number <= 117297)
    m_sampleType = "isC";

  if ( (run_number >= 117284 && run_number <= 117287)
       || (run_number == 116109)  // ttbar + cc + Np
       )  
    m_sampleType = "isCC";

  // Wbb samples 
  if ( (run_number >= 106280 && run_number <= 106283)
       || (run_number >= 107280 && run_number <= 107283) )
    m_sampleType = "isBB";

  // Zbb samples
  if ( (run_number >= 109300 && run_number <= 109313)
       || (run_number >= 118962 && run_number <= 118965) )
    m_sampleType = "isBB";
  
  // ttbb samples 
  if ( run_number == 116108 )
    m_sampleType = "isBB";



}


HforTool::~HforTool() {
}


