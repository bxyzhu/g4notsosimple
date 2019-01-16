#include "G4SimpleDetectorConstruction.hh"

G4SimpleDetectorConstruction::G4SimpleDetectorConstruction(G4VPhysicalVolume *world)
: G4VUserDetectorConstruction()
{
  fWorld = world;
}


G4SimpleDetectorConstruction::~G4SimpleDetectorConstruction()
{
  delete fWorld;
}

G4VPhysicalVolume* G4SimpleDetectorConstruction::Construct()
{
  return fWorld;
}
