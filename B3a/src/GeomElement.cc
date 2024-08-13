// $Id: GeomElement.cc A. Fijalkowska, 02.02.2023
// Generic class for geometry elements
//
#include "G4PVPlacement.hh" 
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh" 
#include "globals.hh"
#include "CADMesh.hh"

#include "GeomElement.hh"

GeomElement::GeomElement(G4String prefix)
{
    stlPrefix = prefix;
    logVol=0L;
}


void GeomElement::AddElement(G4String stlName, G4Material* material, G4VisAttributes* visAtt)
{
    G4String stlPath = stlPrefix+stlName+".stl";
    auto meshSolid = CADMesh::TessellatedMesh::FromSTL(stlPath);
    G4VSolid* solid = meshSolid->GetSolid();
    
    G4String solidName = stlName+"Solid";
    logVol = new G4LogicalVolume(solid, material, solidName);
	logVol->SetVisAttributes(visAtt);

}

void GeomElement::AddElement(G4String stlName, G4Material* material)
{
    G4String stlPath = stlPrefix+stlName+".stl";
    auto meshSolid = CADMesh::TessellatedMesh::FromSTL(stlPath);
    G4VSolid* solid = meshSolid->GetSolid();
    
    G4String solidName = stlName+"Solid";
    logVol = new G4LogicalVolume(solid, material, solidName);
}

void GeomElement::Place(G4RotationMatrix *pRot, 
                        G4ThreeVector &tlate, 
                        const G4String &pName, 
                        G4LogicalVolume *pMotherLogical,  
                        G4int pCopyNo)
{
    new G4PVPlacement(pRot, tlate, logVol, pName,  pMotherLogical, 0,pCopyNo);
}







