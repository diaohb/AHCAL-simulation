#include "PCBVolume.hh"
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
#include "G4NistManager.hh"
#include "Randomize.hh"
#include "CADMesh.hh"

namespace SimCalModule
{
    PCBVolume::PCBVolume(G4String PCBName, PCBParameter *inputPara, DetectorConstruction *pConstructor)
        : G4LogicalVolume(new G4Box(PCBName + "_Solid", 120.9 * mm, 400. * mm, 2.5 * mm), pConstructor->GetCaloMaterial(inputPara->AirIndex), PCBName), ThisUnitParameter(inputPara), fConstructor(pConstructor)
    {

        std::ifstream file_in("/cefs/higgs/diaohb/SIM/AHCAL-simulation/list");
        int line = 0;
        std::string stl_file;
        std::vector<std::string> comp_name = {
            "SR0603",
            "SC1206",
            "SC0402",
            "SN74LVC125A",
            "STC3528",
            "PW0016A",
            "18SOP",
            "SOIC8",
            "SOP58P810",
            "SD0805",
            "PW24",
            "KS5",
            "TMP117",
            "SOIC127P600",
            "RJ6",
            "RT3",
            "SR0402",
            "D8",
            "BB02",
            "FH29BW",
            "STC3216",
            "10603",
            "BGA",
            "MCH",
            "SC0603"};
        while (file_in >> stl_file)
        {
            line++;
            G4cout << "Loading " << line << "  " << stl_file << G4endl;
            int comp = 0;
            for (int i = 0; i < comp_name.size(); i++)
            {
                if (stl_file.find(comp_name.at(i)) != std::string::npos)
                {
                    comp = i;
                    break;
                }
                comp = -7;
            }
            auto mesh = CADMesh::TessellatedMesh::FromSTL(stl_file);
            // mesh->SetScale(500);
            auto logical = new G4LogicalVolume(mesh->GetSolid(), fConstructor->GetCaloMaterial((MaterialIndex)(comp + 18)), "logical");
            // G4cout << stl_file<<"  "<< fConstructor->GetCaloMaterial((MaterialIndex)(comp+18))->GetName()<< G4endl;

            if (stl_file.find("BOARD_OUTLINE") != std::string::npos)
            {
                logical->SetVisAttributes(G4Colour(0, 1, 1, 0.3));
                new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logical, "physical", this, true, 0);
            }
            else
            {
                logical->SetVisAttributes(G4Colour(0.5, 0.5, 0, 0.3));
                new G4PVPlacement(0, G4ThreeVector(0, 0, 0.01), logical, "physical", this, true, 0);
            }
        }
    }

    PCBVolume::~PCBVolume()
    {
    }
}
