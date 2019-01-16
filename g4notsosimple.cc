#include <iostream>
#include <vector>
#include <string>
#include <regex>

#include "G4GDMLParser.hh"
#include "G4SimpleDetectorConstruction.hh"
#include "G4SimplePhysicsList.hh"
#include "G4SimplePrimaryGeneratorAction.hh"
#include "G4SimpleSteppingAction.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "Randomize.hh"

using namespace std;

int main(int argc, char** argv)
{
  if(argc > 2) {
    cout << "Usage: " << argv[0] << " [macro]" << endl;
    return 1;
  }

  // Choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the simple run manager
  G4RunManager * runManager = new G4RunManager;

  // Set mandatory initialization classes
  //
  // Detector construction -- Load from a GDML file
  // Parse GDML file
  string filename = "geCounter.gdml";
  G4GDMLParser parser;
  parser.Read(filename);
  G4SimpleDetectorConstruction* detector = new G4SimpleDetectorConstruction(parser.GetWorldVolume());
  runManager->SetUserInitialization(detector);

  // Must generate Physics List before user actions
  runManager->SetUserInitialization(new G4SimplePhysicsList);

  // Set user action classes
  G4SimplePrimaryGeneratorAction *primary = new G4SimplePrimaryGeneratorAction();
  runManager->SetUserAction(primary);

  G4SimpleSteppingAction *stepping = new G4SimpleSteppingAction();
  runManager->SetUserAction(stepping);

  // Initialize G4 kernel
  runManager->Initialize();

  // Initialize Visualization
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  if (argc!=1) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
      UImanager->ApplyCommand(command+fileName);
  }
  else {
    // interactive mode : define UI session
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
  }

  return 0;
}
