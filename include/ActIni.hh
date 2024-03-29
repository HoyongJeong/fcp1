#ifndef ACTINI_h
#define ACTINI_h 1

////////////////////////////////////////////////////////////////////////////////
//   ActIni.hh
//
//   This file is a header for ActIni class. Every actions are initialized
// through this class.
//
//                       - 18. Dec. 2023. Hoyong Jeong (hoyong5419@korea.ac.kr)
////////////////////////////////////////////////////////////////////////////////

#include "G4VUserActionInitialization.hh"
#include "G4String.hh"

class ActIni: public G4VUserActionInitialization
{
  public:
	ActIni();
	virtual ~ActIni();

	virtual void BuildForMaster() const;
	virtual void Build() const;

  private:
};

#endif
