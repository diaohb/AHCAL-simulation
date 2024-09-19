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
        HcalUnitParameter.AttachThick = 0 * mm;
        HcalUnitParameter.Sensitive_dig_out_X = 5.5 * mm;
        HcalUnitParameter.Sensitive_dig_out_Y = HcalUnitParameter.Sensitive_dig_out_X;
        HcalUnitParameter.Sensitive_dig_out_Z = 1.1 * mm;
        HcalUnitParameter.SensitiveMatIndex = PlasticSciHCAL;
        HcalUnitParameter.Sensitive_dig_out_MatIndex = SiPM;
        HcalUnitParameter.PassiveMatIndex = ESR;
        HcalUnitParameter.AttachMatIndex = Quartz;
        EcalAbsorberThick = 3.2 * mm; // 3.2 mm for ScW ECAL
        HcalAbsorberThick = 20.0 * mm;
        EcalPCBThick = 2.0 * mm;
        HcalPCBThick = 3.0 * mm;  //2.5mm *4/5 for PCB, 1mm for component
        HcalPCB_Cu_Thick = 0.5 * mm; //2.5mm *1/5
        HcalPCB_Abs_gap = 3.5 * mm; //4mm-1mm+0.5mm
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
        HcalCellNumberX = 1;
        HcalCellNumberY = 1;
        HcalLayerNumber = 1;
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

        // G4Material *FR4Mat = new G4Material("FR4", density = 1.86 * g / cm3, nComponents = 2);
        // FR4Mat->AddMaterial(QuartzMat, fracMass = 52.8 * perCent);
        // FR4Mat->AddMaterial(EpoxyMat, fracMass = 47.2 * perCent);
        
        //PCB:Cu=4:1, frac of V, thick=2.5mm
        // components_thick=3mm/3=1mm, mat=FR4
        //total FR4:Cu=3mm:0.5mm, density=(3*1.86+0.5*8.9)/(3+0.5)=2.86

        G4Material *FR4Mat = new G4Material("FR4", density = 1.86 * g / cm3, nComponents = 2);
        FR4Mat->AddMaterial(QuartzMat, fracMass = 50 * perCent);
        FR4Mat->AddMaterial(EpoxyMat, fracMass = 50 * perCent);
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
        G4MaterialPropertiesTable *myMPT2 = new G4MaterialPropertiesTable();

        G4Material *PWOMat = nistManager->FindOrBuildMaterial("G4_PbWO4");
        PWOMat->GetIonisation()->SetBirksConstant(0.0333333 * mm / MeV);
        MaterialStore.push_back(PWOMat); // 13

        G4Material *BGOMat = nistManager->FindOrBuildMaterial("G4_BGO");
        BGOMat->GetIonisation()->SetBirksConstant(0.008415 * mm / MeV);
        MaterialStore.push_back(BGOMat); // 14

        G4Material *PlasticSciHCALMat = nistManager->BuildMaterialWithNewDensity("polystyrene", "G4_POLYSTYRENE", 1.032 * g / cm3);
        PlasticSciHCALMat->GetIonisation()->SetBirksConstant(0.07943 * mm / MeV);
        MaterialStore.push_back(PlasticSciHCALMat); // 15

        G4MaterialPropertiesTable* myMPT1 = new G4MaterialPropertiesTable();
        std::vector<double> photonEnergy = {2 * eV, 3 * eV};
        std::vector<double> refractiveIndex = {1.58, 1.58};
        std::vector<double> absorb = {210*cm, 210*cm};
        myMPT1->AddProperty("RINDEX", photonEnergy, refractiveIndex);
        myMPT1->AddProperty("ABSLENGTH", photonEnergy, absorb,false,true);
        // Check that group velocity is calculated from RINDEX
        if (myMPT1->GetProperty("RINDEX")->GetVectorLength() !=
            myMPT1->GetProperty("GROUPVEL")->GetVectorLength())
        {
            G4ExceptionDescription ed;
            ed << "Error calculating group velocities. Incorrect number of entries "
                  "in group velocity material property vector.";
            G4Exception("OpNovice::OpNoviceDetectorConstruction", "OpNovice001",
                        FatalException, ed);
        }
        myMPT1->AddConstProperty("SCINTILLATIONYIELD", 10000. / MeV);
        myMPT1->AddConstProperty("SCINTILLATIONRISETIME1", 0.9 * ns);
        myMPT1->AddConstProperty("SSCINTILLATIONTIMECONSTANT1", 2.1 * ns);
        // myMPT1->AddConstProperty("REFRACTIVE", 1.58);

        myMPT1->DumpTable();
        PlasticSciHCALMat->SetMaterialPropertiesTable(myMPT1);

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
        SciGlassMat->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);
        MaterialStore.push_back(SciGlassMat); // 17

        G4Material *SiPMMat = nistManager->FindOrBuildMaterial("G4_AIR");
        MaterialStore.push_back(SiPMMat); // 18

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

        // HCAL
        G4double HcalUnitSizeX = HcalUnitParameter.SensitiveX + 2. * HcalUnitParameter.PassiveSideThick;
        G4double HcalUnitSizeY = HcalUnitParameter.SensitiveY + 2. * HcalUnitParameter.PassiveSideThick;
        G4double HcalUnitSizeZ = HcalUnitParameter.SensitiveLength + 2. * HcalUnitParameter.PassiveCoverThick + ((G4int)(HcalUnitParameter.ifDoubleSidedReadout) + 1) * HcalUnitParameter.AttachThick;

        G4RotationMatrix *HcalUnitInv = new G4RotationMatrix();
        HcalUnitInv->rotateY(180. * deg);
        // new CaloUnitVolume("HcalUnit", HcalUnitInv, G4ThreeVector(0, 0, 0),ifcheckOverlaps, World_Logical, HcalUnitLogical,HcalSensitiveLogical, &HcalUnitParameter, false, 0, this);
        /////////////////////////////////////////////////////////////////////////////////
            G4double Sensitive_x = HcalUnitParameter.SensitiveX / 2.;
            G4double Sensitive_y = HcalUnitParameter.SensitiveY / 2.;
            G4double Sensitive_z = HcalUnitParameter.SensitiveLength / 2.;
            G4double Sensitive_dig_out_x = HcalUnitParameter.Sensitive_dig_out_X / 2.;
            G4double Sensitive_dig_out_y = HcalUnitParameter.Sensitive_dig_out_Y / 2.;
            G4double Sensitive_dig_out_z = HcalUnitParameter.Sensitive_dig_out_Z / 2.;
            G4double Passive_x = Sensitive_x + HcalUnitParameter.PassiveSideThick;
            G4double Passive_y = Sensitive_y + HcalUnitParameter.PassiveSideThick;
            G4double Passive_z = Sensitive_z + HcalUnitParameter.PassiveCoverThick;
            G4double Housing_x = Passive_x;
            G4double Housing_y = Passive_y;
            G4double Housing_z = Passive_z;

            // Box for housing
            G4Box *SolidHousing = new G4Box("SolidHousing", Housing_x, Housing_y, Housing_z);
            G4LogicalVolume *HousingLogical = new G4LogicalVolume(SolidHousing, G4Material::GetMaterial("G4_Galactic"), "HousingLogical");

            // Passive
            G4Box *SolidPassive = new G4Box("SolidPassive", Passive_x, Passive_y, Passive_z);
            G4LogicalVolume *PassiveLogical = new G4LogicalVolume(SolidPassive, GetCaloMaterial(HcalUnitParameter.PassiveMatIndex), "PassiveLogical");
            G4VPhysicalVolume *Passive_phys = new G4PVPlacement(0, G4ThreeVector(0, 0, (G4int)(!HcalUnitParameter.ifDoubleSidedReadout) * HcalUnitParameter.AttachThick / 2.), PassiveLogical, "PassivePhysical", HousingLogical, false, 0, ifcheckOverlaps);

            // Sensitive
            G4Box *SolidSensitive = new G4Box("SolidSensitive", Sensitive_x, Sensitive_y, Sensitive_z);
            G4LogicalVolume *SensitiveLogical = new G4LogicalVolume(SolidSensitive, GetCaloMaterial(HcalUnitParameter.SensitiveMatIndex), "SensitiveLogical");
            G4VPhysicalVolume *Sensitive_phys = new G4PVPlacement(0, G4ThreeVector(), SensitiveLogical, "SensitivePhysical", PassiveLogical, false, 0, ifcheckOverlaps);

            // Sensitive_dig_out
            G4Box *SolidSensitive_dig_out = new G4Box("SolidSensitive_dig_out", Sensitive_dig_out_x, Sensitive_dig_out_y, Sensitive_dig_out_z);
            G4LogicalVolume *Sensitive_dig_out_Logical = new G4LogicalVolume(SolidSensitive_dig_out, GetCaloMaterial(HcalUnitParameter.Sensitive_dig_out_MatIndex), "Sensitive_dig_out_Logical");
            G4VPhysicalVolume *Sensitive_dig_out_phys = new G4PVPlacement(0, G4ThreeVector(0, 0, -Sensitive_z + Sensitive_dig_out_z), Sensitive_dig_out_Logical, "Sensitive_dig_out_Physical", SensitiveLogical, false, 0, ifcheckOverlaps);

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
            SetCaloLogicalVolume(HcalSensitiveLogical, SensitiveLogical);
            SetCaloLogicalVolume(HcalUnitLogical, HousingLogical);
        /////////////////////////////////////////////////////////////////////////////////
        // Visualization attributes
        G4VisAttributes *AbsVisAtt = new G4VisAttributes(G4Colour(0.28, 0.82, 0.8, 0.3)); // Light Blue
        AbsVisAtt->SetVisibility(true);
        World_Logical->SetVisAttributes(G4VisAttributes::GetInvisible());

        return World_Physical;
    }

    void DetectorConstruction::ConstructSDandField()
    {
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
            SetSensitiveDetector(HcalSensitiveLogical, fHcalUnitSD.Get());
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
}
