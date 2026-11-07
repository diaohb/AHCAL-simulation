#ifndef PCBVolume_h
#define PCBVolume_h 1

#include "globals.hh"
#include "DetectorConstruction.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

namespace SimCalModule
{
    class DetectorConstruction;
    struct CaloUnitParameter;
    struct PCBParameter;

    class PCBVolume : public G4LogicalVolume
    {
    public:
        PCBVolume(G4String PCBName,PCBParameter *inputPara,DetectorConstruction *pConstructor);
        ~PCBVolume() override;
        PCBParameter *ThisUnitParameter;
        DetectorConstruction *fConstructor;

    };
}

#endif