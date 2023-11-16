#ifndef PrimaryGeneratorMessenger_h
#define PrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcommand;

namespace SimCalModule
{
    class PrimaryGeneratorAction;

    class PrimaryGeneratorMessenger : public G4UImessenger
    {
    public:
        PrimaryGeneratorMessenger(PrimaryGeneratorAction *);
        ~PrimaryGeneratorMessenger() override;

        void SetNewValue(G4UIcommand *, G4String) override;

    private:
        PrimaryGeneratorAction *fPrimaryGeneratorAction;
        G4UIdirectory *GunDir;
        G4UIcmdWithADoubleAndUnit *SetBeamSpreadSigmaCmd;
    };
}

#endif
