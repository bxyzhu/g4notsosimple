#ifndef G4SimpleDetectorConstruction_h
#define G4SimpleDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"

class G4VPhysicalVolume;

class G4SimpleDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    G4SimpleDetectorConstruction(G4VPhysicalVolume *world);
    virtual ~G4SimpleDetectorConstruction();
    virtual G4VPhysicalVolume* Construct();

  private:
    G4VPhysicalVolume *fWorld;
};

#endif
