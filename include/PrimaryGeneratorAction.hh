#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

namespace SimCalModule
{
    class PrimaryGeneratorMessenger;

    class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
    {
    public:
        PrimaryGeneratorAction();
        ~PrimaryGeneratorAction() override;

    public:
        void GeneratePrimaries(G4Event *) override;
        void SetDefaultKinematic();
        void SetBeamSpreadSigma(G4double);
        G4ParticleGun *GetParticleGun() const { return fParticleGun; };
        G4GeneralParticleSource *GetGPS() const { return fGParticleSource;};

    private:
        G4ParticleGun *fParticleGun;
        PrimaryGeneratorMessenger *fGunMessenger;
        G4GeneralParticleSource *fGParticleSource;
        G4double BeamSpreadSigma;
    };
}

#endif
