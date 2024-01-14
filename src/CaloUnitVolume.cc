#include "CaloUnitVolume.hh"
#include "G4Box.hh"
#include "G4Orb.hh"
#include "G4Colour.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4RotationMatrix.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "Randomize.hh"

namespace SimCalModule
{
    CaloUnitVolume::CaloUnitVolume(G4String CaloName, G4RotationMatrix *pRot, const G4ThreeVector &pPlace, G4bool pcheckOverlaps, G4LogicalVolume *pMotherLogical,
                                   G4LogicalVolume *&pCaloUnitLogical, G4LogicalVolume *&pCaloSensitiveLogical, CaloUnitParameter *inputPara, G4bool pMany,
                                   G4int pCopyNo, DetectorConstruction *pConstructor)
        : G4PVPlacement(pRot, pPlace, new G4LogicalVolume(new G4Box("temp", 1., 1., 1.), G4Material::GetMaterial("G4_Galactic"), "temp", 0, 0, 0),
                        CaloName, pMotherLogical, pMany, pCopyNo, pcheckOverlaps),
        ThisUnitParameter(inputPara), SensitiveLogical(nullptr), Sensitive_dig_out_Logical(nullptr), PassiveLogical(nullptr),
         AttachLogical(nullptr), HousingLogical(nullptr), fConstructor(pConstructor)
    {
        if (pCaloUnitLogical == nullptr)
        {
            G4double Sensitive_x = ThisUnitParameter->SensitiveX / 2.;
            G4double Sensitive_y = ThisUnitParameter->SensitiveY / 2.;
            G4double Sensitive_z = ThisUnitParameter->SensitiveLength / 2.;
            G4double Sensitive_dig_out_x = ThisUnitParameter->Sensitive_dig_out_X / 2.;
            G4double Sensitive_dig_out_y = ThisUnitParameter->Sensitive_dig_out_Y / 2.;
            G4double Sensitive_dig_out_z = ThisUnitParameter->Sensitive_dig_out_Z / 2.;
            G4double Passive_x = Sensitive_x + ThisUnitParameter->PassiveSideThick;
            G4double Passive_y = Sensitive_y + ThisUnitParameter->PassiveSideThick;
            G4double Passive_z = Sensitive_z + ThisUnitParameter->PassiveCoverThick;
            G4double Housing_x = ThisUnitParameter->House_X/2.;
            G4double Housing_y = ThisUnitParameter->House_Y/2.;
            G4double Housing_z = ThisUnitParameter->House_Z/2.;//Passive_z + ((G4int)(ThisUnitParameter->ifDoubleSidedReadout) + 1) * ThisUnitParameter->AttachThick / 2.;

            // Box for housing
            G4Box *SolidHousing = new G4Box("SolidHousing", Housing_x, Housing_y, Housing_z);
            HousingLogical = new G4LogicalVolume(SolidHousing, G4Material::GetMaterial("G4_Galactic"), "HousingLogical");

            // Attach
            if (ThisUnitParameter->AttachThick > 0)
            {
                G4Box *SolidAttach = new G4Box("SolidAttach", Passive_x, Passive_y, ThisUnitParameter->AttachThick / 2.);
                AttachLogical = new G4LogicalVolume(SolidAttach, fConstructor->GetCaloMaterial(ThisUnitParameter->AttachMatIndex), "AttachLogical");
                if (ThisUnitParameter->ifDoubleSidedReadout)
                {
                    new G4PVPlacement(0, G4ThreeVector(0, 0, -Passive_z - ThisUnitParameter->AttachThick / 2.), AttachLogical, "AttachPhysical", HousingLogical, false, 0, pcheckOverlaps);
                    new G4PVPlacement(0, G4ThreeVector(0, 0, Passive_z + ThisUnitParameter->AttachThick / 2.), AttachLogical, "AttachPhysical", HousingLogical, false, 0, pcheckOverlaps);
                }
                else
                    new G4PVPlacement(0, G4ThreeVector(0, 0, -Passive_z), AttachLogical, "AttachPhysical", HousingLogical, false, 0, pcheckOverlaps);
            }

            // Passive
            G4Box *SolidPassive = new G4Box("SolidPassive", Passive_x, Passive_y, Passive_z);
            PassiveLogical = new G4LogicalVolume(SolidPassive, fConstructor->GetCaloMaterial(ThisUnitParameter->PassiveMatIndex), "PassiveLogical");
            new G4PVPlacement(0, G4ThreeVector(0, 0, (G4int)(!ThisUnitParameter->ifDoubleSidedReadout) * ThisUnitParameter->AttachThick / 2.), PassiveLogical, "PassivePhysical", HousingLogical, false, 0, pcheckOverlaps);

            // Sensitive
            G4Box *SolidSensitive = new G4Box("SolidSensitive", Sensitive_x, Sensitive_y, Sensitive_z);
            SensitiveLogical = new G4LogicalVolume(SolidSensitive, fConstructor->GetCaloMaterial(ThisUnitParameter->SensitiveMatIndex), "SensitiveLogical");
            new G4PVPlacement(0, G4ThreeVector(), SensitiveLogical, "SensitivePhysical", PassiveLogical, false, 0, pcheckOverlaps);

            // Sensitive
            G4Box *SolidSensitive_dig_out = new G4Box("SolidSensitive_dig_out", Sensitive_dig_out_x, Sensitive_dig_out_y, Sensitive_dig_out_z);
            Sensitive_dig_out_Logical = new G4LogicalVolume(SolidSensitive_dig_out, fConstructor->GetCaloMaterial(ThisUnitParameter->Sensitive_dig_out_MatIndex), "Sensitive_dig_out_Logical");
            new G4PVPlacement(0, G4ThreeVector(0,0,-Sensitive_z+Sensitive_dig_out_z), Sensitive_dig_out_Logical, "Sensitive_dig_out_Physical", SensitiveLogical, false, 0, pcheckOverlaps);
            SetVisAttributes();

            G4Box *SolidSensitive_dig_out_ESR = new G4Box("SolidSensitive_dig_out_ESR", Sensitive_dig_out_x, Sensitive_dig_out_y, ThisUnitParameter->PassiveCoverThick/2);
            Sensitive_dig_out_Logical_ESR = new G4LogicalVolume(SolidSensitive_dig_out_ESR, fConstructor->GetCaloMaterial(ThisUnitParameter->Sensitive_dig_out_MatIndex), "Sensitive_dig_out_Logical_ESR");
            new G4PVPlacement(0, G4ThreeVector(0,0,-Sensitive_z-ThisUnitParameter->PassiveCoverThick/2), Sensitive_dig_out_Logical_ESR, "Sensitive_dig_out_Physical_ESR", PassiveLogical, false, 0, pcheckOverlaps);
            SetVisAttributes();

            fConstructor->SetCaloLogicalVolume(pCaloSensitiveLogical, SensitiveLogical);
            fConstructor->SetCaloLogicalVolume(pCaloUnitLogical, HousingLogical);
            SetLogicalVolume(HousingLogical);
        }
        else
        {
            SetLogicalVolume(pCaloUnitLogical);
        }
    }

    CaloUnitVolume::~CaloUnitVolume()
    {
    }

    void CaloUnitVolume::SetVisAttributes()
    {
        G4VisAttributes *HousingVisAtt = new G4VisAttributes(G4Colour(0.6, 0.6, 0.8));
        HousingVisAtt->SetVisibility(false);
        HousingLogical->SetVisAttributes(HousingVisAtt);

        G4VisAttributes *PassiveVisAtt = new G4VisAttributes(G4Colour(0.6, 0.8, 1.0));
        PassiveVisAtt->SetVisibility(true);
        PassiveLogical->SetVisAttributes(PassiveVisAtt);

        G4VisAttributes *SensitiveVisAtt = new G4VisAttributes(G4Colour(0.28, 0.82, 0.8));
        SensitiveVisAtt->SetVisibility(true);
        SensitiveLogical->SetVisAttributes(SensitiveVisAtt);

        G4VisAttributes *Sensitive_dig_out_VisAtt = new G4VisAttributes(G4Colour(0.9, 0.9, 0.));
        Sensitive_dig_out_VisAtt->SetVisibility(true);
        Sensitive_dig_out_Logical->SetVisAttributes(Sensitive_dig_out_VisAtt);

        if (ThisUnitParameter->AttachThick > 0)
        {
            G4VisAttributes *AttachVisAtt = new G4VisAttributes(G4Colour(0, 1.0, 0));
            AttachVisAtt->SetVisibility(true);
            AttachLogical->SetVisAttributes(AttachVisAtt);
        }
    }
}
