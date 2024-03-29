#ifndef EVEACT_h
#define EVEACT_h 1

////////////////////////////////////////////////////////////////////////////////
//   EveAct.hh
//
//   This file is a header for EveAct class. User can add user-defined
// event action in this class. So this class works at every event.
//
//                       - 18. Dec. 2023. Hoyong Jeong (hoyong5419@korea.ac.kr)
////////////////////////////////////////////////////////////////////////////////

#include "globals.hh"
#include "G4UserEventAction.hh"

class G4Event;

class EveAct: public G4UserEventAction
{
  public:
	EveAct();
	virtual ~EveAct();

	virtual void BeginOfEventAction(const G4Event*);
	virtual void EndOfEventAction(const G4Event*);

	void AddScint();
	void AddCeren();

  private:
	G4int m_NScint;
	G4int m_NCeren;

};

#endif
