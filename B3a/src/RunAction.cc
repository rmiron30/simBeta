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
/// \file B3/B3a/src/RunAction.cc
/// \brief Implementation of the B3a::RunAction class

#include "RunAction.hh"

#include "PrimaryGeneratorAction.hh"

#include "G4AccumulableManager.hh"
#include "G4ParticleGun.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include <filesystem>
#include "G4AnalysisManager.hh"

using namespace B3;

namespace B3a
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
{
  // add new units for dose
  //
  const G4double milligray = 1.e-3 * gray;
  const G4double microgray = 1.e-6 * gray;
  const G4double nanogray = 1.e-9 * gray;
  const G4double picogray = 1.e-12 * gray;

  new G4UnitDefinition("milligray", "milliGy", "Dose", milligray);
  new G4UnitDefinition("microgray", "microGy", "Dose", microgray);
  new G4UnitDefinition("nanogray", "nanoGy", "Dose", nanogray);
  new G4UnitDefinition("picogray", "picoGy", "Dose", picogray);

  // Register accumulable to the accumulable manager
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->RegisterAccumulable(fGoodEvents);
  accumulableManager->RegisterAccumulable(fSumDose);

  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetDefaultFileType("root");
  analysisManager->SetVerboseLevel(2);
  analysisManager->SetFirstHistoId(1);
  analysisManager->CreateH1("back", "eDep - back", 1000, 0, 5, "MeV");
  analysisManager->CreateH1("front", "eDep - front", 1000, 0, 5, "MeV");
  analysisManager->CreateH1("sample", "eDep - sample", 1000, 0, 5, "MeV");
  // analysisManager->CreateH2("eDep2D","Energy Deposition", 1000, -10 * cm, 10 * cm, 1000, -10 * cm, 10 * cm); // 2D energy deposition
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* run)
{
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;

  // reset accumulables to their initial values
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();

  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
   if (isMaster)
    G4Random::showEngineStatus();

  // Get analysis manager
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  analysisManager->Reset();
  // Open an output file
  //
  G4String fileName = "scoring";
  analysisManager->OpenFile(fileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  // Merge accumulables
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Merge();

  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

  // Run conditions
  //  note: There is no primary generator action object for "master"
  //        run manager for multi-threaded mode.
  const auto generatorAction = static_cast<const PrimaryGeneratorAction*>(
    G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String partName;
  if (generatorAction) {
    G4ParticleDefinition* particle = generatorAction->GetParticleGun()->GetParticleDefinition();
    partName = particle->GetParticleName();
  }

  // if (generatorAction) {
  //   G4ParticleDefinition* particle = generatorAction->GetParticleDefinition(); /// GPS
  //   if (particle) {
  //     partName = particle->GetParticleName();
  //   }
  // }

  char timeDir[80];

  // Print results
  //
  if (IsMaster()) {

    G4Random::showEngineStatus();

    // the current time

    std::time_t t = std::time(nullptr);

    std::strftime(timeDir, 80, "simBeta-%Y-%m-%d-%H-%M-%S", std::localtime(&t));

    // save files

    std::filesystem::create_directory(timeDir);

    G4cout << G4endl << "--------------------End of Global Run-----------------------" << G4endl
           << "  The run was " << nofEvents << " events ";
  }
  else {
    G4cout << G4endl << "--------------------End of Local Run------------------------" << G4endl
           << "  The run was " << nofEvents << " " << partName;
  }

  analysisManager->Write();
  analysisManager->CloseFile(false);

  if (IsMaster()){
      const auto copyOptions = std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive;
      std::filesystem::copy("scoring.root", timeDir + std::string("/scoring.root"), copyOptions);
      std::filesystem::copy("monobeta.in", timeDir + std::string("/monobeta.in"), copyOptions);
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B3a
