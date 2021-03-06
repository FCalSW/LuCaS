// *********************************************************
// *                         Mokka                         *
// *    -- A detailed Geant 4 simulation for the ILC --    *
// *                                                       *
// *  polywww.in2p3.fr/geant4/tesla/www/mokka/mokka.html   *
// *********************************************************
//
// $Id$
// $Name: mokka-06-07-patch02 $

#ifndef ParticleGunGenerator_hh
#define ParticleGunGenerator_hh 1

#include "G4ThreeVector.hh"
#include "globals.hh"

#include "VPrimaryGenerator.hh"

class G4ParticleGun;
class ParticleGunMessenger;
class G4Event;

class ParticleGunGenerator: public VPrimaryGenerator
{
public:
  ParticleGunGenerator(void);
  ~ParticleGunGenerator(void);

  void GeneratePrimaryVertex(G4Event *evt);
  void PrintGeneratorInfo(void);

typedef enum {kGaussian, kUniform } ESmearingMode;

public:
//Particle Gun specific Set methods:
  void SetGunPositionStep(G4ThreeVector gunPositionStep)           { fGunPositionStep = gunPositionStep; }
  void SetGunPositionSmearing(G4ThreeVector gunPositionSmearing)   { fGunPositionSmearing = gunPositionSmearing; }
  void SetGunThetaStep(G4double gunThetaStep)                      { fGunThetaStep = gunThetaStep; }
  void SetGunThetaSmearing(G4double gunThetaSmearing)              { fGunThetaSmearing = gunThetaSmearing; }
  void SetGunPhiStep(G4double gunPhiStep)                          { fGunPhiStep = gunPhiStep; }
  void SetGunPhiSmearing(G4double gunPhiSmearing)                  { fGunPhiSmearing = gunPhiSmearing; }
  void SetGunMomentum(G4double gunMomentum);                       // non-trivial
  void SetGunMomentumStep(G4double gunMomentumStep)                { fGunMomentumStep = gunMomentumStep; }
  void SetGunMomentumSmearing(G4double gunMomentumSmearing)        { fGunMomentumSmearing = gunMomentumSmearing; }
  void SetGunDirectionSmearingMode(ESmearingMode smearingMode)     { fGunDirectionSmearingMode = smearingMode; }
  void SetGunPositionSmearingMode(ESmearingMode smearingMode)      { fGunPositionSmearingMode = smearingMode; }

// Particle Gun specific Get methods:
  G4ThreeVector GetGunPositionStep(void)                           { return fGunPositionStep; }
  G4ThreeVector GetGunPositionSmearing(void)                       { return fGunPositionSmearing; }
  G4double GetGunThetaStep(void)                                   { return fGunThetaStep; }
  G4double GetGunThetaSmearing(void)                               { return fGunThetaSmearing; }
  G4double GetGunPhiStep(void)                                     { return fGunPhiStep; }
  G4double GetGunPhiSmearing(void)                                 { return fGunPhiSmearing; }
  G4double GetGunMomentum(void);                                   // non-trivial
  G4double GetGunMomentumStep(void)                                { return fGunMomentumStep; }
  G4double GetGunMomentumSmearing(void)                            { return fGunMomentumSmearing; }
  ESmearingMode GetGunDirectionSmearingMode(void)                  { return fGunDirectionSmearingMode; }
  ESmearingMode GetGunPositionSmearingMode(void)                   { return fGunPositionSmearingMode; }

private:
  G4ParticleGun * fParticleGun;
  ParticleGunMessenger * fGunMessenger;

  G4ThreeVector fGunPositionStep;
  G4ThreeVector fGunPositionSmearing;
  G4double      fGunThetaStep;
  G4double      fGunThetaSmearing;
  G4double      fGunPhiStep;
  G4double      fGunPhiSmearing;
  G4double      fGunMomentumStep;
  G4double      fGunMomentumSmearing;
  ESmearingMode fGunDirectionSmearingMode;
  ESmearingMode fGunPositionSmearingMode;
};

#endif
