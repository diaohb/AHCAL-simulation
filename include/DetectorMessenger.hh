#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;
class G4UIcmdWithAString;
class G4UIcommand;

namespace SimCalModule
{
    class DetectorConstruction;

    class DetectorMessenger : public G4UImessenger
    {
    public:
        DetectorMessenger(DetectorConstruction *);
        ~DetectorMessenger() override;
        void SetNewValue(G4UIcommand *, G4String);

    private:
        DetectorConstruction *calDetector;
        G4UIdirectory *fGeoDir;
        G4UIdirectory *fTimeDir;
        G4UIcmdWithAnInteger *fEcalCellNumXCmd;
        G4UIcmdWithAnInteger *fEcalCellNumYCmd;
        G4UIcmdWithAnInteger *fEcalLayerNumCmd;
        G4UIcmdWithAnInteger *fHcalCellNumXCmd;
        G4UIcmdWithAnInteger *fHcalCellNumYCmd;
        G4UIcmdWithAnInteger *fHcalLayerNumCmd;
        G4UIcmdWithADoubleAndUnit *fEcalSensitiveThickCmd;
        G4UIcmdWithADoubleAndUnit *fHcalSensitiveThickCmd;
        G4UIcmdWithADoubleAndUnit *fEcalPassiveThickCmd;
        G4UIcmdWithADoubleAndUnit *fHcalPassiveThickCmd;
        G4UIcmdWithADoubleAndUnit *fEcalAbsThickCmd;
        G4UIcmdWithADoubleAndUnit *fHcalAbsThickCmd;
        G4UIcmdWithADoubleAndUnit *fUpstreamObjThickCmd;
        G4UIcmdWithADoubleAndUnit *fIntermediateObjThickCmd;
        G4UIcmdWithADoubleAndUnit *fEcalStepTimeLimitCmd;
        G4UIcmdWithADoubleAndUnit *fHcalStepTimeLimitCmd;
    };
}

#endif
