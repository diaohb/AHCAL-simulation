#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"

namespace SimCalModule
{
    DetectorMessenger::DetectorMessenger(DetectorConstruction *aCalDetector)
        : G4UImessenger(), calDetector(aCalDetector)
    {
        // Setup a command directory for detector controls with guidance
        fGeoDir = new G4UIdirectory("/cal/geo/");
        fGeoDir->SetGuidance("UI commands for geometry");

        fTimeDir = new G4UIdirectory("/cal/time/");
        fTimeDir->SetGuidance("UI commands for time");

        fEcalCellNumXCmd = new G4UIcmdWithAnInteger("/cal/geo/ecalCellNumX", this);
        fEcalCellNumXCmd->SetGuidance("Set number of cells of ECAL along X");
        fEcalCellNumXCmd->SetParameterName("ecalCellNumX", false);
        fEcalCellNumXCmd->SetToBeBroadcasted(false);
        fEcalCellNumXCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fEcalCellNumYCmd = new G4UIcmdWithAnInteger("/cal/geo/ecalCellNumY", this);
        fEcalCellNumYCmd->SetGuidance("Set number of cells of ECAL along Y");
        fEcalCellNumYCmd->SetParameterName("ecalCellNumY", false);
        fEcalCellNumYCmd->SetToBeBroadcasted(false);
        fEcalCellNumYCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fEcalLayerNumCmd = new G4UIcmdWithAnInteger("/cal/geo/ecalLayerNum", this);
        fEcalLayerNumCmd->SetGuidance("Set number of layers of ECAL");
        fEcalLayerNumCmd->SetParameterName("ecalLayerNum", false);
        fEcalLayerNumCmd->SetToBeBroadcasted(false);
        fEcalLayerNumCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHcalCellNumXCmd = new G4UIcmdWithAnInteger("/cal/geo/hcalCellNumX", this);
        fHcalCellNumXCmd->SetGuidance("Set number of cells of HCAL along X");
        fHcalCellNumXCmd->SetParameterName("hcalCellNumX", false);
        fHcalCellNumXCmd->SetToBeBroadcasted(false);
        fHcalCellNumXCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHcalCellNumYCmd = new G4UIcmdWithAnInteger("/cal/geo/hcalCellNumY", this);
        fHcalCellNumYCmd->SetGuidance("Set number of cells of HCAL along Y");
        fHcalCellNumYCmd->SetParameterName("hcalCellNumY", false);
        fHcalCellNumYCmd->SetToBeBroadcasted(false);
        fHcalCellNumYCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHcalLayerNumCmd = new G4UIcmdWithAnInteger("/cal/geo/hcalLayerNum", this);
        fHcalLayerNumCmd->SetGuidance("Set number of layers of HCAL");
        fHcalLayerNumCmd->SetParameterName("hcalLayerNum", false);
        fHcalLayerNumCmd->SetToBeBroadcasted(false);
        fHcalLayerNumCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fEcalSensitiveThickCmd = new G4UIcmdWithADoubleAndUnit("/cal/geo/ecalSensitiveThick", this);
        fEcalSensitiveThickCmd->SetGuidance("Set the thickness of an ECAL sensitive cell");
        fEcalSensitiveThickCmd->SetParameterName("ecalSensitiveThick", false);
        fEcalSensitiveThickCmd->SetDefaultUnit("mm");
        fEcalSensitiveThickCmd->SetToBeBroadcasted(false);
        fEcalSensitiveThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHcalSensitiveThickCmd = new G4UIcmdWithADoubleAndUnit("/cal/geo/hcalSensitiveThick", this);
        fHcalSensitiveThickCmd->SetGuidance("Set the thickness of an HCAL sensitive cell");
        fHcalSensitiveThickCmd->SetParameterName("hcalSensitiveThick", false);
        fHcalSensitiveThickCmd->SetDefaultUnit("mm");
        fHcalSensitiveThickCmd->SetToBeBroadcasted(false);
        fHcalSensitiveThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fEcalPassiveThickCmd = new G4UIcmdWithADoubleAndUnit("/cal/geo/ecalPassiveThick", this);
        fEcalPassiveThickCmd->SetGuidance("Set the thickness of an ECAL passive cell");
        fEcalPassiveThickCmd->SetParameterName("ecalPassiveThick", false);
        fEcalPassiveThickCmd->SetDefaultUnit("mm");
        fEcalPassiveThickCmd->SetToBeBroadcasted(false);
        fEcalPassiveThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHcalPassiveThickCmd = new G4UIcmdWithADoubleAndUnit("/cal/geo/hcalPassiveThick", this);
        fHcalPassiveThickCmd->SetGuidance("Set the thickness of an HCAL passive cell");
        fHcalPassiveThickCmd->SetParameterName("hcalPassiveThick", false);
        fHcalPassiveThickCmd->SetDefaultUnit("mm");
        fHcalPassiveThickCmd->SetToBeBroadcasted(false);
        fHcalPassiveThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fEcalAbsThickCmd = new G4UIcmdWithADoubleAndUnit("/cal/geo/ecalAbsThick", this);
        fEcalAbsThickCmd->SetGuidance("Set the thickness of an ECAL absorber plate");
        fEcalAbsThickCmd->SetParameterName("ecalAbsThick", false);
        fEcalAbsThickCmd->SetDefaultUnit("mm");
        fEcalAbsThickCmd->SetToBeBroadcasted(false);
        fEcalAbsThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHcalAbsThickCmd = new G4UIcmdWithADoubleAndUnit("/cal/geo/hcalAbsThick", this);
        fHcalAbsThickCmd->SetGuidance("Set the thickness of an HCAL absorber plate");
        fHcalAbsThickCmd->SetParameterName("hcalAbsThick", false);
        fHcalAbsThickCmd->SetDefaultUnit("mm");
        fHcalAbsThickCmd->SetToBeBroadcasted(false);
        fHcalAbsThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fUpstreamObjThickCmd = new G4UIcmdWithADoubleAndUnit("/cal/geo/upstreamThick", this);
        fUpstreamObjThickCmd->SetGuidance("Set the thickness of a slab in the upstream");
        fUpstreamObjThickCmd->SetParameterName("upstreamThick", false);
        fUpstreamObjThickCmd->SetDefaultUnit("mm");
        fUpstreamObjThickCmd->SetToBeBroadcasted(false);
        fUpstreamObjThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fIntermediateObjThickCmd = new G4UIcmdWithADoubleAndUnit("/cal/geo/intermediateThick", this);
        fIntermediateObjThickCmd->SetGuidance("Set the thickness of the magnet between ECAL and HCAL");
        fIntermediateObjThickCmd->SetParameterName("intermediateThick", false);
        fIntermediateObjThickCmd->SetDefaultUnit("mm");
        fIntermediateObjThickCmd->SetToBeBroadcasted(false);
        fIntermediateObjThickCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fEcalStepTimeLimitCmd = new G4UIcmdWithADoubleAndUnit("/cal/time/ecalStepTimeLimit", this);
        fEcalStepTimeLimitCmd->SetGuidance("Set the time limit for the ECAL energy deposition");
        fEcalStepTimeLimitCmd->SetParameterName("ecalStepTimeLimit", false);
        fEcalStepTimeLimitCmd->SetDefaultUnit("ns");
        fEcalStepTimeLimitCmd->SetToBeBroadcasted(false);
        fEcalStepTimeLimitCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        fHcalStepTimeLimitCmd = new G4UIcmdWithADoubleAndUnit("/cal/time/hcalStepTimeLimit", this);
        fHcalStepTimeLimitCmd->SetGuidance("Set the time limit for the HCAL energy deposition");
        fHcalStepTimeLimitCmd->SetParameterName("hcalStepTimeLimit", false);
        fHcalStepTimeLimitCmd->SetDefaultUnit("ns");
        fHcalStepTimeLimitCmd->SetToBeBroadcasted(false);
        fHcalStepTimeLimitCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    }

    DetectorMessenger::~DetectorMessenger()
    {
        delete fEcalCellNumXCmd;
        delete fEcalCellNumYCmd;
        delete fEcalLayerNumCmd;
        delete fHcalCellNumXCmd;
        delete fHcalCellNumYCmd;
        delete fHcalLayerNumCmd;
        delete fEcalAbsThickCmd;
        delete fHcalAbsThickCmd;
        delete fUpstreamObjThickCmd;
        delete fIntermediateObjThickCmd;
        delete fEcalStepTimeLimitCmd;
        delete fHcalStepTimeLimitCmd;
        delete fGeoDir;
        delete fTimeDir;
    }

    void DetectorMessenger::SetNewValue(G4UIcommand *cmd, G4String newValue)
    {
        if (cmd == fEcalCellNumXCmd)
        {
            calDetector->SetEcalCellNumberX(fEcalCellNumXCmd->GetNewIntValue(newValue));
        }
        else if (cmd == fEcalCellNumYCmd)
        {
            calDetector->SetEcalCellNumberY(fEcalCellNumYCmd->GetNewIntValue(newValue));
        }
        else if (cmd == fEcalLayerNumCmd)
        {
            calDetector->SetEcalLayerNumber(fEcalLayerNumCmd->GetNewIntValue(newValue));
        }
        else if (cmd == fHcalCellNumXCmd)
        {
            calDetector->SetHcalCellNumberX(fHcalCellNumXCmd->GetNewIntValue(newValue));
        }
        else if (cmd == fHcalCellNumYCmd)
        {
            calDetector->SetHcalCellNumberY(fHcalCellNumYCmd->GetNewIntValue(newValue));
        }
        else if (cmd == fHcalLayerNumCmd)
        {
            calDetector->SetHcalLayerNumber(fHcalLayerNumCmd->GetNewIntValue(newValue));
        }
        else if (cmd == fEcalSensitiveThickCmd)
        {
            calDetector->SetEcalSensitiveThick(fEcalSensitiveThickCmd->GetNewDoubleValue(newValue));
        }
        else if (cmd == fHcalSensitiveThickCmd)
        {
            calDetector->SetHcalSensitiveThick(fHcalSensitiveThickCmd->GetNewDoubleValue(newValue));
        }
        else if (cmd == fEcalPassiveThickCmd)
        {
            calDetector->SetEcalPassiveThick(fEcalPassiveThickCmd->GetNewDoubleValue(newValue));
        }
        else if (cmd == fHcalPassiveThickCmd)
        {
            calDetector->SetHcalPassiveThick(fHcalPassiveThickCmd->GetNewDoubleValue(newValue));
        }
        else if (cmd == fEcalAbsThickCmd)
        {
            calDetector->SetEcalAbsThick(fEcalAbsThickCmd->GetNewDoubleValue(newValue));
        }
        else if (cmd == fHcalAbsThickCmd)
        {
            calDetector->SetHcalAbsThick(fHcalAbsThickCmd->GetNewDoubleValue(newValue));
        }
        else if (cmd == fUpstreamObjThickCmd)
        {
            calDetector->SetUpstreamObjThick(fUpstreamObjThickCmd->GetNewDoubleValue(newValue));
        }
        else if (cmd == fIntermediateObjThickCmd)
        {
            calDetector->SetIntermediateObjThick(fIntermediateObjThickCmd->GetNewDoubleValue(newValue));
        }
        else if (cmd == fEcalStepTimeLimitCmd)
        {
            calDetector->SetEcalStepTimeLimit(fEcalStepTimeLimitCmd->GetNewDoubleValue(newValue));
        }
        else if (cmd == fHcalStepTimeLimitCmd)
        {
            calDetector->SetHcalStepTimeLimit(fHcalStepTimeLimitCmd->GetNewDoubleValue(newValue));
        }
    }
}