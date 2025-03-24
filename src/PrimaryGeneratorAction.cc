#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "Randomize.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

namespace SimCalModule
{
    PrimaryGeneratorAction::PrimaryGeneratorAction()
        : G4VUserPrimaryGeneratorAction()
    {
        G4int nParticle = 1;
        fGParticleSource = new G4GeneralParticleSource();
        // fParticleGun = new G4ParticleGun(nParticle);
        // fGunMessenger = new PrimaryGeneratorMessenger(this);
        SetDefaultKinematic();
    }

    PrimaryGeneratorAction::~PrimaryGeneratorAction()
    {
        // delete fParticleGun;
        // delete fGunMessenger;
        delete fGParticleSource;
    }

    void PrimaryGeneratorAction::SetDefaultKinematic()
    {
        // BeamSpreadSigma = 0. * mm;
        // G4ParticleTable *ParticleTable = G4ParticleTable::GetParticleTable();
        // G4ParticleDefinition *IncidentParticle = ParticleTable->FindParticle("geantino");
        // fParticleGun->SetParticleDefinition(IncidentParticle);
        // // fParticleGun->SetParticleEnergy(1. * GeV);
        // fParticleGun->SetParticleMomentum(1. * GeV);
        // fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));
        // fParticleGun->SetParticlePosition(G4ThreeVector(2, 2, -100 * mm));
    }

    void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
    {
        // G4double GunPosX = fParticleGun->GetParticlePosition().x();
        // G4double GunPosY = fParticleGun->GetParticlePosition().y();
        // G4double GunPosZ = fParticleGun->GetParticlePosition().z();
        // if (BeamSpreadSigma > 0)
        //     fParticleGun->SetParticlePosition(G4ThreeVector(GunPosX + G4RandGauss::shoot(0., BeamSpreadSigma), GunPosY + G4RandGauss::shoot(0., BeamSpreadSigma), GunPosZ));
        fGParticleSource->GeneratePrimaryVertex(anEvent);
        // fParticleGun->SetParticlePosition(G4ThreeVector(GunPosX, GunPosY, GunPosZ));
    }

    void PrimaryGeneratorAction::SetBeamSpreadSigma(G4double newBeamSpreadSigma)
    {
        BeamSpreadSigma = newBeamSpreadSigma;
    }
}
