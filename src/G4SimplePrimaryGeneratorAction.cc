#include "G4SimplePrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"

G4SimplePrimaryGeneratorAction::G4SimplePrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(), fParticleGun(0)
{
  fParticleGun = new G4GeneralParticleSource;
}

G4SimplePrimaryGeneratorAction::~G4SimplePrimaryGeneratorAction()
{
  delete fParticleGun;
}

void G4SimplePrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  fParticleGun->GeneratePrimaryVertex(event);
}
