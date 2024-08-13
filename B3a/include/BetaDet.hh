// $Id: BetaDet.hh 03.02.2023 A. Fijalkowska $
//
/// \file BetaDet.hh
/// \brief Class for AluCham-BlindFlange.stl and AluCham-Tube.stl
//
//
#ifndef BetaDet_H
#define BetaDet_H 1
#include "G4String.hh"
#include "G4LogicalVolume.hh"
#include "G4MultiFunctionalDetector.hh"
#include "GeomElement.hh"

class BetaDet : public GeomElement
{
  public:

    BetaDet(G4String prefix); 
    virtual void Place(G4RotationMatrix *pRot, 
                        G4ThreeVector &tlate, 
                        const G4String &pName, 
                        G4LogicalVolume *pMotherLogical,  
                        G4int pCopyNo = 0);
    void MakeSensitiveDet();
    
  private:
    G4Material* MakeEJ200();
    G4VisAttributes* MakeBetaDetVis();
    GeomElement* front;
    GeomElement* back;
    G4MultiFunctionalDetector* betaFrontMultiDet;
    G4MultiFunctionalDetector* betaBackMultiDet;
};

#endif
