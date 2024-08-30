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
/// \file B3/B3a/include/PrimaryGeneratorAction.hh
/// \brief Definition of the B3::PrimaryGeneratorAction class

#ifndef B3PrimaryGeneratorAction_h
#define B3PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "globals.hh"

// VITO
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include <vector>
#include "g4root_defs.hh"
#include "TH1F.h"


class G4ParticleGun;
class G4Event;

namespace B3
{

  class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
    public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override;

    void GeneratePrimaries(G4Event*) override;

    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

  private:
    G4ParticleGun* fParticleGun = nullptr;
   
  G4ThreeVector GenerateIsotropicDirection( G4double thetaMin = 0,
                                              G4double thetaMax = M_PI,
                                              G4double phiMin = 0,
                                              G4double phiMax = 2.*M_PI);
  
};

/* 

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
   ~PrimaryGeneratorAction();
   G4ParticleDefinition* GetParticleDefinition() const { return fParticleDefinition; }


    virtual void GeneratePrimaries(G4Event*);

  private:
    G4GeneralParticleSource* fParticleGun;

    // Initialize the particle definition
   G4ParticleDefinition* fParticleDefinition; 
};
*/
}  // namespace B3

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
