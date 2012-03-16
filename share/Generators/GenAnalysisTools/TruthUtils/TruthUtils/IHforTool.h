#ifndef IHFORTOOL_H
#define IHFORTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>
static const InterfaceID IID_IHforTool("IHforTool", 1, 0);

/**
@class     HforTool.h

@brief     Tool for Alpgen heavy flavor overlap removal
        
@author    Dominic Hirschbuehl
           Bergische Universitaet Wuppertal
	   Michiel Sanders
	   Ludwig-Maximilans-Universitaet Muenchen

           April, 2010
*/

namespace HepMC {class FourVector ;}

class IHforTool : virtual public IAlgTool {
        public:
        static const InterfaceID& interfaceID();     
	virtual StatusCode initialize()=0;
	
	virtual StatusCode execute()=0;

	// functions to retrieve the b/c quarks
	virtual const std::vector<HepMC::FourVector> & get_bQuarks_MPI()=0 ;
	virtual const std::vector<HepMC::FourVector> & get_bQuarks_GS()=0 ;
	virtual const std::vector<HepMC::FourVector> & get_bQuarks_ME()=0 ;
	virtual const std::vector<HepMC::FourVector> & get_bQuarks_MEin()=0 ;
	virtual const std::vector<HepMC::FourVector> & get_bQuarks_topdecay()=0 ;
	virtual const std::vector<HepMC::FourVector> & get_bQuarks_PDF()=0 ;
	virtual const std::vector<HepMC::FourVector> & get_bQuarks_unknown()=0 ;
	virtual const std::vector<HepMC::FourVector> & get_cQuarks_MPI()=0 ;
	virtual const std::vector<HepMC::FourVector> & get_cQuarks_GS()=0 ;
	virtual const std::vector<HepMC::FourVector> & get_cQuarks_ME()=0 ;
	virtual const std::vector<HepMC::FourVector> & get_cQuarks_MEin()=0 ;
	virtual const std::vector<HepMC::FourVector> & get_cQuarks_topdecay()=0 ;
	virtual const std::vector<HepMC::FourVector> & get_cQuarks_PDF()=0 ;
	virtual const std::vector<HepMC::FourVector> & get_cQuarks_unknown()=0 ;

	virtual const std::string getDecision(const std::string schema="") = 0;

};


inline const InterfaceID& IHforTool::interfaceID()
{ 
        return IID_IHforTool; 
}

#endif
