#ifndef STEACT_h
#define STEACT_h 1

////////////////////////////////////////////////////////////////////////////////
//   SetAct.hh
//
//   This file is a header for SteAct class. User can add user-defined
// stepping action in this class. So this class works at every step.
// The most busiest class.
//
//                       - 18. Dec. 2023. Hoyong Jeong (hoyong5419@korea.ac.kr)
////////////////////////////////////////////////////////////////////////////////

#include "G4UserSteppingAction.hh"

#include "EveAct.hh"

class EveAct;

class SteAct: public G4UserSteppingAction
{
  public:
	SteAct(EveAct* EA);
	virtual ~SteAct();

	virtual void UserSteppingAction(const G4Step*);

  private:
	EveAct* m_EA;
};

#endif
