//
//*******************************************************
//*                                                     *
//*                      Mokka                          * 
//*   - the detailed geant4 simulation for Tesla -      *
//*                                                     *
//* For more information about Mokka, please, go to the *
//*                                                     *
//*  polype.in2p3.fr/geant4/tesla/www/mokka/mokka.html  *
//*                                                     *
//* Mokka home page.                                    *
//*                                                     *
//*******************************************************
//
// $Id$
// $Name: mokka-06-07-patch02 $
//
// Factory to create a physics list from a string.
//--------------------------------------------------------------------


#ifndef PhysicsListFactory_h
#define PhysicsListFactory_h 1

#include "G4VUserPhysicsList.hh"

class PhysicsListFactory 
{
public:

  // only method of class - returns list for name (LHEP is default)
  static G4VUserPhysicsList* create( const std::string& name ) ;
};
#endif

