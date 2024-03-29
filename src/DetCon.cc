////////////////////////////////////////////////////////////////////////////////
//   DetCon.cc
//
//   Definitions of DetCon class's member functions.
// And it describes geometry of simulation.
//
//                       - 18. Dec. 2023. Hoyong Jeong (hoyong5419@korea.ac.kr)
////////////////////////////////////////////////////////////////////////////////

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4UIcommand.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "DetCon.hh"

//////////////////////////////////////////////////
//   Constructor and destructor
//////////////////////////////////////////////////
DetCon::DetCon()
{
	ConstructMaterials();
	DefineDimensions();
}

DetCon::~DetCon()
{
	DestructMaterials();
}

//////////////////////////////////////////////////
//   Define dimensions
//////////////////////////////////////////////////
void DetCon::DefineDimensions()
{
	// World dimensions: Laboratory size
	m_LabX = 2000. * mm; // World x dimension
	m_LabY = 2000. * mm; // World y dimension
	m_LabZ = 2000. * mm; // World z dimension

	// Scintillator dimensions
	m_SciX =   50. * mm; // Scintillator x dimension
	m_SciY =   50. * mm; // Scintillator y dimension
	m_SciZ = 1500. * mm; // Scintillator z dimension
}

//////////////////////////////////////////////////
//   Construct everything
//////////////////////////////////////////////////
G4VPhysicalVolume* DetCon::Construct()
{
	//------------------------------------------------
	//   Volumes
	//------------------------------------------------
	// World
	m_WorldSolid = new G4Box("WorldSolid", m_LabX / 2., m_LabY / 2., m_LabZ / 2.);
	m_WorldLV = new G4LogicalVolume(m_WorldSolid, m_VacMat, "WorldLV");
	m_WorldPV = new G4PVPlacement(0, G4ThreeVector(), "WorldPV", m_WorldLV, 0, false, 0);

	// Lab
	m_LabSolid = new G4Box("LabSolid", m_LabX / 2., m_LabY / 2., m_LabZ / 2.);
	m_LabLV = new G4LogicalVolume(m_LabSolid, m_AirMat, "LabLV");
	m_LabPV = new G4PVPlacement(0, G4ThreeVector(), "LabPV", m_LabLV, m_WorldPV, false, 0);

	// Scintillator
	m_SciSolid = new G4Box("SciSolid", m_SciX / 2., m_SciY / 2., m_SciZ / 2.);
	m_SciLV = new G4LogicalVolume(m_SciSolid, m_SciMat, "SciLV");
	m_SciPV = new G4PVPlacement(0, G4ThreeVector(), "SciPV", m_SciLV, m_LabPV, false, 0);


	//------------------------------------------------
	//   Surfaces
	//------------------------------------------------
	// Scintillator
	m_SciOpS = new G4OpticalSurface("SciOpS");
	m_SciOpS -> SetType(dielectric_LUTDAVIS);
	m_SciOpS -> SetFinish(RoughTeflon_LUT); // Surface property
	m_SciOpS -> SetModel(DAVIS);

	m_SciLBS = new G4LogicalBorderSurface("SciLBS", m_SciPV, m_LabPV, m_SciOpS);

	G4OpticalSurface* opS = dynamic_cast<G4OpticalSurface*>(m_SciLBS -> GetSurface(m_SciPV, m_LabPV) -> GetSurfaceProperty());
	if ( opS ) opS -> DumpInfo();

	// Air
	m_AirOpS = new G4OpticalSurface("AirOpS");
	m_AirOpS -> SetType(dielectric_dielectric);
	m_AirOpS -> SetFinish(polished);
	m_AirOpS -> SetModel(glisur);

	// Generate & Add Material Properties Table attached to the optical surfaces
	std::vector<G4double> ephoton = { 2.034 * eV, 4.136 * eV };

	// OpticalAirSurface
	std::vector<G4double> reflectivity = { 0.3, 0.5 };
	std::vector<G4double> efficiency   = { 0.8, 1.0 };

	m_AirST = new G4MaterialPropertiesTable();

	m_AirST -> AddProperty("REFLECTIVITY", ephoton, reflectivity);
	m_AirST -> AddProperty("EFFICIENCY", ephoton, efficiency);

	G4cout << "Air Surface G4MaterialPropertiesTable:" << G4endl;
	m_AirST -> DumpTable();

	m_AirOpS -> SetMaterialPropertiesTable(m_AirST);


	return m_LabPV;
}

void DetCon::ConstructMaterials()
{
	const G4double labTemp = 300.0 * kelvin;

	// Elements to be used to construct materials
	m_ElH  = new G4Element("Hydrogen",  "H",  1,   1.00794 * g/mole);
	m_ElC  = new G4Element(  "Carbon",  "C",  6,  12.0107  * g/mole);
	m_ElN  = new G4Element("Nitrogen",  "N",  7,  14.00674 * g/mole);
	m_ElO  = new G4Element(  "Oxygen",  "O",  8,  15.9994  * g/mole);
	m_ElAr = new G4Element(   "Argon", "Ar", 18,  39.948   * g/mole);

	// Vacuum material
	m_VacMat = new G4Material("Vacuum", 0.1225e-5*g/cm3, 3, kStateGas, labTemp);
	m_VacMat -> AddElement(m_ElN, 75.47/99.95);
	m_VacMat -> AddElement(m_ElO, 23.20/99.95);
	m_VacMat -> AddElement(m_ElAr, 1.28/99.95);

	// Air material
	m_AirMat = new G4Material("Air", 1.225e-3*g/cm3, 3, kStateGas, labTemp);
	m_AirMat -> AddElement(m_ElN, 75.47/99.95);
	m_AirMat -> AddElement(m_ElO, 23.20/99.95);
	m_AirMat -> AddElement(m_ElAr, 1.28/99.95);

	// Scintillator material
	// (Basen od EJ-200 datasheet: https://eljentechnology.com/products/plastic-scintillators/ej-200-ej-204-ej-208-ej-212)
	m_SciMat = new G4Material("scint", 1.023*g/cm3, 2, kStateSolid, labTemp);
	m_SciMat -> AddElement(m_ElH, 5.17 / 9.86);
	m_SciMat -> AddElement(m_ElC, 4.69 / 9.86);

	//------------------------------------------------
	// Generate & add material properties table
	//------------------------------------------------
	std::vector<G4double> photonEnergy = {2.480 * eV, 2.485 * eV, 2.490 * eV, 2.495 * eV, 2.500 * eV, 2.505 * eV, 2.510 * eV, 2.515 * eV, 2.520 * eV, 2.525 * eV,
	                                      2.530 * eV, 2.536 * eV, 2.541 * eV, 2.546 * eV, 2.551 * eV, 2.557 * eV, 2.562 * eV, 2.567 * eV, 2.572 * eV, 2.578 * eV,
	                                      2.583 * eV, 2.589 * eV, 2.594 * eV, 2.599 * eV, 2.605 * eV, 2.610 * eV, 2.616 * eV, 2.621 * eV, 2.627 * eV, 2.633 * eV,
	                                      2.638 * eV, 2.644 * eV, 2.649 * eV, 2.655 * eV, 2.661 * eV, 2.667 * eV, 2.672 * eV, 2.678 * eV, 2.684 * eV, 2.690 * eV,
	                                      2.696 * eV, 2.701 * eV, 2.707 * eV, 2.713 * eV, 2.719 * eV, 2.725 * eV, 2.731 * eV, 2.737 * eV, 2.743 * eV, 2.749 * eV,
	                                      2.755 * eV, 2.762 * eV, 2.768 * eV, 2.774 * eV, 2.780 * eV, 2.786 * eV, 2.793 * eV, 2.799 * eV, 2.805 * eV, 2.812 * eV,
	                                      2.818 * eV, 2.824 * eV, 2.831 * eV, 2.837 * eV, 2.844 * eV, 2.850 * eV, 2.857 * eV, 2.864 * eV, 2.870 * eV, 2.877 * eV,
	                                      2.884 * eV, 2.890 * eV, 2.897 * eV, 2.904 * eV, 2.911 * eV, 2.918 * eV, 2.924 * eV, 2.931 * eV, 2.938 * eV, 2.945 * eV,
	                                      2.952 * eV, 2.959 * eV, 2.966 * eV, 2.973 * eV, 2.981 * eV, 2.988 * eV, 2.995 * eV, 3.002 * eV, 3.010 * eV, 3.017 * eV,
	                                      3.024 * eV, 3.032 * eV, 3.039 * eV, 3.047 * eV, 3.054 * eV, 3.062 * eV, 3.069 * eV, 3.077 * eV, 3.084 * eV, 3.092 * eV,
	                                      3.100 * eV, 3.108 * eV, 3.115 * eV, 3.123 * eV, 3.131 * eV, 3.139 * eV};
	std::vector<G4double> rindex = {1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 
	                                1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	                                1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	                                1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	                                1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	                                1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	                                1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	                                1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	                                1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	                                1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
	                                1.58, 1.58, 1.58, 1.58, 1.58, 1.58};
	std::vector<G4double> absorption = {3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m,
	                                    3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m,
	                                    3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m,
	                                    3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m,
	                                    3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m,
	                                    3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m,
	                                    3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m,
	                                    3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m,
	                                    3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m,
	                                    3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m,
	                                    3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m, 3.8 * m};
	std::vector<G4double> scintil = {0.000, 0.062, 0.066, 0.071, 0.075, 0.079, 0.084, 0.088, 0.092, 0.097,
	                                 0.104, 0.111, 0.118, 0.125, 0.131, 0.137, 0.146, 0.155, 0.164, 0.170,
	                                 0.176, 0.182, 0.195, 0.209, 0.221, 0.233, 0.244, 0.258, 0.271, 0.285,
	                                 0.302, 0.320, 0.335, 0.351, 0.366, 0.382, 0.397, 0.408, 0.416, 0.426,
	                                 0.440, 0.453, 0.462, 0.470, 0.478, 0.488, 0.499, 0.512, 0.528, 0.543,
	                                 0.563, 0.583, 0.598, 0.612, 0.626, 0.645, 0.665, 0.685, 0.705, 0.730,
	                                 0.758, 0.779, 0.800, 0.824, 0.846, 0.866, 0.886, 0.906, 0.926, 0.944,
	                                 0.964, 0.988, 0.997, 1.000, 1.003, 1.001, 0.987, 0.973, 0.954, 0.916,
	                                 0.872, 0.815, 0.763, 0.691, 0.631, 0.559, 0.494, 0.428, 0.364, 0.314,
	                                 0.271, 0.218, 0.179, 0.147, 0.122, 0.099, 0.074, 0.049, 0.037, 0.028,
	                                 0.019, 0.015, 0.014, 0.013, 0.011, 0.000};

	m_SciMPT = new G4MaterialPropertiesTable();

	m_SciMPT->AddProperty("RINDEX", photonEnergy, rindex, false, true);

	// Check that group velocity is calculated from RINDEX
	if ( m_SciMPT -> GetProperty("RINDEX") -> GetVectorLength() != m_SciMPT -> GetProperty("GROUPVEL") -> GetVectorLength())
	{
		G4ExceptionDescription ed;
		ed << "Error calculating group velocities. Incorrect number of entries "
		      "in group velocity material property vector.";
		G4Exception("mCP::DetCon", "mCP001", FatalException, ed);
	}

	// Adding a property from two std::vectors. Argument createNewKey is false and spline is true.
	m_SciMPT -> AddProperty("ABSLENGTH", photonEnergy, absorption, false, true);

	// Adding a property using a C-style array.
	// Spline interpolation isn't used for scintillation.
	// Arguments spline and createNewKey both take default value false.
	m_SciMPT -> AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, scintil, false, true);
	m_SciMPT -> AddConstProperty("SCINTILLATIONYIELD", 10000. / MeV);
	m_SciMPT -> AddConstProperty("RESOLUTIONSCALE", .0);
	m_SciMPT -> AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1 * ns);
	m_SciMPT -> AddConstProperty("SCINTILLATIONRISETIME1"    , 0.9 * ns);
	m_SciMat -> SetMaterialPropertiesTable(m_SciMPT);


	// Air
	std::vector<G4double> rindex2 = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	m_AirMPT = new G4MaterialPropertiesTable();
	m_AirMPT -> AddProperty("RINDEX", photonEnergy, rindex2);
	G4cout << "Air G4MaterialPropertiesTable:" << G4endl;
	m_AirMPT -> DumpTable();
	m_AirMat -> SetMaterialPropertiesTable(m_AirMPT);
}

void DetCon::DestructMaterials()
{
	delete m_AirMPT;
	delete m_SciMPT;

	delete m_SciMat;
	delete m_AirMat;
	delete m_VacMat;

	delete m_ElAr;
	delete m_ElO;
	delete m_ElN;
	delete m_ElC;
	delete m_ElH;
}
