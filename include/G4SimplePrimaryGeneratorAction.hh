#ifndef G4SimplePrimaryGeneratorAction_h
#define G4SimplePrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"

class G4Event;
class G4GeneralParticleSource;

class G4SimplePrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    G4SimplePrimaryGeneratorAction();
    virtual ~G4SimplePrimaryGeneratorAction();

    // method from the base class
    void GeneratePrimaries(G4Event* event);

    // method to access particle gun
    const G4GeneralParticleSource* GetParticleGun() const { return fParticleGun; }

  private:
    G4GeneralParticleSource *fParticleGun;
};

#endif
