
#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

namespace SimCalModule
{
    PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction *PGA)
        : G4UImessenger(), fPrimaryGeneratorAction(PGA)
    {
        GunDir = new G4UIdirectory("/cal/gun/");
        GunDir->SetGuidance("PrimaryGeneratorAction Control");

        SetBeamSpreadSigmaCmd = new G4UIcmdWithADoubleAndUnit("/cal/gun/BeamSpreadSigma", this);
        SetBeamSpreadSigmaCmd->SetGuidance("Gaussion spread sigma of incident particles");
        SetBeamSpreadSigmaCmd->SetParameterName("BeamSpreadSigma", false);
        SetBeamSpreadSigmaCmd->SetDefaultUnit("mm");
        SetBeamSpreadSigmaCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    }

    PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
    {
        delete SetBeamSpreadSigmaCmd;
        delete GunDir;
    }

    void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand *cmd, G4String newValue)
    {
        if (cmd == SetBeamSpreadSigmaCmd)
            fPrimaryGeneratorAction->SetBeamSpreadSigma(SetBeamSpreadSigmaCmd->GetNewDoubleValue(newValue));
    }
}
