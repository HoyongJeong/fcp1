////////////////////////////////////////////////////////////////////////////////
//   EveAct.cc
//
//   Definitions of EveAct class's member functions. Details of user
// actions are here.
//
//                       - 18. Dec. 2023. Hoyong Jeong (hoyong5419@korea.ac.kr)
////////////////////////////////////////////////////////////////////////////////

#include <ctime>

#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4RootAnalysisManager.hh"

#include "EveAct.hh"

//////////////////////////////////////////////////
//   Constructor
//////////////////////////////////////////////////
EveAct::EveAct()
{
	// Initialize
	m_NScint = 0;
	m_NCeren = 0;
}

//////////////////////////////////////////////////
//   Destructor
//////////////////////////////////////////////////
EveAct::~EveAct()
{
}

//////////////////////////////////////////////////
//   Begin of event action
//////////////////////////////////////////////////
void EveAct::BeginOfEventAction(const G4Event* /* anEvent */)
{
	// Initialize
	m_NScint = 0;
	m_NCeren = 0;
}

//////////////////////////////////////////////////
//   End of event action
//////////////////////////////////////////////////
void EveAct::EndOfEventAction(const G4Event* anEvent)
{
	// Get event ID
	G4int eventID = anEvent -> GetEventID();

	// Get analysis manager
	auto AM = G4RootAnalysisManager::Instance();

	AM -> FillNtupleIColumn(0, eventID);
	AM -> FillNtupleIColumn(1, m_NScint);
	AM -> FillNtupleIColumn(2, m_NCeren);
	AM -> AddNtupleRow();
}

//////////////////////////////////////////////////
//   Add optical photon
//////////////////////////////////////////////////
void EveAct::AddScint()
{
	m_NScint++;
}

void EveAct::AddCeren()
{
	m_NCeren++;
}
