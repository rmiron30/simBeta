// $Id: BetaDet.cc 03.02.2023 A. Fijalkowska $
//
/// \file BetaDet.cc
/// Beta-Det-H-Back.stl Beta-Det-H-Front.stl
//
// 

#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh" 
#include "G4Element.hh"  
#include "G4ThreeVector.hh" 
#include "globals.hh"
#include "CADMesh.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4SDManager.hh"
#include "G4PSEnergyDeposit.hh"
#include "BetaDet.hh"

BetaDet::BetaDet(G4String prefix) : GeomElement(prefix)
{
    G4Material* ej200 = MakeEJ200();
    G4VisAttributes* betaDetVis = MakeBetaDetVis();
    back = new GeomElement(prefix);
    back->AddElement("Beta-Det-H-Back", ej200, betaDetVis);
    front = new GeomElement(prefix);
    front->AddElement("Beta-Det-H-Front", ej200, betaDetVis);
    
    
}

G4Material* BetaDet::MakeEJ200()
{
    G4MaterialTable* materialTable = G4Material::GetMaterialTable();
    for(int i=0; i!=materialTable->size(); ++i)
    {
        if(materialTable->at(i)->GetName () == "ej200")
            return materialTable->at(i);
    }
	G4double atomicMass;
	G4double z;
	G4double density;
	G4int numberElements;
  

	G4Element* H  = new G4Element("Hydrogen", "H",  z=1.,  atomicMass =1.008 *g/mole );
	G4Element* C  = new G4Element("Carbon",   "C",  z=6.,  atomicMass = 12.01*g/mole );
	G4Material*	ej200 = new G4Material( "ej200", density= 1.023 *g/cm3, numberElements=2 );
	ej200->AddElement( C,  469 );
	ej200->AddElement( H,  517);	
    return ej200;
}

G4VisAttributes* BetaDet::MakeBetaDetVis()
{
   G4VisAttributes* BetaDetVis = new G4VisAttributes( G4Colour(0.8,0.2,0.0));
   BetaDetVis->SetForceAuxEdgeVisible(true);
   BetaDetVis->SetForceSolid(true);
   return BetaDetVis;
}

void BetaDet::Place(G4RotationMatrix *pRot, 
                        G4ThreeVector &tlate, 
                        const G4String &pName, 
                        G4LogicalVolume *pMotherLogical,  
                        G4int pCopyNo)
{
    front->Place(pRot, tlate, pName+"Front", pMotherLogical, pCopyNo);
    back->Place(pRot, tlate, pName+"Back", pMotherLogical, pCopyNo);

}

void BetaDet::MakeSensitiveDet()
{ 
    //         auto cryst = new G4MultiFunctionalDetector("crystal");
    //         G4SDManager::GetSDMpointer()->AddNewDetector(cryst);
    //         G4VPrimitiveScorer* primitiv1 = new G4PSEnergyDeposit("edep");
    //         cryst->RegisterPrimitive(primitiv1);
    //         SetSensitiveDetector("CrystalLV", cryst);
   {
       betaFrontMultiDet = new G4MultiFunctionalDetector("betaFront");
       G4VPrimitiveScorer* primitiveFront = new G4PSEnergyDeposit("eDep");
       betaFrontMultiDet->RegisterPrimitive(primitiveFront);
       G4LogicalVolume* betaFrontLog=front->GetLogical();
       betaFrontLog->SetSensitiveDetector(betaFrontMultiDet);
       G4SDManager::GetSDMpointer()->AddNewDetector(betaFrontMultiDet);
    }
    {
       betaBackMultiDet = new G4MultiFunctionalDetector("betaBack");
       G4VPrimitiveScorer* primitiveBack = new G4PSEnergyDeposit("eDep");
       betaBackMultiDet->RegisterPrimitive(primitiveBack);
       G4LogicalVolume* betaBackLog=back->GetLogical();
       betaBackLog->SetSensitiveDetector(betaBackMultiDet);
       G4SDManager::GetSDMpointer()->AddNewDetector(betaBackMultiDet);
    }  
    // original
    // {
    //    betaFrontMultiDet = new G4MultiFunctionalDetector("/betaFront");
    //    G4VPrimitiveScorer* primitiveFront = new G4PSEnergyDeposit("eDep",0);
    //    betaFrontMultiDet->RegisterPrimitive(primitiveFront);
    //    G4LogicalVolume* betaFrontLog=front->GetLogical();
    //    betaFrontLog->SetSensitiveDetector(betaFrontMultiDet);
    //    G4SDManager::GetSDMpointer()->AddNewDetector(betaFrontMultiDet);
    
    // }
    // {
    //    betaBackMultiDet = new G4MultiFunctionalDetector("/betaBack");
    //    G4VPrimitiveScorer* primitiveBack = new G4PSEnergyDeposit("eDep",0);
    //    betaBackMultiDet->RegisterPrimitive(primitiveBack);
    //    G4LogicalVolume* betaBackLog=back->GetLogical();
    //    betaBackLog->SetSensitiveDetector(betaBackMultiDet);
    //    G4SDManager::GetSDMpointer()->AddNewDetector(betaBackMultiDet);
    // }  



}

