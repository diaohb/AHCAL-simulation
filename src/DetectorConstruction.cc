#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4AutoDelete.hh"
#include "G4SDManager.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4RunManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"

#include "G4GeometryManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"

namespace SimCalModule
{
    DetectorConstruction::DetectorConstruction()
        : G4VUserDetectorConstruction(), ifcheckOverlaps(false)
    {
        EcalUnitLogical = nullptr;
        HcalUnitLogical = nullptr;
        EcalSensitiveLogical = nullptr;
        HcalSensitiveLogical = nullptr;
        fEcalUnitSD.Put(0);
        fHcalUnitSD.Put(0);
        fFieldMessenger.Put(0);
        fDetectorMessenger = new DetectorMessenger(this);
        DefineMaterials();
        SetDefaults();
        G4cout << "DetectorConstruction initialized." << G4endl;
    }

    DetectorConstruction::~DetectorConstruction()
    {
        G4cout << "DetectorConstruction destructed." << G4endl;
        delete fDetectorMessenger;
    }

    void DetectorConstruction::SetDefaults()
    {
        EcalUnitParameter.ifDoubleSidedReadout = false;
        EcalUnitParameter.SensitiveLength = 2 * mm;
        EcalUnitParameter.SensitiveX = 45 * mm;
        EcalUnitParameter.SensitiveY = 5 * mm;
        EcalUnitParameter.PassiveSideThick = 0.15 * mm;
        EcalUnitParameter.PassiveCoverThick = 0.25 * mm;
        EcalUnitParameter.AttachThick = 0 * mm;
        EcalUnitParameter.SensitiveMatIndex = PlasticSciECAL;
        EcalUnitParameter.PassiveMatIndex = ESR;
        EcalUnitParameter.AttachMatIndex = Quartz;
        
        HcalUnitParameter.ifDoubleSidedReadout = false;
        HcalUnitParameter.SensitiveLength = 3.0 * mm;
        HcalUnitParameter.SensitiveX = 40.0 * mm;
        HcalUnitParameter.SensitiveY = HcalUnitParameter.SensitiveX;
        HcalUnitParameter.PassiveSideThick = 0.065 * mm;
        HcalUnitParameter.PassiveCoverThick = 0.065 * mm;
        HcalUnitParameter.House_X = 40.3 * mm;
        HcalUnitParameter.House_Y = HcalUnitParameter.House_X;
        HcalUnitParameter.House_Z = 3.5 * mm;
        HcalUnitParameter.AttachThick = 0 * mm;
        HcalUnitParameter.Sensitive_dig_out_X =5.5 * mm;
        HcalUnitParameter.Sensitive_dig_out_Y = HcalUnitParameter.Sensitive_dig_out_X;
        HcalUnitParameter.Sensitive_dig_out_Z = 1.1 * mm;
        HcalUnitParameter.SiPM_Sensitive_X = 1.3 * mm;
        HcalUnitParameter.SiPM_Sensitive_Y = HcalUnitParameter.SiPM_Sensitive_X;
        HcalUnitParameter.SiPM_Sensitive_Z = 0.01 * mm;
        HcalUnitParameter.Sensitive_SiPM_MatIndex = Si;
        HcalUnitParameter.SensitiveMatIndex = PlasticSciHCAL;
        HcalUnitParameter.Sensitive_dig_out_MatIndex = Si;
        HcalUnitParameter.PassiveMatIndex = ESR;
        HcalUnitParameter.AttachMatIndex = Quartz;
        EcalAbsorberThick = 3.2 * mm; // 3.2 mm for ScW ECAL
        HcalAbsorberThick = 20.0 * mm;
        EcalPCBThick = 2.0 * mm;

        HcaltriggerThick = 40.0 * mm;
        HcaltriggerIndex = PlasticSciHCAL;
        HcalPCBThick = 3.5 * mm;  //2.5mm *4/5 for PCB, 1mm for component
        HcalPCB_Cu_Thick = 0.8 * mm; //2.5mm *1/5
        HcalPCB_Abs_gap = 6.5 * mm - HcalPCBThick - HcalPCB_Cu_Thick;//4mm-1mm
        
        UpstreamSizeX = 0 * mm;
        UpstreamSizeY = 0 * mm;
        UpstreamSizeZ = 0 * mm;
        IntermediateSizeX = 0 * mm;
        IntermediateSizeY = 0 * mm;
        IntermediateSizeZ = 0 * mm;
        WorldMatIndex = Air;
        EcalAbsorberMatIndex = CuW;
        HcalAbsorberMatIndex = Steel;
        EcalPCBMatIndex = FR4;
        HcalPCBMatIndex = FR4;
        HcalPCB_Cu_MatIndex = Cu;
        UpstreamMatIndex = Al;
        IntermediateMatIndex = Air;
        EcalCellNumberX = 5;
        EcalCellNumberY = 42;
        EcalLayerNumber = 32;
        HcalCellNumberX = 18;
        HcalCellNumberY = 18;
        HcalLayerNumber = 40;
        EcalModuleType = 0; // 0:Off; 1:Cube; 2:Crossed bar; 3:ScW ECAL
        HcalModuleType = 1; // 0:Off; 1:AHCAL; 2:GSHCAL
        EcalStepTimeLimit = 150.0 * ns;
        HcalStepTimeLimit = 150.0 * ns;
    }

    void DetectorConstruction::DefineMaterials()
    {
        G4NistManager *nistManager = G4NistManager::Instance();
        G4double density;
        G4int nElements;
        G4int nComponents;
        G4int nAtoms;
        G4double fracMass;

        G4Element *EleH = nistManager->FindOrBuildElement("H");
        G4Element *EleC = nistManager->FindOrBuildElement("C");
        G4Element *EleO = nistManager->FindOrBuildElement("O");
        G4Element *EleCe = nistManager->FindOrBuildElement("Ce");
        G4Element *EleB = nistManager->FindOrBuildElement("B");
        G4Element *EleF = nistManager->FindOrBuildElement("F");
        G4Element *EleGd = nistManager->FindOrBuildElement("Gd");
        G4Element *EleMn = nistManager->FindOrBuildElement("Mn");
        G4Element *EleP = nistManager->FindOrBuildElement("P");
        G4Element *EleS = nistManager->FindOrBuildElement("S");
        G4Element *EleSi = nistManager->FindOrBuildElement("Si");
        G4Element *EleGe = nistManager->FindOrBuildElement("Ge");

        G4Material *VacuumMat = nistManager->FindOrBuildMaterial("G4_Galactic");
        MaterialStore.push_back(VacuumMat); // 0

        G4Material *AirMat = nistManager->FindOrBuildMaterial("G4_AIR");
        MaterialStore.push_back(AirMat); // 1

        G4Material *AlMat = nistManager->FindOrBuildMaterial("G4_Al");
        MaterialStore.push_back(AlMat); // 2

        G4Material *SiMat = nistManager->FindOrBuildMaterial("G4_Si");
        MaterialStore.push_back(SiMat); // 3

        G4Material *FeMat = nistManager->FindOrBuildMaterial("G4_Fe");
        MaterialStore.push_back(FeMat); // 4

        G4Material *WMat = nistManager->FindOrBuildMaterial("G4_W");
        MaterialStore.push_back(WMat); // 5

        G4Material *CuMat = nistManager->FindOrBuildMaterial("G4_Cu");
        MaterialStore.push_back(CuMat); // 6

        // G4Material *WMat = new G4Material("W", density = 17.8 * g / cm3, nComponents = 3);
        // WMat->AddElement(EleW, 92.99 * perCent);
        // WMat->AddElement(EleNi, 5.25 * perCent);
        // WMat->AddElement(EleCu, 1.76 * perCent);
        // MaterialStore.push_back(WMat); // 7

        G4Material *CuWMat = new G4Material("CuW", density = 16.45 * g / cm3, nComponents = 2);
        CuWMat->AddMaterial(WMat, fracMass = 85. * perCent);
        CuWMat->AddMaterial(CuMat, fracMass = 15. * perCent);
        MaterialStore.push_back(CuWMat); // 7

        G4Material *SteelMat = new G4Material("Steel", density = 7.85 * g / cm3, nComponents = 6);
        SteelMat->AddElement(EleC, fracMass = 0.22 * perCent);
        SteelMat->AddElement(EleMn, fracMass = 1.4 * perCent);
        SteelMat->AddMaterial(SiMat, fracMass = 0.35 * perCent);
        SteelMat->AddElement(EleP, fracMass = 0.045 * perCent);
        SteelMat->AddElement(EleS, fracMass = 0.05 * perCent);
        SteelMat->AddMaterial(FeMat, fracMass = 97.935 * perCent);
        MaterialStore.push_back(SteelMat); // 8

        G4Material *QuartzMat = nistManager->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
        MaterialStore.push_back(QuartzMat); // 9

        G4Material *EpoxyMat = new G4Material("Epoxy", density = 1.25 * g / cm3, nElements = 3);
        EpoxyMat->AddElement(EleC, nAtoms = 11);
        EpoxyMat->AddElement(EleH, nAtoms = 12);
        EpoxyMat->AddElement(EleO, nAtoms = 3);
        MaterialStore.push_back(EpoxyMat); // 10

		G4Material *FR4Mat = new G4Material("FR4", density = 1.86 * g / cm3, nComponents = 2);
		FR4Mat->AddMaterial(QuartzMat, fracMass = 52.8 * perCent);
		FR4Mat->AddMaterial(EpoxyMat, fracMass = 47.2 * perCent);
        
        //PCB:Cu=4:1, frac of V, thick=2.5mm
        // components_thick=3mm/3=1mm, mat=FR4
        //total FR4:Cu=3mm:0.5mm, density=(3*1.86+0.5*8.9)/(3+0.5)=2.86

        // G4Material *FR4Mat = new G4Material("FR4", density = 1.86 * g / cm3, nComponents = 2);
        // FR4Mat->AddMaterial(QuartzMat, fracMass = 50 * perCent);
        // FR4Mat->AddMaterial(EpoxyMat, fracMass = 50 * perCent);
        // FR4Mat->AddMaterial(CuMat, fracMass = 44.4 * perCent);
        MaterialStore.push_back(FR4Mat); // 11

        // G4Material *ESRMat = new G4Material("ESR", density = 0.90 * g / cm3, nElements = 2);
        // ESRMat->AddElement(EleC, nAtoms = 2);
        // ESRMat->AddElement(EleH, nAtoms = 4);
        G4Material *ESRMat = new G4Material("ESR", density = 1.38 * g / cm3, nElements = 3);
        ESRMat->AddElement(EleC, nAtoms = 10);
        ESRMat->AddElement(EleH, nAtoms = 8);
        ESRMat->AddElement(EleO, nAtoms = 4);
        MaterialStore.push_back(ESRMat); // 12

        G4Material *PWOMat = nistManager->FindOrBuildMaterial("G4_PbWO4");
        PWOMat->GetIonisation()->SetBirksConstant(0.0333333 * mm / MeV);
        MaterialStore.push_back(PWOMat); // 13

        G4Material *BGOMat = nistManager->FindOrBuildMaterial("G4_BGO");
        BGOMat->GetIonisation()->SetBirksConstant(0.008415 * mm / MeV);
        MaterialStore.push_back(BGOMat); // 14

        G4Material *PlasticSciHCALMat = nistManager->BuildMaterialWithNewDensity("polystyrene", "G4_POLYSTYRENE", 1.032 * g / cm3);
        PlasticSciHCALMat->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);
        MaterialStore.push_back(PlasticSciHCALMat); // 15

        G4Material *PlasticSciECALMat = nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
        PlasticSciECALMat->GetIonisation()->SetBirksConstant(0.07943 * mm / MeV);
        MaterialStore.push_back(PlasticSciECALMat); // 16

        G4Material *SciGlassMat = new G4Material("SciGlass", density = 6.0 * g / cm3, nElements = 7);
        SciGlassMat->AddElement(EleSi, nAtoms = 10);
        SciGlassMat->AddElement(EleO, nAtoms = 192);
        SciGlassMat->AddElement(EleB, nAtoms = 50);
        SciGlassMat->AddElement(EleF, nAtoms = 93);
        SciGlassMat->AddElement(EleGd, nAtoms = 88);
        SciGlassMat->AddElement(EleCe, nAtoms = 1);
        SciGlassMat->AddElement(EleGe, nAtoms = 5);
        SciGlassMat->GetIonisation()->SetBirksConstant(0.1 * mm / MeV);
        MaterialStore.push_back(SciGlassMat); // 17

        // Print material table
        G4cout << *(G4Material::GetMaterialTable()) << G4endl;
    }

    G4VPhysicalVolume *DetectorConstruction::Construct()
    {
        // Cleanup old geometry
        G4GeometryManager::GetInstance()->OpenGeometry();
        G4PhysicalVolumeStore::GetInstance()->Clean();
        G4LogicalVolumeStore::GetInstance()->Clean();
        G4SolidStore::GetInstance()->Clean();

        // World
        G4double HalfCubicWorld = 5 * m;
        G4Box *World_Solid = new G4Box("World_Solid", HalfCubicWorld, HalfCubicWorld, HalfCubicWorld);
        G4LogicalVolume *World_Logical = new G4LogicalVolume(World_Solid, GetCaloMaterial(WorldMatIndex), "World_Logical");
        G4VPhysicalVolume *World_Physical = new G4PVPlacement(0, G4ThreeVector(), World_Logical, "World_Physical", 0, false, 0, ifcheckOverlaps);

        // Upstream
        G4Box *UpstreamSolid = nullptr;
        G4LogicalVolume *UpstreamLogical = nullptr;
        if (UpstreamSizeX * UpstreamSizeY * UpstreamSizeZ > 0)
        {
            UpstreamSolid = new G4Box("UpstreamSolid", UpstreamSizeX / 2., UpstreamSizeY / 2., UpstreamSizeZ / 2.);
            UpstreamLogical = new G4LogicalVolume(UpstreamSolid, GetCaloMaterial(UpstreamMatIndex), "UpstreamLogical");
            new G4PVPlacement(0, G4ThreeVector(0, 0, -UpstreamSizeZ / 2. - 50. * mm), UpstreamLogical, "UpstreamPhysical", World_Logical, false, 0, ifcheckOverlaps);
        }

        // ECAL
        G4double EcalUnitSizeX = EcalUnitParameter.SensitiveX + 2. * EcalUnitParameter.PassiveSideThick;
        G4double EcalUnitSizeY = EcalUnitParameter.SensitiveY + 2. * EcalUnitParameter.PassiveSideThick;
        G4double EcalUnitSizeZ = EcalUnitParameter.SensitiveLength + 2. * EcalUnitParameter.PassiveCoverThick + ((G4int)(EcalUnitParameter.ifDoubleSidedReadout) + 1) * EcalUnitParameter.AttachThick;
        G4Box *EcalAbsSolid = nullptr;
        G4LogicalVolume *EcalAbsLogical = nullptr;
        G4Box *EcalPCBSolid = nullptr;
        G4LogicalVolume *EcalPCBLogical = nullptr;
        G4double EcalXYsize = std::max(EcalCellNumberX * EcalUnitSizeX, EcalCellNumberY * EcalUnitSizeY);
        if (EcalModuleType > 0)
        {
            if (EcalAbsorberThick > 0)
            {
                EcalAbsSolid = new G4Box("EcalAbsSolid", EcalXYsize / 2., EcalXYsize / 2., EcalAbsorberThick / 2.);
                EcalAbsLogical = new G4LogicalVolume(EcalAbsSolid, GetCaloMaterial(EcalAbsorberMatIndex), "EcalAbsLogical");
            }
            if (EcalPCBThick > 0)
            {
                EcalPCBSolid = new G4Box("EcalPCBSolid", EcalXYsize / 2., EcalXYsize / 2., EcalPCBThick / 2.);
                EcalPCBLogical = new G4LogicalVolume(EcalPCBSolid, GetCaloMaterial(EcalPCBMatIndex), "EcalPCBLogical");
            }
        }
        if (EcalModuleType == 1)
        {
            G4RotationMatrix *EcalUnitInv = new G4RotationMatrix();
            EcalUnitInv->rotateY(180. * deg);
            G4double Zpos = EcalUnitSizeZ / 2.;
            auto EcalCellMax = std::to_string(std::max(EcalCellNumberX, EcalCellNumberY));
            G4int EcalCellMaxCount = pow(10, EcalCellMax.length());
            G4int EcalCopyNum = 0;
            for (G4int z = 0; z < EcalLayerNumber; z++)
            {
                for (G4int y = 0; y < EcalCellNumberY; y++)
                {
                    EcalCopyNum = (z + 1) * EcalCellMaxCount * EcalCellMaxCount + (y + 1) * EcalCellMaxCount;
                    for (G4int x = 0; x < EcalCellNumberX; x++)
                    {
                        EcalCopyNum++;
                        new CaloUnitVolume("EcalUnit", EcalUnitInv, G4ThreeVector(EcalUnitSizeX * (EcalCellNumberX / 2. - 0.5 - x), EcalUnitSizeY * (EcalCellNumberY / 2. - 0.5 - y), Zpos),
                                           ifcheckOverlaps, World_Logical, EcalUnitLogical, EcalSensitiveLogical, &EcalUnitParameter, false, EcalCopyNum, this);
                    }
                }
                Zpos += (EcalUnitSizeZ + EcalPCBThick) / 2.;
                if (EcalPCBThick > 0)
                    new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), EcalPCBLogical, "EcalPCBPhysical", World_Logical, false, EcalCopyNum / EcalCellMaxCount / EcalCellMaxCount * EcalCellMaxCount * EcalCellMaxCount, ifcheckOverlaps);
                Zpos += (EcalPCBThick + EcalAbsorberThick) / 2.;
                if (EcalAbsorberThick > 0)
                    new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), EcalAbsLogical, "EcalAbsPhysical", World_Logical, false, EcalCopyNum / EcalCellMaxCount / EcalCellMaxCount * EcalCellMaxCount * EcalCellMaxCount, ifcheckOverlaps);
                Zpos += (EcalAbsorberThick + EcalUnitSizeZ) / 2.;
            }
        }
        else if (EcalModuleType == 2)
        {
            EcalUnitSizeY = EcalUnitParameter.SensitiveY + 2. * EcalUnitParameter.PassiveSideThick;
            EcalUnitSizeZ = EcalUnitSizeX;
            G4RotationMatrix *EcalUnitRMHorizontal = new G4RotationMatrix();
            EcalUnitRMHorizontal->rotateY(90. * deg);
            G4RotationMatrix *EcalUnitRMVertical = new G4RotationMatrix();
            EcalUnitRMVertical->rotateZ(90. * deg);
            EcalUnitRMVertical->rotateY(-90. * deg);
            G4double Zpos = EcalUnitSizeZ / 2.;
            auto EcalCellMax = std::to_string(std::max(EcalCellNumberX, EcalCellNumberY));
            G4int EcalCellMaxCount = pow(10, EcalCellMax.length());
            G4int EcalCopyNum = 0;
            for (G4int z = 0; z < EcalLayerNumber; z++)
            {
                EcalCopyNum = (z + 1) * EcalCellMaxCount;
                if (z % 2 == 0)
                    for (G4int y = 0; y < EcalCellNumberY; y++)
                    {
                        EcalCopyNum++;
                        new CaloUnitVolume("EcalUnit", EcalUnitRMHorizontal, G4ThreeVector(0, EcalUnitSizeY * (EcalCellNumberY / 2. - 0.5 - y), Zpos),
                                           ifcheckOverlaps, World_Logical, EcalUnitLogical, EcalSensitiveLogical, &EcalUnitParameter, false, EcalCopyNum, this);
                    }
                else
                    for (G4int x = 0; x < EcalCellNumberX; x++)
                    {
                        EcalCopyNum++;
                        new CaloUnitVolume("EcalUnit", EcalUnitRMVertical, G4ThreeVector(EcalUnitSizeY * (EcalCellNumberX / 2. - 0.5 - x), 0, Zpos),
                                           ifcheckOverlaps, World_Logical, EcalUnitLogical, EcalSensitiveLogical, &EcalUnitParameter, false, EcalCopyNum, this);
                    }
                Zpos += (EcalUnitSizeZ + EcalPCBThick) / 2.;
                if (EcalPCBThick > 0)
                    new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), EcalPCBLogical, "EcalPCBPhysical", World_Logical, false, EcalCopyNum / EcalCellMaxCount * EcalCellMaxCount, ifcheckOverlaps);
                Zpos += (EcalPCBThick + EcalAbsorberThick) / 2.;
                if (EcalAbsorberThick > 0)
                    new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), EcalAbsLogical, "EcalAbsPhysical", World_Logical, false, EcalCopyNum / EcalCellMaxCount * EcalCellMaxCount, ifcheckOverlaps);
                Zpos += (EcalAbsorberThick + EcalUnitSizeZ) / 2.;
            }
        }
        else if (EcalModuleType == 3)
        {
            EcalUnitSizeX = EcalUnitParameter.SensitiveX + 2. * EcalUnitParameter.PassiveSideThick;
            EcalUnitSizeY = EcalUnitParameter.SensitiveY + 2. * EcalUnitParameter.PassiveSideThick;
            EcalUnitSizeZ = EcalUnitParameter.SensitiveLength + 2. * EcalUnitParameter.PassiveCoverThick + ((G4int)(EcalUnitParameter.ifDoubleSidedReadout) + 1) * EcalUnitParameter.AttachThick;
            G4RotationMatrix *EcalUnitRM = new G4RotationMatrix();
            EcalUnitRM->rotateZ(90. * deg);
            G4double ECALCoverThick = 1. * mm;  // Additional cover
            G4double Zpos = ECALCoverThick / 2.;
            auto ECALCoverSolid = new G4Box("ECALCoverSolid", EcalXYsize / 2., EcalXYsize / 2., ECALCoverThick / 2.);
            auto ECALCoverLogical = new G4LogicalVolume(ECALCoverSolid, GetCaloMaterial(Al), "ECALCoverLogical");
            new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), ECALCoverLogical, "ECALCoverPhysicalFront", World_Logical, false, 0, ifcheckOverlaps);
            Zpos += 2. * mm;
            Zpos += (ECALCoverThick + EcalUnitSizeZ) / 2.;
            auto EcalCellMax = std::to_string(std::max(EcalCellNumberX, EcalCellNumberY));
            G4int EcalCellMaxCount = pow(10, EcalCellMax.length());
            G4int EcalCopyNum = 0;
            for (G4int z = 0; z < EcalLayerNumber; z++)
            {
                if (z % 2 == 0)
                {
                    for (G4int y = 0; y < EcalCellNumberY; y++)
                    {
                        EcalCopyNum = (z + 1) * EcalCellMaxCount * EcalCellMaxCount + (y + 1) * EcalCellMaxCount;
                        for (G4int x = 0; x < EcalCellNumberX; x++)
                        {
                            EcalCopyNum++;
                            new CaloUnitVolume("EcalUnit", 0, G4ThreeVector(EcalUnitSizeX * (EcalCellNumberX / 2. - 0.5 - x), EcalUnitSizeY * (EcalCellNumberY / 2. - 0.5 - y), Zpos),
                                               ifcheckOverlaps, World_Logical, EcalUnitLogical, EcalSensitiveLogical, &EcalUnitParameter, false, EcalCopyNum, this);
                        }
                    }
                    Zpos += (EcalUnitSizeZ + EcalPCBThick) / 2.;
                    if (EcalPCBThick > 0)
                        new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), EcalPCBLogical, "EcalPCBPhysical", World_Logical, false, EcalCopyNum / EcalCellMaxCount * EcalCellMaxCount, ifcheckOverlaps);
                    Zpos += 0.75 * mm;
                    Zpos += (EcalPCBThick + EcalAbsorberThick) / 2.;
                    if (EcalAbsorberThick > 0)
                        new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), EcalAbsLogical, "EcalAbsPhysical", World_Logical, false, EcalCopyNum / EcalCellMaxCount * EcalCellMaxCount, ifcheckOverlaps);
                    Zpos += (EcalAbsorberThick + EcalPCBThick) / 2.;
                    Zpos += 0.75 * mm;
                }
                else
                {
                    if (EcalPCBThick > 0)
                        new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), EcalPCBLogical, "EcalPCBPhysical", World_Logical, false, EcalCopyNum / EcalCellMaxCount * EcalCellMaxCount, ifcheckOverlaps);
                    Zpos += (EcalUnitSizeZ + EcalPCBThick) / 2.;
                    for (G4int y = 0; y < EcalCellNumberX; y++)
                    {
                        EcalCopyNum = (z + 1) * EcalCellMaxCount * EcalCellMaxCount + (y + 1) * EcalCellMaxCount;
                        for (G4int x = 0; x < EcalCellNumberY; x++)
                        {
                            EcalCopyNum++;
                            new CaloUnitVolume("EcalUnit", EcalUnitRM, G4ThreeVector(EcalUnitSizeY * (EcalCellNumberY / 2. - 0.5 - x), EcalUnitSizeX * (EcalCellNumberX / 2. - 0.5 - y), Zpos),
                                               ifcheckOverlaps, World_Logical, EcalUnitLogical, EcalSensitiveLogical, &EcalUnitParameter, false, EcalCopyNum, this);
                        }
                    }
                    Zpos += 1.5 * mm;
                    Zpos += (EcalUnitSizeZ + EcalAbsorberThick) / 2.;
                    if (EcalAbsorberThick > 0)
                        new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), EcalAbsLogical, "EcalAbsPhysical", World_Logical, false, EcalCopyNum / EcalCellMaxCount * EcalCellMaxCount, ifcheckOverlaps);
                    Zpos += (EcalAbsorberThick + EcalUnitSizeZ) / 2.;
                    Zpos += 1.5 * mm;
                }
            }
            Zpos -= EcalPCBThick / 2.;
            Zpos -= 0.75 * mm;
            Zpos += 2. * mm;
            Zpos += ECALCoverThick / 2.;
            new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), ECALCoverLogical, "ECALCoverPhysicalBack", World_Logical, false, 0, ifcheckOverlaps);
        }

        // Intermediate
        G4Box *IntermediateSolid = nullptr;
        G4LogicalVolume *IntermediateLogical = nullptr;
        if (IntermediateSizeX * IntermediateSizeY * IntermediateSizeZ > 0)
        {
            IntermediateSolid = new G4Box("IntermediateSolid", IntermediateSizeX / 2., IntermediateSizeY / 2., IntermediateSizeZ / 2.);
            IntermediateLogical = new G4LogicalVolume(IntermediateSolid, GetCaloMaterial(IntermediateMatIndex), "IntermediateLogical");
            G4double Zpos = IntermediateSizeZ / 2.;
            if (EcalModuleType > 0)
                Zpos += (EcalUnitSizeZ + EcalPCBThick + EcalAbsorberThick) * EcalLayerNumber;
            new G4PVPlacement(0, G4ThreeVector(0, 0, EcalUnitSizeZ * EcalLayerNumber + IntermediateSizeZ / 2.), IntermediateLogical, "IntermediatePhysical", World_Logical, false, 0, ifcheckOverlaps);
        }

        // HCAL
        G4double HcalUnitSizeX = HcalUnitParameter.House_X;//HcalUnitParameter.SensitiveX + 2. * HcalUnitParameter.PassiveSideThick;
        G4double HcalUnitSizeY = HcalUnitParameter.House_X;//HcalUnitParameter.SensitiveY + 2. * HcalUnitParameter.PassiveSideThick;
        G4double HcalUnitSizeZ = HcalUnitParameter.SensitiveLength + 2. * HcalUnitParameter.PassiveCoverThick + ((G4int)(HcalUnitParameter.ifDoubleSidedReadout) + 1) * HcalUnitParameter.AttachThick;
        G4Box *HcalAbsSolid = nullptr;
        G4LogicalVolume *HcalAbsLogical = nullptr;
        G4Box *HcalPCBSolid = nullptr;
        G4LogicalVolume *HcalPCBLogical = nullptr;
        G4Box *HcalPCB_Cu_Solid = nullptr;
        G4LogicalVolume *HcalPCB_Cu_Logical = nullptr;
        G4double HcalXYsize = std::max(HcalCellNumberX * HcalUnitSizeX, HcalCellNumberY * HcalUnitSizeY);
        if (HcalModuleType > 0)
        {
            if (HcalAbsorberThick > 0)
            {
                HcalAbsSolid = new G4Box("HcalAbsSolid", HcalXYsize / 2., HcalXYsize / 2., HcalAbsorberThick / 2.);
                HcalAbsLogical = new G4LogicalVolume(HcalAbsSolid, GetCaloMaterial(HcalAbsorberMatIndex), "HcalAbsLogical");
            }
            if (HcalPCBThick > 0)
            {
                HcalPCBSolid = new G4Box("HcalPCBSolid", HcalXYsize / 2., HcalXYsize / 2., HcalPCBThick / 2.);
                HcalPCBLogical = new G4LogicalVolume(HcalPCBSolid, GetCaloMaterial(HcalPCBMatIndex), "HcalPCBLogical");
            }
            if (HcalPCB_Cu_Thick > 0)
            {
                HcalPCB_Cu_Solid = new G4Box("HcalPCB_Cu_Solid", HcalXYsize / 2., HcalXYsize / 2., HcalPCB_Cu_Thick / 2.);
                HcalPCB_Cu_Logical = new G4LogicalVolume(HcalPCB_Cu_Solid, GetCaloMaterial(HcalPCB_Cu_MatIndex), "HcalPCB_Cu_Logical");
            }
        }
        if (HcalModuleType == 1)
        {
            G4RotationMatrix *HcalUnitInv = new G4RotationMatrix();
            HcalUnitInv->rotateY(180. * deg);
            G4double Zpos = HcalUnitSizeZ / 2.;
            if (EcalModuleType > 0)
                Zpos += 19.9 * EcalLayerNumber / 2. + 230. * mm; // Additional 230 mm (assume including ECAL cover)
            if (IntermediateSizeX * IntermediateSizeY * IntermediateSizeZ > 0)
                Zpos += IntermediateSizeZ;
            auto HcalCellMax = std::to_string(std::max(HcalCellNumberX, HcalCellNumberY));
            G4int HcalCellMaxCount = pow(10, HcalCellMax.length());
            G4int HcalCopyNum = 0;
            Zpos -= HcalUnitSizeZ / 2.;    // Additional cover

            Zpos += HcaltriggerThick;
            if(HcaltriggerThick>0){
                auto HCALtriggerSolid = new G4Box("HCALtriggerSolid", HcalXYsize / 2., HcalXYsize / 2., HcaltriggerThick / 2.);
                auto HCALtriggerLogical = new G4LogicalVolume(HCALtriggerSolid, GetCaloMaterial(HcaltriggerIndex), "HCALtriggerLogical");
                new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), HCALtriggerLogical, "HCALtriggerPhysicalFront", World_Logical, false, 10, ifcheckOverlaps);
            }
            Zpos += HcaltriggerThick + 1500. * mm;

            G4double HCALCoverThick = 2. * mm;
            Zpos += HCALCoverThick / 2.;
            auto HCALCoverSolid = new G4Box("HCALCoverSolid", HcalXYsize / 2., HcalXYsize / 2., HCALCoverThick / 2.);
            auto HCALCoverLogical = new G4LogicalVolume(HCALCoverSolid, GetCaloMaterial(HcalAbsorberMatIndex), "HCALCoverLogical");
            new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), HCALCoverLogical, "HCALCoverPhysicalFront", World_Logical, false, 11, ifcheckOverlaps);
            Zpos += 2. * mm;
            Zpos += (HCALCoverThick + HcalUnitSizeZ) / 2.;
            for (G4int z = 0; z < HcalLayerNumber; z++)
            {
                for (G4int y = 0; y < HcalCellNumberY; y++)
                {
                    HcalCopyNum = (z + 1) * HcalCellMaxCount * HcalCellMaxCount + (y + 1) * HcalCellMaxCount;
                    for (G4int x = 0; x < HcalCellNumberX; x++)
                    {
                        HcalCopyNum++;
                        new CaloUnitVolume("HcalUnit", HcalUnitInv, G4ThreeVector(HcalUnitSizeX * (HcalCellNumberX / 2. - 0.5 - x), HcalUnitSizeY * (HcalCellNumberY / 2. - 0.5 - y), Zpos),
                                           ifcheckOverlaps, World_Logical, HcalUnitLogical, HcalSensitiveLogical, &HcalUnitParameter, false, HcalCopyNum, this);
                    }
                }
                Zpos += (HcalUnitSizeZ + HcalPCBThick) / 2.;
                if (HcalPCBThick > 0)
                    new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), HcalPCBLogical, "HcalPCBPhysical", World_Logical, false, HcalCopyNum / HcalCellMaxCount / HcalCellMaxCount * HcalCellMaxCount * HcalCellMaxCount, ifcheckOverlaps);


                Zpos += (HcalPCBThick + HcalPCB_Cu_Thick) / 2.;
				if(HcalPCB_Cu_Thick>0)
					new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), HcalPCB_Cu_Logical, "HcalPCB_Cu_Physical", World_Logical, false, HcalCopyNum / HcalCellMaxCount / HcalCellMaxCount * HcalCellMaxCount * HcalCellMaxCount, ifcheckOverlaps);


                Zpos += (HcalPCB_Cu_Thick + HcalAbsorberThick) / 2. + HcalPCB_Abs_gap * mm;  // HcalPCBGap 4 mm
                if (HcalAbsorberThick > 0)
                    new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), HcalAbsLogical, "HcalAbsPhysical", World_Logical, false, HcalCopyNum / HcalCellMaxCount / HcalCellMaxCount * HcalCellMaxCount * HcalCellMaxCount, ifcheckOverlaps);
                Zpos += (HcalAbsorberThick + HcalUnitSizeZ) / 2.;
            }
            Zpos -= HcalUnitSizeZ / 2.;
            Zpos += 2. * mm;
            Zpos += HCALCoverThick / 2.;
            new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), HCALCoverLogical, "HCALCoverPhysicalBack", World_Logical, false, 0, ifcheckOverlaps);
        }
        else if (HcalModuleType == 2)
        {
            G4RotationMatrix *HcalUnitInv = new G4RotationMatrix();
            HcalUnitInv->rotateY(180. * deg);
            G4double Zpos = HcalUnitSizeZ / 2.;
            if (EcalModuleType > 0)
                Zpos += (EcalUnitSizeZ + EcalPCBThick + EcalAbsorberThick) * EcalLayerNumber;
            if (IntermediateSizeX * IntermediateSizeY * IntermediateSizeZ > 0)
                Zpos += IntermediateSizeZ;
            auto HcalCellMax = std::to_string(std::max(HcalCellNumberX, HcalCellNumberY));
            G4int HcalCellMaxCount = pow(10, HcalCellMax.length());
            G4int HcalCopyNum = 0;
            for (G4int z = 0; z < HcalLayerNumber; z++)
            {
                for (G4int y = 0; y < HcalCellNumberY; y++)
                {
                    HcalCopyNum = (z + 1) * HcalCellMaxCount * HcalCellMaxCount + (y + 1) * HcalCellMaxCount;
                    for (G4int x = 0; x < HcalCellNumberX; x++)
                    {
                        HcalCopyNum++;
                        new CaloUnitVolume("HcalUnit", HcalUnitInv, G4ThreeVector(HcalUnitSizeX * (HcalCellNumberX / 2. - 0.5 - x), HcalUnitSizeY * (HcalCellNumberY / 2. - 0.5 - y), Zpos),
                                           ifcheckOverlaps, World_Logical, HcalUnitLogical, HcalSensitiveLogical, &HcalUnitParameter, false, HcalCopyNum, this);
                    }
                }
                Zpos += (HcalUnitSizeZ + HcalPCBThick) / 2.;
                if (HcalPCBThick > 0)
                    new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), HcalPCBLogical, "HcalPCBPhysical", World_Logical, false, HcalCopyNum / HcalCellMaxCount / HcalCellMaxCount * HcalCellMaxCount * HcalCellMaxCount, ifcheckOverlaps);
                Zpos += (HcalPCBThick + HcalAbsorberThick) / 2.;  // HcalPCBGap 4 mm
                if (HcalAbsorberThick > 0)
                    new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), HcalAbsLogical, "HcalAbsPhysical", World_Logical, false, HcalCopyNum / HcalCellMaxCount / HcalCellMaxCount * HcalCellMaxCount * HcalCellMaxCount, ifcheckOverlaps);
                Zpos += (HcalAbsorberThick + HcalUnitSizeZ) / 2.;
            }
        }

        // Visualization attributes
        G4VisAttributes *AbsVisAtt = new G4VisAttributes(G4Colour(0.28, 0.82, 0.8, 0.3)); // Light Blue
        AbsVisAtt->SetVisibility(true);
        if (EcalModuleType > 0 && EcalAbsorberThick > 0)
            EcalAbsLogical->SetVisAttributes(AbsVisAtt);
        if (HcalModuleType > 0 && HcalAbsorberThick > 0)
            HcalAbsLogical->SetVisAttributes(AbsVisAtt);
        HcalAbsLogical->SetVisAttributes(AbsVisAtt);
        AbsVisAtt =new G4VisAttributes(G4Colour(0.9,0,0));
		if(HcalPCB_Cu_Thick > 0)
			HcalPCB_Cu_Logical->SetVisAttributes(AbsVisAtt);
        AbsVisAtt =new G4VisAttributes(G4Colour(0,0.9,0));
        HcalPCBLogical->SetVisAttributes(AbsVisAtt);
        World_Logical->SetVisAttributes(G4VisAttributes::GetInvisible());

        return World_Physical;
    }

    void DetectorConstruction::ConstructSDandField()
    {
        // ECAL SD
        if (EcalSensitiveLogical != nullptr)
        {
            if (!fEcalUnitSD.Get())
            {
                G4cout << "Construction /CaloDet/EcalSD" << G4endl;
                EcalUnitSD *ecalunitSD = new EcalUnitSD("/CaloDet/EcalSD");
                fEcalUnitSD.Put(ecalunitSD);
            }
            G4SDManager::GetSDMpointer()->AddNewDetector(fEcalUnitSD.Get());
            SetSensitiveDetector(EcalSensitiveLogical, fEcalUnitSD.Get());
        }

        // HCAL SD
        if (HcalSensitiveLogical != nullptr)
        {
            if (!fHcalUnitSD.Get())
            {
                G4cout << "Construction /CaloDet/HcalSD" << G4endl;
                HcalUnitSD *hcalunitSD = new HcalUnitSD("/CaloDet/HcalSD");
                fHcalUnitSD.Put(hcalunitSD);
            }
            G4SDManager::GetSDMpointer()->AddNewDetector(fHcalUnitSD.Get());
            // SetSensitiveDetector(HcalSensitiveLogical, fHcalUnitSD.Get());
            G4UserLimits *userLimits = new G4UserLimits(); // 最大步长 1 mm
            // userLimits->SetMaxAllowedStep(0.1 * mm);
            // SiPMSensitiveLogical->SetUserLimits(userLimits);
            // HcalSensitiveLogical->SetUserLimits(userLimits);
            // Dig_out_Logical->SetUserLimits(userLimits);
            SiPMSensitiveLogical->SetSensitiveDetector(fHcalUnitSD.Get());
            Dig_out_Logical->SetSensitiveDetector(fHcalUnitSD.Get());
            HcalSensitiveLogical->SetSensitiveDetector(fHcalUnitSD.Get()); 
            
            
            // HCAL SiPM SD
            // if (SiPMSensitiveLogical != nullptr)
            // {
                // G4SDManager::GetSDMpointer()->AddNewDetector(fHcalUnitSD.Get());
                    // SetSensitiveDetector(SiPMSensitiveLogical, fHcalUnitSD.Get());
                // G4cout << "diaohb  " << SiPMSensitiveLogical->GetName() << "  " << HcalSensitiveLogical->GetName() << G4endl;
            // }
        }

        // Field
        if (!fFieldMessenger.Get())
        {
            G4ThreeVector fieldValue = G4ThreeVector();
            G4GlobalMagFieldMessenger *msg = new G4GlobalMagFieldMessenger(fieldValue);
            msg->SetVerboseLevel(1);
            G4AutoDelete::Register(msg);
            fFieldMessenger.Put(msg);
        }
    }

    void DetectorConstruction::ResetCaloLogical()
    {
        EcalUnitLogical = nullptr;
        HcalUnitLogical = nullptr;
    }

    void DetectorConstruction::SetEcalCellNumberX(G4int fEcalCellX)
    {
        EcalCellNumberX = fEcalCellX;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetEcalCellNumberY(G4int fEcalCellY)
    {
        EcalCellNumberY = fEcalCellY;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetEcalLayerNumber(G4int fEcalLayer)
    {
        EcalLayerNumber = fEcalLayer;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetHcalCellNumberX(G4int fHcalCellX)
    {
        HcalCellNumberX = fHcalCellX;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetHcalCellNumberY(G4int fHcalCellY)
    {
        HcalCellNumberY = fHcalCellY;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetHcalLayerNumber(G4int fHcalLayer)
    {
        HcalLayerNumber = fHcalLayer;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetEcalSensitiveThick(G4double ecalSensitiveThick)
    {
        EcalUnitParameter.SensitiveLength = ecalSensitiveThick;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetHcalSensitiveThick(G4double hcalSensitiveThick)
    {
        HcalUnitParameter.SensitiveLength = hcalSensitiveThick;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetEcalPassiveThick(G4double ecalPassiveThick)
    {
        EcalUnitParameter.PassiveSideThick = ecalPassiveThick;
        EcalUnitParameter.PassiveCoverThick = ecalPassiveThick;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetHcalPassiveThick(G4double hcalPassiveThick)
    {
        HcalUnitParameter.PassiveSideThick = hcalPassiveThick;
        HcalUnitParameter.PassiveCoverThick = hcalPassiveThick;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetEcalAbsThick(G4double ecalabsThick)
    {
        EcalAbsorberThick = ecalabsThick;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetHcalAbsThick(G4double hcalabsThick)
    {
        HcalAbsorberThick = hcalabsThick;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetUpstreamObjThick(G4double fUpThick)
    {
        UpstreamSizeZ = fUpThick;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetIntermediateObjThick(G4double fInterThick)
    {
        IntermediateSizeZ = fInterThick;
        ResetCaloLogical();
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }

    void DetectorConstruction::SetEcalStepTimeLimit(G4double fTime)
    {
        EcalStepTimeLimit = fTime;
    }

    void DetectorConstruction::SetHcalStepTimeLimit(G4double fTime)
    {
        HcalStepTimeLimit = fTime;
    }

    void DetectorConstruction::SetCaloLogicalVolume(G4LogicalVolume *&myCaloLogical, G4LogicalVolume *fCaloLogical)
    {
        if (&myCaloLogical == &EcalUnitLogical)
            EcalUnitLogical = fCaloLogical;
        else if (&myCaloLogical == &HcalUnitLogical)
            HcalUnitLogical = fCaloLogical;
        else if (&myCaloLogical == &EcalSensitiveLogical)
            EcalSensitiveLogical = fCaloLogical;
        else if (&myCaloLogical == &HcalSensitiveLogical)
            HcalSensitiveLogical = fCaloLogical;
    }
    void DetectorConstruction::SetSiPMLogicalVolume(G4LogicalVolume *fdigoutLogical, G4LogicalVolume *fCaloLogical)
    {
        SiPMSensitiveLogical = fCaloLogical;
        Dig_out_Logical = fdigoutLogical;
    }
}
