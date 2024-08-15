//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B3/B3a/src/DetectorConstruction.cc
/// \brief Implementation of the B3::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4NistManager.hh"
#include "G4PSDoseDeposit.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"

// from VitoGeant

#include "G4Element.hh" 
#include "G4ThreeVector.hh" 
#include "globals.hh"
#include "CADMesh.hh"
#include "BetaDet.hh"
#include "G4SubtractionSolid.hh"

namespace B3
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
  DefineMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  G4NistManager* man = G4NistManager::Instance();

  G4bool isotopes = false;
	G4double atomicMass;
	G4double z;
	G4double density;
	G4int numberElements;
  

	G4Element* H  = new G4Element("Hydrogen", "H",  z=1.,  atomicMass =1.008 *g/mole );
	G4Element* C  = new G4Element("Carbon",   "C",  z=6.,  atomicMass = 12.01*g/mole );
// 	G4Material*	ej200 = new G4Material( "ej200", density= 1.023 *g/cm3, numberElements=2 );
// 	ej200->AddElement( C,  469 );
// 	ej200->AddElement( H,  517);	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // BetaDetectors from VitoGeant
    // G4VPhysicalVolume* worldPhys = ConstructWorld();
  G4String prefix = "/home/ilaria/Simulations/isolde_geant4_elements/Main-elements/";
  G4ThreeVector pos(0,0,0);
  // BetaDet* betaDet = new BetaDet(prefix);

  // Define new Beta detectors with the needed geometry for each case

  // initial geometry, used for simMonoenerg simulations (which are displayed in the google sheet)

  // G4double betaside = 50 * mm, beta_dz = 6 * mm; // beta detectors dimensions
  // G4double corner_dz = 8 * mm, cornerside = 10 * mm, angle = twopi/8; // dimensions for corners which will be substracted
  // G4double diameter = 10 * mm; // dimensions for the hole

  // 2023 geometry; 2 identical detectors, made at CERN

  G4double betaside = 62 * mm, beta_dz = 6.3 * mm; // beta detectors dimensions
  G4double corner_dz = 8 * mm, cornerside = 17.7 * mm, angle = twopi/8; // dimensions for corners which will be substracted
  G4double diameter = 10 * mm; // dimensions for the hole

  // 2024 geometry; back from 2023, front is different (made at UTK) -> bigger apperture, smaller thickness, ej204

  G4double beta_dzF = 5 * mm; // beta detectors dimensions
  G4double diameterF = 20 * mm; // dimensions for the hole

  G4double atomicMass, z, density;
	G4int numberElements;
  
  // EJ 200
	G4Element* H  = new G4Element("Hydrogen", "H",  z=1.,  atomicMass =1.008 *g/mole );
	G4Element* C  = new G4Element("Carbon",   "C",  z=6.,  atomicMass = 12.01*g/mole );
	G4Material*	ej200 = new G4Material( "ej200", density= 1.023 *g/cm3, numberElements=2 );
	ej200->AddElement( C,  469 );
	ej200->AddElement( H,  517);	

  // EJ 204


  // 2023 geometry
  // auto solidBetaF = new G4Box("betaF", 0.5 * betaside, 0.5 * betaside, 0.5 * beta_dz); 
  // auto holeF = new G4Tubs("holeF", 0, 0.5 * diameter, corner_dz,0, twopi);
  // 2024 geometry
  auto solidBetaF = new G4Box("betaF", 0.5 * betaside, 0.5 * betaside, 0.5 * beta_dzF); // uncomment for 2024 geometry
  auto holeF = new G4Tubs("holeF", 0, 0.5 * diameterF, corner_dz,0, twopi);

  auto corner = new G4Box("corner1", 0.5*cornerside, 0.5*cornerside, 0.5*corner_dz);
  G4SubtractionSolid* volume1 = new G4SubtractionSolid("volume1", solidBetaF, holeF);

  G4RotationMatrix *rotm1 = new G4RotationMatrix();
  rotm1->rotateZ(45.*deg);
  G4RotationMatrix *rotm2 = new G4RotationMatrix();
  rotm2->rotateZ(135.*deg);
  G4RotationMatrix *rotm3 = new G4RotationMatrix();
  rotm3->rotateZ(225.*deg);
  G4RotationMatrix *rotm4 = new G4RotationMatrix();
  rotm4->rotateZ(315.*deg);

  G4SubtractionSolid* volume2 = new G4SubtractionSolid("volume2", volume1, corner, rotm1, G4ThreeVector(31,31,0));
  G4SubtractionSolid* volume3 = new G4SubtractionSolid("volume3", volume2, corner, rotm2, G4ThreeVector(-31,31,0));
  G4SubtractionSolid* volume4 = new G4SubtractionSolid("volume4", volume3, corner, rotm3, G4ThreeVector(-31,-31,0));
  G4SubtractionSolid* volume5 = new G4SubtractionSolid("volume5", volume4, corner, rotm4, G4ThreeVector(31,-31,0));

  auto solidBetaB = new G4Box("betaB", 0.5 * betaside, 0.5 * betaside, 0.5 * beta_dz);
  auto holeB = new G4Tubs("holeB", 0, 0.5 * diameter, corner_dz,0, twopi); 
  G4SubtractionSolid* volume6 = new G4SubtractionSolid("volume6", solidBetaB, holeB);
  G4SubtractionSolid* volume7 = new G4SubtractionSolid("volume7", volume6, corner, rotm1, G4ThreeVector(31,31,0));
  G4SubtractionSolid* volume8 = new G4SubtractionSolid("volume8", volume7, corner, rotm2, G4ThreeVector(-31,31,0));
  G4SubtractionSolid* volume9 = new G4SubtractionSolid("volume9", volume8, corner, rotm3, G4ThreeVector(-31,-31,0));
  G4SubtractionSolid* volume10 = new G4SubtractionSolid("volume10", volume9, corner, rotm4, G4ThreeVector(31,-31,0));

  G4VisAttributes* BetaDetVis = new G4VisAttributes( G4Colour(0.8,0.2,0.0));
  BetaDetVis->SetForceAuxEdgeVisible(true);
  BetaDetVis->SetForceSolid(true);

  auto logicBetaF = new G4LogicalVolume(volume5,  // its solid
                                           ej200,  // its material
                                           "betaFront");  // its name                                        

  logicBetaF->SetVisAttributes(BetaDetVis);

  auto logicBetaB = new G4LogicalVolume(volume10,  // its solid
                                           ej200,  // its material
                                           "betaBack");  // its name

  logicBetaB->SetVisAttributes(BetaDetVis);
  
  // //
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* default_mat = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* vacuum = new G4Material("GalacticVacuum", 1., 1.01*g/mole,
                           CLHEP::universe_mean_density, 
                           kStateGas, 3.e-18*pascal, 2.73*kelvin);

  G4double world_sizeXY = 1 * m;
  G4double world_sizeZ = 1 * m;

  auto solidWorld =
    new G4Box("World",  // its name
              0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);  // its size

  auto logicWorld = new G4LogicalVolume(solidWorld,  // its solid
                                        vacuum,  // its material
                                        "World");  // its name

  auto physWorld = new G4PVPlacement(nullptr,  // no rotation
                                     G4ThreeVector(),  // at (0,0,0)
                                     logicWorld,  // its logical volume
                                     "World",  // its name
                                     nullptr,  // its mother  volume
                                     false,  // no boolean operation
                                     0,  // copy number
                                     fCheckOverlaps);  // checking overlaps

  // betaDet->Place(0, pos, "BetaDet", logicWorld);      
  // betaDet->MakeSensitiveDet();

  //
  // implanter crystal, thin disk
  //

  // initial geometry, used for simMonoenerg simulations (which are displayed in the google sheet)
  // G4double sample_radius = 1 * cm, sample_dZ = 2 * mm; // thickness varies from 0.2 mm to 2 mm with a step of 0.1 mm

  // 2023 geometry
  // G4double sample_radius = 0.5 * 12 * mm, sample_dZ = 0.5 * mm; 

  // // 2024 geometry
  G4double sample_radius = 0.5 * 20 * mm, sample_dZ = 2. * mm; 

  G4NistManager* man=G4NistManager::Instance();
	G4Element* K = man->FindOrBuildElement (19);
	G4Element* Cl = man->FindOrBuildElement(17);
	G4Material*	KCl = new G4Material( "KCl", density= 1.98 *g/cm3, numberElements=2 );
	KCl->AddElement( K,  1 );
	KCl->AddElement( Cl,  1);

  auto solidsample = new G4Tubs("sample", 0., sample_radius, 0.5 * sample_dZ, 0., twopi);

  auto logicsample = new G4LogicalVolume(solidsample,  // its solid
                                          KCl,  // its material
                                          "sampleLV");  // its name

  G4VisAttributes* sampleVis = new G4VisAttributes( G4Colour(0.0,0.8,0.2));
  sampleVis->SetForceAuxEdgeVisible(true);
  sampleVis->SetForceSolid(true);
  logicsample->SetVisAttributes(sampleVis);


  // place new beta in world

  new G4PVPlacement(nullptr,  // no rotation
                    G4ThreeVector(0,0,-29),  // 29.65 if 2023, 29 if 2024
                    logicBetaF,  // its logical volume
                    "betaFront",  // its name
                    logicWorld,  // its mother  volume
                    false,  // no boolean operation
                    0,  // copy number
                    fCheckOverlaps);  // checking overlaps

  new G4PVPlacement(nullptr,  // no rotation
                    G4ThreeVector(0,0,+29.65),  //
                    logicBetaB,  // its logical volume
                    "betaBack",  // its name
                    logicWorld,  // its mother  volume
                    false,  // no boolean operation
                    0,  // copy number
                    fCheckOverlaps);  // checking overlaps
  
  // place sample in world
  
  new G4PVPlacement(nullptr,  // no rotation
                    G4ThreeVector(),  // at (0,0,0)
                    logicsample,  // its logical volume
                    "sample",  // its name
                    logicWorld,  // its mother  volume
                    false,  // no boolean operation
                    0,  // copy number
                    fCheckOverlaps);  // checking overlaps

  // always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // make sensitive the 2 beta detectors

  auto betaFront = new G4MultiFunctionalDetector("betaFront");         // the new geometry, actualized
  G4SDManager::GetSDMpointer()->AddNewDetector(betaFront);
  G4VPrimitiveScorer* primitivF = new G4PSEnergyDeposit("eDep");
  betaFront->RegisterPrimitive(primitivF);
  SetSensitiveDetector("betaFront", betaFront);

  auto betaBack = new G4MultiFunctionalDetector("betaBack");
  G4SDManager::GetSDMpointer()->AddNewDetector(betaBack);
  G4VPrimitiveScorer* primitivB = new G4PSEnergyDeposit("eDep");
  betaBack->RegisterPrimitive(primitivB);
  SetSensitiveDetector("betaBack", betaBack);

  // declare crystal as a MultiFunctionalDetector scorer
  //
  auto sample = new G4MultiFunctionalDetector("sample");
  G4SDManager::GetSDMpointer()->AddNewDetector(sample);
  G4VPrimitiveScorer* primitiv1 = new G4PSEnergyDeposit("eDep");
  sample->RegisterPrimitive(primitiv1);
  SetSensitiveDetector("sampleLV", sample);
}


//VITO
void DetectorConstruction::AddElement(G4String stlName, G4Material* material, G4VisAttributes* visAtt)
{
    G4String stlPath = "../stlNew/"+stlName+".stl";
    auto meshSolid = CADMesh::TessellatedMesh::FromSTL(stlPath);
    G4VSolid* solid = meshSolid->GetSolid();
    
    G4String solidName = stlName+"Solid";
    G4LogicalVolume* logVol = new G4LogicalVolume(solid, material, solidName);
	logVol->SetVisAttributes(visAtt);
	G4String physName = stlName+"Phys";
	// new G4PVPlacement(0, G4ThreeVector(0,0,0), logVol, physName,  worldLogic, 1,0);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B3
