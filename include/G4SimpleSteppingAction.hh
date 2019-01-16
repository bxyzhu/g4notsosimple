#ifndef G4SimpleSteppingAction_h
#define G4SimpleSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4Run.hh"
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "g4root.hh"
#include "g4xml.hh"
#include "g4csv.hh"
#include "g4hdf5.hh"
#include "G4VPhysicalVolume.hh"

#include <iostream>
#include <vector>
#include <string>
#include <regex>

class G4Step;
class G4UIcommand;
class G4VPhysicalVolume;

class G4SimpleSteppingAction : public G4UserSteppingAction, public G4UImessenger
{
  public:
    G4SimpleSteppingAction();
    virtual ~G4SimpleSteppingAction();

    // method from the base class
    virtual void UserSteppingAction(const G4Step *step);

    G4VAnalysisManager* GetAnalysisManager();
    void SetNewValue(G4UIcommand *command, G4String newValues);
    void ResetVars();
    void WriteRow(G4VAnalysisManager* man);

  protected:
    G4UIcommand* fVolIDCmd;
    G4UIcmdWithAString* fOutputFormatCmd;
    G4UIcmdWithAString* fOutputOptionCmd;
    G4UIcmdWithABool* fRecordAllStepsCmd;

    enum EFormat { kCsv, kXml, kRoot, kHdf5 };
    EFormat fFormat;
    enum EOption { kStepWise, kEventWise };
    EOption fOption;
    bool fRecordAllSteps;

    std::vector<std::regex> fPatterns;
    std::vector<int> fPatternIDs;

    G4int fNEvents;
    G4int fEventNumber;
    std::vector<G4int> fPID;
    std::vector<G4int> fTrackID;
    std::vector<G4int> fParentID;
    std::vector<G4int> fStepNumber;
    std::vector<G4double> fKE;
    std::vector<G4double> fEDep;
    std::vector<G4double> fX;
    std::vector<G4double> fY;
    std::vector<G4double> fZ;
    std::vector<G4double> fLX;
    std::vector<G4double> fLY;
    std::vector<G4double> fLZ;
    std::vector<G4double> fT;
    std::vector<G4int> fVolID;
    std::vector<G4int> fIRep;
    std::map<G4VPhysicalVolume*, int> fVolIDMap;

};

#endif
