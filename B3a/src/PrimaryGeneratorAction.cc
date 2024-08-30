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
/// \file B3/B3a/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the B3::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4ChargedGeantino.hh"
#include "G4IonTable.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include <fstream>
#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnitsTable.hh"
#include "g4root_defs.hh"
#include "TH1F.h"

#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"

namespace B3
{

///////// using MONOENERGETIC BEAMS
PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  // default particle kinematic
  

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("e-");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.));
  fParticleGun->SetParticleEnergy(1 * MeV);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

G4ThreeVector PrimaryGeneratorAction::GenerateIsotropicDirection( G4double thetaMin,
                                                                 G4double thetaMax ,
                                                                 G4double phiMin,
                                                                 G4double phiMax)
{
   if(thetaMin < 0 || thetaMin > 2.*M_PI || thetaMax < 0 || thetaMax > 2.*M_PI )
   {
       std::cout << "angles not in the limit " << std::endl;
       return G4ThreeVector(0,0,0);
   }
   if(thetaMin >= thetaMax)
   {
       std::cout << " theta min has to be smaller than theta max" << std::endl;
       return G4ThreeVector(0,0,0);
   }
   
   G4double randomPhi = G4UniformRand()*(phiMax - phiMin) + phiMin; 
   G4double cosThetaMin = cos(thetaMin);
   G4double cosThetaMax = cos(thetaMax); 
   G4double randomCosTheta = G4UniformRand()*(cosThetaMin - cosThetaMax) + cosThetaMax;
   G4double randomTheta = acos(randomCosTheta);

   G4double x =  sin(randomTheta)*cos(randomPhi);
   G4double y = sin(randomTheta)*sin(randomPhi);
   G4double z = randomCosTheta;                                                      
   G4ThreeVector randDir = G4ThreeVector(x, y, z);
   return randDir;
}     

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  G4ParticleDefinition* particle = fParticleGun->GetParticleDefinition();
  if (particle == G4ChargedGeantino::ChargedGeantino()) {
    // fluorine
    G4int Z = 19, A = 47;
    G4double ionCharge = 0. * eplus;
    G4double excitEnergy = 0. * keV;

    G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z, A, excitEnergy);
    fParticleGun->SetParticleDefinition(ion);
    fParticleGun->SetParticleCharge(ionCharge);
  }

  // randomized position on a circular surface
  // the beta source should be located on the surface of the sample
  // isotropic emission
  G4double randomPhi = G4UniformRand()*2*M_PI;
   G4double r = G4UniformRand()*5*mm; // *10 mm for 2024, *5 mm for 2023
   G4double x = r*cos(randomPhi);
   G4double y = r*sin(randomPhi);
   G4double z = -0.5*0.5*mm; // 2 mm for 2024, 0.5 mm for 2023
   G4ThreeVector pos = G4ThreeVector(x, y, z);
   fParticleGun->SetParticlePosition(pos);
  G4ThreeVector direction = GenerateIsotropicDirection();
  fParticleGun->SetParticleMomentumDirection(direction);


  // create vertex
  //
  fParticleGun->GeneratePrimaryVertex(event);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/* /////////// USING GENERAL PARTICLE SOURCE
PrimaryGeneratorAction::PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(),
   fParticleGun(0), fParticleDefinition(nullptr)
{
   fParticleGun = new G4GeneralParticleSource();
   fParticleDefinition = fParticleGun->GetParticleDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  fParticleGun->GeneratePrimaryVertex(anEvent);
  fParticleDefinition = fParticleGun->GetParticleDefinition();
}
*/
 // namespace B3
}

