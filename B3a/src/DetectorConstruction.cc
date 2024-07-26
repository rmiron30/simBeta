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

  G4Element* O = man->FindOrBuildElement("O", isotopes);
  G4Element* Si = man->FindOrBuildElement("Si", isotopes);
  G4Element* Lu = man->FindOrBuildElement("Lu", isotopes);

  auto LSO = new G4Material("Lu2SiO5", 7.4 * g / cm3, 3);
  LSO->AddElement(Lu, 2);
  LSO->AddElement(Si, 1);
  LSO->AddElement(O, 5);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // BetaDetectors from VitoGeant
    // G4VPhysicalVolume* worldPhys = ConstructWorld();
  G4String prefix = "/home/ilaria/Simulations/isolde_geant4_elements/Main-elements/";
  G4ThreeVector pos(0,0,0);

  BetaDet* betaDet = new BetaDet(prefix);


  // Gamma detector Parameters
  //
  G4double cryst_dX = 6 * cm, cryst_dY = 6 * cm, cryst_dZ = 3 * cm;
  G4int nb_cryst = 32;
  G4int nb_rings = 0;
  //
  G4double dPhi = twopi / nb_cryst, half_dPhi = 0.5 * dPhi;
  G4double cosdPhi = std::cos(half_dPhi);
  G4double tandPhi = std::tan(half_dPhi);
  //
  G4double ring_R1 = 0.5 * cryst_dY / tandPhi;
  G4double ring_R2 = (ring_R1 + cryst_dZ) / cosdPhi;
  //
  // G4double detector_dZ = nb_rings * cryst_dX;
  G4double detector_dZ = 9 * cryst_dX;
  //
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* default_mat = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* cryst_mat = nist->FindOrBuildMaterial("Lu2SiO5");

  //
  // World
  //
  G4double world_sizeXY = 2.4 * ring_R2;
  G4double world_sizeZ = 1.2 * detector_dZ;

  auto solidWorld =
    new G4Box("World",  // its name
              0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);  // its size

  auto logicWorld = new G4LogicalVolume(solidWorld,  // its solid
                                        default_mat,  // its material
                                        "World");  // its name

  auto physWorld = new G4PVPlacement(nullptr,  // no rotation
                                     G4ThreeVector(),  // at (0,0,0)
                                     logicWorld,  // its logical volume
                                     "World",  // its name
                                     nullptr,  // its mother  volume
                                     false,  // no boolean operation
                                     0,  // copy number
                                     fCheckOverlaps);  // checking overlaps

  betaDet->Place(0, pos, "BetaDet", logicWorld);
      
  betaDet->MakeSensitiveDet();

  //
  // ring
  //
  auto solidRing = new G4Tubs("Ring", ring_R1, ring_R2, 0.5 * cryst_dX, 0., twopi);

  auto logicRing = new G4LogicalVolume(solidRing,  // its solid
                                       default_mat,  // its material
                                       "Ring");  // its name

  //
  // define crystal
  //
  G4double gap = 0.5 * mm;  // a gap for wrapping
  G4double dX = cryst_dX - gap, dY = cryst_dY - gap;
  auto solidCryst = new G4Box("crystal", dX / 2, dY / 2, cryst_dZ / 2);

  auto logicCryst = new G4LogicalVolume(solidCryst,  // its solid
                                        cryst_mat,  // its material
                                        "CrystalLV");  // its name

  // place crystals within a ring
  //
  for (G4int icrys = 0; icrys < nb_cryst; icrys++) {
    G4double phi = icrys * dPhi;
    G4RotationMatrix rotm = G4RotationMatrix();
    rotm.rotateY(90 * deg);
    rotm.rotateZ(phi);
    G4ThreeVector uz = G4ThreeVector(std::cos(phi), std::sin(phi), 0.);
    G4ThreeVector position = (ring_R1 + 0.5 * cryst_dZ) * uz;
    G4Transform3D transform = G4Transform3D(rotm, position);

    new G4PVPlacement(transform,  // rotation,position
                      logicCryst,  // its logical volume
                      "crystal",  // its name
                      logicRing,  // its mother  volume
                      false,  // no boolean operation
                      icrys,  // copy number
                      fCheckOverlaps);  // checking overlaps
  }

  //
  // full detector
  //
  auto solidDetector = new G4Tubs("Detector", ring_R1, ring_R2, 0.5 * detector_dZ, 0., twopi);

  auto logicDetector = new G4LogicalVolume(solidDetector,  // its solid
                                           default_mat,  // its material
                                           "Detector");  // its name

  //
  // place rings within detector
  //
  G4double OG = -0.5 * (detector_dZ + cryst_dX);
  for (G4int iring = 0; iring < nb_rings; iring++) {
    OG += cryst_dX;
    new G4PVPlacement(nullptr,  // no rotation
                      G4ThreeVector(0, 0, OG),  // position
                      logicRing,  // its logical volume
                      "ring",  // its name
                      logicDetector,  // its mother  volume
                      false,  // no boolean operation
                      iring,  // copy number
                      fCheckOverlaps);  // checking overlaps
  }

  //
  // place detector in world
  //
  new G4PVPlacement(nullptr,  // no rotation
                    G4ThreeVector(),  // at (0,0,0)
                    logicDetector,  // its logical volume
                    "Detector",  // its name
                    logicWorld,  // its mother  volume
                    false,  // no boolean operation
                    0,  // copy number
                    fCheckOverlaps);  // checking overlaps

  //
  // implanter crystal, thin disk, 2 mm thickness
  //
  G4double sample_radius = 2 * cm;
  G4double sample_dZ = 1 * mm;
  G4double density;
	G4int numberElements;  
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

  
  // place sample in world
  
  new G4PVPlacement(nullptr,  // no rotation
                    G4ThreeVector(),  // at (0,0,0)
                    logicsample,  // its logical volume
                    "sample",  // its name
                    logicWorld,  // its mother  volume
                    false,  // no boolean operation
                    0,  // copy number
                    fCheckOverlaps);  // checking overlaps

  // Visualization attributes
  //
  logicRing->SetVisAttributes(G4VisAttributes::GetInvisible());
  logicDetector->SetVisAttributes(G4VisAttributes::GetInvisible());

  // Print materials
  // G4cout << *(G4Material::GetMaterialTable()) << G4endl;

  // always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

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
