#include "G4SimpleSteppingAction.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"

using namespace std;

G4SimpleSteppingAction::G4SimpleSteppingAction()
: G4UserSteppingAction(), fNEvents(0), fEventNumber(0)
{
  ResetVars();

  fVolIDCmd = new G4UIcommand("/g4simple/setVolID", this);
  fVolIDCmd->SetParameter(new G4UIparameter("pattern", 's', false));
  fVolIDCmd->SetParameter(new G4UIparameter("id", 'i', false));
  fVolIDCmd->SetGuidance("Volumes with name matching [pattern] will be given volume ID [id]");

  fOutputFormatCmd = new G4UIcmdWithAString("/g4simple/setOutputFormat", this);
  string candidates = "csv xml root hdf5";

  fOutputFormatCmd->SetCandidates(candidates.c_str());
  fOutputFormatCmd->SetGuidance("Set output format (default csv)");
  // Default output
  fOutputFormatCmd->SetDefaultValue("csv");

  fOutputOptionCmd = new G4UIcmdWithAString("/g4simple/setOutputOption", this);
  candidates = "stepwise eventwise";
  fOutputOptionCmd->SetCandidates(candidates.c_str());
  fOutputOptionCmd->SetDefaultValue("stepwise");
  fOutputOptionCmd->SetGuidance("Set output option (default stepwise):");
  fOutputOptionCmd->SetGuidance("  stepwise: one row per step");
  fOutputOptionCmd->SetGuidance("  eventwise: one row per event");

  fRecordAllStepsCmd = new G4UIcmdWithABool("/g4simple/recordAllSteps", this);
  fRecordAllStepsCmd->SetParameterName("recordAllSteps", true);
  fRecordAllStepsCmd->SetDefaultValue(true);
  fRecordAllStepsCmd->SetGuidance("Write out every single step, not just those in sensitive volumes. (default true)");
}


G4SimpleSteppingAction::~G4SimpleSteppingAction()
{
  G4VAnalysisManager* man = GetAnalysisManager();
  if(man->IsOpenFile())
  {
    if(fOption == kEventWise && fPID.size()>0) WriteRow(man);
    man->Write();
    man->CloseFile();
  }
  delete man;
  delete fVolIDCmd;
  delete fOutputFormatCmd;
  delete fOutputOptionCmd;
  delete fRecordAllStepsCmd;
}


void G4SimpleSteppingAction::UserSteppingAction(const G4Step *step)
{
  G4VAnalysisManager* man = GetAnalysisManager();

  if(!man->IsOpenFile())
  {
    // need to create the ntuple before opening the file in order to avoid
    // writing error in csv, xml, and hdf5
    man->CreateNtuple("g4sntuple", "steps data");
    man->CreateNtupleIColumn("nEvents");
    man->CreateNtupleIColumn("event");
    if(fOption == kEventWise)
    {
      man->CreateNtupleIColumn("pid", fPID);
      man->CreateNtupleIColumn("trackID", fTrackID);
      man->CreateNtupleIColumn("parentID", fParentID);
      man->CreateNtupleIColumn("step", fStepNumber);
      man->CreateNtupleDColumn("KE", fKE);
      man->CreateNtupleDColumn("Edep", fEDep);
      man->CreateNtupleDColumn("x", fX);
      man->CreateNtupleDColumn("y", fY);
      man->CreateNtupleDColumn("z", fZ);
      man->CreateNtupleDColumn("lx", fLX);
      man->CreateNtupleDColumn("ly", fLY);
      man->CreateNtupleDColumn("lz", fLZ);
      man->CreateNtupleDColumn("t", fT);
      man->CreateNtupleIColumn("volID", fVolID);
      man->CreateNtupleIColumn("iRep", fIRep);
    }
    else if(fOption == kStepWise)
    {
      man->CreateNtupleIColumn("pid");
      man->CreateNtupleIColumn("trackID");
      man->CreateNtupleIColumn("parentID");
      man->CreateNtupleIColumn("step");
      man->CreateNtupleDColumn("KE");
      man->CreateNtupleDColumn("Edep");
      man->CreateNtupleDColumn("x");
      man->CreateNtupleDColumn("y");
      man->CreateNtupleDColumn("z");
      man->CreateNtupleDColumn("lx");
      man->CreateNtupleDColumn("ly");
      man->CreateNtupleDColumn("lz");
      man->CreateNtupleDColumn("t");
      man->CreateNtupleIColumn("volID");
      man->CreateNtupleIColumn("iRep");
    }
    else
    {
      G4cout << "ERROR: Unknown output option " << fOption << G4endl;
      return;
    }
    man->FinishNtuple();

    // look for filename set by macro command: /analysis/setFileName [name]
    if(man->GetFileName() == "") man->SetFileName("g4notsosimpleout");
    G4cout << "Opening file " << man->GetFileName() << G4endl;
    man->OpenFile();

    ResetVars();
    fNEvents = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed();
    fVolIDMap.clear();
  }
  else
  {
    fEventNumber = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    static G4int lastEventID = fEventNumber;
    if(fEventNumber != lastEventID) {
      if(fOption == kEventWise && fPID.size()>0) WriteRow(man);
      ResetVars();
      lastEventID = fEventNumber;
    }
  }

  // post-step point will always work: only need to use the pre-step point
  // on the first step, for which the pre-step volume is always the same as
  // the post-step volume
  G4VPhysicalVolume* vpv = step->GetPostStepPoint()->GetPhysicalVolume();
  G4int id = fVolIDMap[vpv];
  if(id == 0 && fPatterns.size() > 0) {
    string name = (vpv == NULL) ? "NULL" : vpv->GetName();
    for(size_t i=0; i<fPatterns.size(); i++) {
      if(regex_match(name, fPatterns[i])) {
        id = fPatternIDs[i];
        break;
      }
    }
    if(id == 0 && !fRecordAllSteps) id = -1;
    fVolIDMap[vpv] = id;
  }

  // always record primary event info from pre-step of first step
  // if recording all steps, do this block to record prestep info
  if(fVolID.size() == 0 || (fRecordAllSteps && step->GetTrack()->GetCurrentStepNumber() == 1)) {
    fVolID.push_back(id == -1 ? 0 : id);
    fPID.push_back(step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
    fTrackID.push_back(step->GetTrack()->GetTrackID());
    fParentID.push_back(step->GetTrack()->GetParentID());
    fStepNumber.push_back(step->GetTrack()->GetCurrentStepNumber());
    fKE.push_back(step->GetPreStepPoint()->GetKineticEnergy());
    fEDep.push_back(0);
    G4ThreeVector pos = step->GetPreStepPoint()->GetPosition();
    G4TouchableHandle vol = step->GetPreStepPoint()->GetTouchableHandle();
    G4ThreeVector lPos = vol->GetHistory()->GetTopTransform().TransformPoint(pos);
    fX.push_back(pos.x());
    fY.push_back(pos.y());
    fZ.push_back(pos.z());
    fLX.push_back(lPos.x());
    fLY.push_back(lPos.y());
    fLZ.push_back(lPos.z());
    fT.push_back(step->GetPreStepPoint()->GetGlobalTime());
    fIRep.push_back(vol->GetReplicaNumber());

    if(fOption == kStepWise) WriteRow(man);
  }

  // If not in a sensitive volume, get out of here.
  if(id == -1) return;

  // Don't write Edep=0 steps (unless desired)
  if(!fRecordAllSteps && step->GetTotalEnergyDeposit() == 0) return;

  // Now record post-step info
  fVolID.push_back(id);
  fPID.push_back(step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
  fTrackID.push_back(step->GetTrack()->GetTrackID());
  fParentID.push_back(step->GetTrack()->GetParentID());
  fStepNumber.push_back(step->GetTrack()->GetCurrentStepNumber());
  fKE.push_back(step->GetTrack()->GetKineticEnergy());
  fEDep.push_back(step->GetTotalEnergyDeposit());
  G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
  G4TouchableHandle vol = step->GetPostStepPoint()->GetTouchableHandle();
  G4ThreeVector lPos = vol->GetHistory()->GetTopTransform().TransformPoint(pos);
  fX.push_back(pos.x());
  fY.push_back(pos.y());
  fZ.push_back(pos.z());
  fLX.push_back(lPos.x());
  fLY.push_back(lPos.y());
  fLZ.push_back(lPos.z());
  fT.push_back(step->GetPostStepPoint()->GetGlobalTime());
  fIRep.push_back(vol->GetReplicaNumber());

  if(fOption == kStepWise) WriteRow(man);
}


G4VAnalysisManager* G4SimpleSteppingAction::GetAnalysisManager()
{
  if(fFormat == kCsv) return G4Csv::G4AnalysisManager::Instance();
  if(fFormat == kXml) return G4Xml::G4AnalysisManager::Instance();
  if(fFormat == kRoot) return G4Root::G4AnalysisManager::Instance();
  if(fFormat == kHdf5) return G4Hdf5::G4AnalysisManager::Instance();
  G4cout << "Error: invalid format " << fFormat << G4endl;
  return NULL;
}


void G4SimpleSteppingAction::SetNewValue(G4UIcommand *command, G4String newValues)
{
  if(command == fVolIDCmd) {
    istringstream iss(newValues);
    string pattern;
    int id;
    iss >> pattern >> id;
    if(id == 0 || id == -1) {
      G4cout << "Pattern " << pattern << ": Can't use ID = " << id << G4endl;
    }
    else {
      fPatterns.push_back(regex(pattern));
      fPatternIDs.push_back(id);
    }
  }
  if(command == fOutputFormatCmd) {
    // also set recommended options.
    // override option by subsequent call to /g4simple/setOutputOption
    if(newValues == "csv") {
      fFormat = kCsv;
      fOption = kStepWise;
    }
    if(newValues == "xml") {
      fFormat = kXml;
      fOption = kEventWise;
    }
    if(newValues == "root") {
      fFormat = kRoot;
      fOption = kEventWise;
    }
    if(newValues == "hdf5") {
      fFormat = kHdf5;
      fOption = kStepWise;
    }
  }
  if(command == fOutputOptionCmd) {
    if(newValues == "stepwise") fOption = kStepWise;
    if(newValues == "eventwise") fOption = kEventWise;
  }
  if(command == fRecordAllStepsCmd) {
    fRecordAllSteps = fRecordAllStepsCmd->GetNewBoolValue(newValues);
  }
}

void G4SimpleSteppingAction::ResetVars()
{
  fPID.clear();
  fTrackID.clear();
  fParentID.clear();
  fStepNumber.clear();
  fKE.clear();
  fEDep.clear();
  fX.clear();
  fY.clear();
  fZ.clear();
  fLX.clear();
  fLY.clear();
  fLZ.clear();
  fT.clear();
  fVolID.clear();
  fIRep.clear();
}

void G4SimpleSteppingAction::WriteRow(G4VAnalysisManager* man)
{
  man->FillNtupleIColumn(0, fNEvents);
  man->FillNtupleIColumn(1, fEventNumber);
  int row = 2;
  if(fOption == kStepWise) {
    size_t i = fPID.size()-1;
    man->FillNtupleIColumn(row++, fPID[i]);
    man->FillNtupleIColumn(row++, fTrackID[i]);
    man->FillNtupleIColumn(row++, fParentID[i]);
    man->FillNtupleIColumn(row++, fStepNumber[i]);
    man->FillNtupleDColumn(row++, fKE[i]);
    man->FillNtupleDColumn(row++, fEDep[i]);
    man->FillNtupleDColumn(row++, fX[i]);
    man->FillNtupleDColumn(row++, fY[i]);
    man->FillNtupleDColumn(row++, fZ[i]);
    man->FillNtupleDColumn(row++, fLX[i]);
    man->FillNtupleDColumn(row++, fLY[i]);
    man->FillNtupleDColumn(row++, fLZ[i]);
    man->FillNtupleDColumn(row++, fT[i]);
    man->FillNtupleIColumn(row++, fVolID[i]);
    man->FillNtupleIColumn(row++, fIRep[i]);
  }
  // for event-wise, manager copies data from vectors over
  // automatically in the next line
  man->AddNtupleRow();
}
