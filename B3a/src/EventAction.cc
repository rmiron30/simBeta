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
/// \file B3/B3a/src/EventAction.cc
/// \brief Implementation of the B3a::EventAction class

#include "EventAction.hh"

#include "RunAction.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4THitsMap.hh"
#include "G4AnalysisManager.hh"
// #include "G4UnitsTable.hh"

namespace B3a
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAction) : fRunAction(runAction) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* /*evt*/) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* evt)
{
  // Hits collections
  //
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  if (!HCE) return;

  // Get hits collections IDs
  if (fCollID_betaBack < 0) {
    G4SDManager* SDMan = G4SDManager::GetSDMpointer();
    fCollID_betaFront = SDMan->GetCollectionID("betaFront/eDep"); // change to betaF and betaB when using the new geometry
    fCollID_betaBack = SDMan->GetCollectionID("betaBack/eDep");
    fCollID_sample = SDMan->GetCollectionID("sample/eDep");
    // fCollID_cryst = SDMan->GetCollectionID("crystal/edep");
  }

  // Energy in crystals : identify 'good events'
  //
  // const G4double eThreshold = 500 * keV;
  G4int nbOfFired = 0;

  auto evtMap = static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCollID_sample));

  // for (auto& mapElement : (*evtMap->GetMap())) {
  //   auto edep = *(mapElement.second);
  //   if (edep > 0) 
  //     energyDeposit = *((*eventMap2)[0]);	 
  // }
  // if (nbOfFired == 2) fRunAction->CountEvent();

  // Energy in betaFront

    G4THitsMap<G4double>* eventMap2 = (G4THitsMap<G4double>*)(HCE->GetHC(fCollID_sample));
    // G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    double energyDeposit = 0;

  if((*eventMap2)[0] != 0)
	{
		energyDeposit = *((*eventMap2)[0]);	 
	}
	// if(eventMap2->entries() !=0)
	// 	std::cout << "!!!" << "sample" << " " << fCollID_sample << " " << energyDeposit/keV << " keV  " << eventMap2->entries() << std::endl;

  // // Energy in betaBAck

  // nbOfFired = 0;

  // evtMap = static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCollID_betaBack));

  // for (auto& mapElement : (*evtMap->GetMap())) {
  //   auto edep = *(mapElement.second);
  //   if (edep > 0.000001) ++nbOfFired;
  //   // auto copyNb  = mapElement.first;
  //   // G4cout << "\n  cryst" << copyNb << ": " << edep/keV << " keV ";
  // }
  // if (nbOfFired > 0) fRunAction->CountEvent();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B3a
