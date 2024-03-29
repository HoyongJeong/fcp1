#ifndef PRIGENACT_h
#define PRIGENACT_h 1

////////////////////////////////////////////////////////////////////////////////
//   PriGenAct.hh
//
//   This file is a header for PriGenAct class. You can set primary beam
// options in this class.
//
//                       - 18. Dec. 2023. Hoyong Jeong (hoyong5419@korea.ac.kr)
////////////////////////////////////////////////////////////////////////////////

#include <vector>

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4Event.hh"

class G4ParticleGun;

class PriGenAct: public G4VUserPrimaryGeneratorAction
{
  public:
	PriGenAct();
	~PriGenAct();

	virtual void GeneratePrimaries(G4Event* anEvent);

  private:
	G4ParticleGun*   m_PG;
	G4ParticleTable* m_PT;

	G4double m_BeamPX, m_BeamPY;
	G4double m_BeamDX, m_BeamDY;
	G4double m_WorldZ;
	G4ThreeVector m_GunPos;
	G4ParticleDefinition* m_Par;
	G4ThreeVector m_MomDir;
	G4double m_KinEgy;
};

#endif
