// $Id: GeomElement.hh A. Fijalkowska, 02.02.2023
// Generic class for geometry elements
//
#ifndef GeomElement_H
#define GeomElement_H 1
#include "G4String.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh" 

class GeomElement
{
  public:
    GeomElement(G4String prefix);
    void AddElement(G4String stlName, G4Material* material, G4VisAttributes* visAtt);
    void AddElement(G4String stlName, G4Material* material);
    virtual void Place(G4RotationMatrix *pRot, 
                        G4ThreeVector &tlate, 
                        const G4String &pName, 
                        G4LogicalVolume *pMotherLogical,  
                        G4int pCopyNo);
   G4LogicalVolume* GetLogical() { return logVol;}                  
  private:
    G4LogicalVolume* logVol;
    G4String stlPrefix;
};

#endif
