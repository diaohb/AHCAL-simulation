#ifndef CaloUnitVolume_h
#define CaloUnitVolume_h 1

#include "globals.hh"
#include "DetectorConstruction.hh"
#include "G4PVPlacement.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

namespace SimCalModule
{
    class DetectorConstruction;
    struct CaloUnitParameter;

    class CaloUnitVolume : public G4PVPlacement
    {
    public:
        CaloUnitVolume(G4String CaloName, G4RotationMatrix *pRot, const G4ThreeVector &pPlace, G4bool pcheckOverlaps,
                       G4LogicalVolume *pMotherLogical, G4LogicalVolume *&pCaloUnitLogical, G4LogicalVolume *&pCaloSensitiveLogical,
                       CaloUnitParameter *inputPara, G4bool pMany, G4int pCopyNo, DetectorConstruction *pConstructor);
        ~CaloUnitVolume() override;

    private:
        void SetVisAttributes();
        CaloUnitParameter *ThisUnitParameter;
        G4LogicalVolume *SensitiveLogical;
        G4LogicalVolume *Sensitive_dig_out_Logical;
        G4LogicalVolume *Sensitive_dig_out_Logical_ESR;
        G4LogicalVolume *PassiveLogical;
        G4LogicalVolume *AttachLogical;
        G4LogicalVolume *HousingLogical;
        DetectorConstruction *fConstructor;
    };
}

#endif