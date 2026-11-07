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

#include "CADMesh.hh"

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
        HcalUnitParameter.Sensitive_dig_out_X = 5.5 * mm;
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

        pcbParameter.AirIndex = Air;
        pcbParameter.PCBMatIndex = FR4;
        pcbParameter.SR0603MatIndex = SR0603;
        pcbParameter.SC1206MatIndex = SC1206;
        pcbParameter.SC0402MatIndex = SC0402;
        pcbParameter.SN74LVC125AMatIndex = SN74LVC125A;
        pcbParameter.STC3528MatIndex = STC3528;
        pcbParameter.STC3528MatIndex = STC3528;
        pcbParameter._8SOPMatIndex = _8SOP;
        pcbParameter.SOIC8MatIndex = SOIC8;
        pcbParameter.SOP58P810MatIndex = SOP58P810;
        pcbParameter.SD0805MatIndex = SD0805;
        pcbParameter.PW24MatIndex = PW24;
        pcbParameter.KS5MatIndex = KS5;
        pcbParameter.TMP117MatIndex = TMP117;
        pcbParameter.SOIC127P600MatIndex = SOIC127P600;
        pcbParameter.RJ6MatIndex = RJ6;
        pcbParameter.RT3MatIndex = RT3;
        pcbParameter.SR0402MatIndex = SR0402;
        pcbParameter.D8MatIndex = D8;
        pcbParameter.BB02MatIndex = BB02;
        pcbParameter.FH29BWMatIndex = FH29BW;
        pcbParameter.STC3216MatIndex = STC3216;
        pcbParameter._0603MatIndex = _0603;
        pcbParameter.BGAMatIndex = BGA;
        pcbParameter.MCHMatIndex = MCH;
        pcbParameter.SC0603MatIndex = SC0603;

        EcalAbsorberThick = 3.2 * mm; // 3.2 mm for ScW ECAL
        HcalAbsorberThick = 20.0 * mm;
        EcalPCBThick = 2.0 * mm;

        HcaltriggerThick = 60.0 * mm;
        HcaltriggerIndex = PlasticSciHCAL;
        HcalPCBThick = 4.5 * mm; // 2.5mm *4/5 for PCB, 1mm for component
        // HcalPCBThick = 2.5 * mm;  //2.5mm *4/5 for PCB, 1mm for component
        HcalPCB_Cu_Thick = 0. * mm;                                   // 2.5mm *1/5
        HcalPCB_Abs_gap = 6.5 * mm - HcalPCBThick - HcalPCB_Cu_Thick; // 4mm-1mm

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
        G4Element *EleCu = nistManager->FindOrBuildElement("Cu");
        G4Element *EleFe = nistManager->FindOrBuildElement("Fe");
        G4Element *EleAg = nistManager->FindOrBuildElement("Ag");
        G4Element *EleAu = nistManager->FindOrBuildElement("Au");
        G4Element *EleZn = nistManager->FindOrBuildElement("Zn");
        G4Element *EleCa = nistManager->FindOrBuildElement("Ca");
        G4Element *EleNi = nistManager->FindOrBuildElement("Ni");
        G4Element *ElePd = nistManager->FindOrBuildElement("Pd");
        G4Element *EleAl = nistManager->FindOrBuildElement("Al");
        G4Element *EleTi = nistManager->FindOrBuildElement("Ti");
        G4Element *EleBa = nistManager->FindOrBuildElement("Ba");
        G4Element *EleSn = nistManager->FindOrBuildElement("Sn");
        G4Element *EleMg = nistManager->FindOrBuildElement("Mg");
        G4Element *EleTa = nistManager->FindOrBuildElement("Ta");

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

        G4Material *CMat = nistManager->FindOrBuildMaterial("G4_C");
        G4Material *HMat = nistManager->FindOrBuildMaterial("G4_H");
        G4Material *OMat = nistManager->FindOrBuildMaterial("G4_O");
        G4Material *ZnMat = nistManager->FindOrBuildMaterial("G4_Zn");
        G4Material *AuMat = nistManager->FindOrBuildMaterial("G4_Au");
        G4Material *AgMat = nistManager->FindOrBuildMaterial("G4_Ag");
        G4Material *PdMat = nistManager->FindOrBuildMaterial("G4_Pd");
        G4Material *PMat = nistManager->FindOrBuildMaterial("G4_P");
        G4Material *CaMat = nistManager->FindOrBuildMaterial("G4_Ca");
        G4Material *NiMat = nistManager->FindOrBuildMaterial("G4_Ni");
        // G4Material *CuMat = nistManager->FindOrBuildMaterial("G4_Cu");

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

        // PCB:Cu=4:1, frac of V, thick=2.5mm
        //  components_thick=3mm/3=1mm, mat=FR4
        // total FR4:Cu=3mm:0.5mm, density=(3*1.86+0.5*8.9)/(3+0.5)=2.86
        double Cu_frac_vol = 0. / 2.5;
        double density_pcb = 1.86 * (1 - Cu_frac_vol) + 8.96 * Cu_frac_vol;
        double frac_mass_cu = Cu_frac_vol * 8.96 / density_pcb;
        G4Material *FR4Mat = new G4Material("FR4", density = density_pcb * g / cm3, nComponents = 3);
        FR4Mat->AddMaterial(QuartzMat, fracMass = 52.8 * perCent * (1 - frac_mass_cu));
        FR4Mat->AddMaterial(EpoxyMat, fracMass = 47.2 * perCent * (1 - frac_mass_cu));
        FR4Mat->AddMaterial(CuMat, fracMass = frac_mass_cu);
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

        G4Material *BaTiO3Mat = new G4Material("BaTiO3", density = 6.02 * g / cm3, nElements = 3);
        BaTiO3Mat->AddElement(EleBa, nAtoms = 1);
        BaTiO3Mat->AddElement(EleTi, nAtoms = 1);
        BaTiO3Mat->AddElement(EleO, nAtoms = 3);
        G4Material *Al2O3Mat = new G4Material("Al2O3", density = 4 * g / cm3, nElements = 2);
        Al2O3Mat->AddElement(EleAl, nAtoms = 2);
        Al2O3Mat->AddElement(EleO, nAtoms = 3);
        G4Material *CaOMat = new G4Material("CaO", density = 3.35 * g / cm3, nElements = 2);
        CaOMat->AddElement(EleCa, nAtoms = 1);
        CaOMat->AddElement(EleO, nAtoms = 1);
        G4Material *MnOMat = new G4Material("MnO", density = 5.45 * g / cm3, nElements = 2);
        MnOMat->AddElement(EleMn, nAtoms = 1);
        MnOMat->AddElement(EleO, nAtoms = 1);
        G4Material *MnO2Mat = new G4Material("MnO2", density = 5.03 * g / cm3, nElements = 2);
        MnO2Mat->AddElement(EleMn, nAtoms = 1);
        MnO2Mat->AddElement(EleO, nAtoms = 2);
        G4Material *MgOMat = new G4Material("MgO", density = 3.58 * g / cm3, nElements = 2);
        MgOMat->AddElement(EleMn, nAtoms = 1);
        MgOMat->AddElement(EleO, nAtoms = 1);
        G4Material *SiO2Mat = new G4Material("SiO2", density = 2.5 * g / cm3, nElements = 2);
        SiO2Mat->AddElement(EleSi, nAtoms = 1);
        SiO2Mat->AddElement(EleO, nAtoms = 1);
        // SR0603,
        G4Material *SR0603Mat = new G4Material("SR0603", density = 3.9 * g / cm3, nComponents = 10);
        SR0603Mat->AddMaterial(Al2O3Mat, fracMass = 74.04 * perCent);
        SR0603Mat->AddMaterial(CaOMat, fracMass = 4.62 * perCent);
        SR0603Mat->AddMaterial(MgOMat, fracMass = 0.38 * perCent);
        SR0603Mat->AddElement(EleAg, fracMass = 1.51 * perCent);
        SR0603Mat->AddElement(ElePd, fracMass = 0.12 * perCent);
        SR0603Mat->AddElement(EleCu, fracMass = 10.24 * perCent);
        SR0603Mat->AddElement(EleNi, fracMass = 2.55 * perCent);
        SR0603Mat->AddElement(EleSn, fracMass = 2.51 * perCent);
        SR0603Mat->AddMaterial(SiO2Mat, fracMass = 2.91 * perCent);
        SR0603Mat->AddMaterial(EpoxyMat, fracMass = 1.12 * perCent);
        MaterialStore.push_back(SR0603Mat);
        // SC1206,
        G4Material *SC1206Mat = new G4Material("SC1206", density = 3.8 * g / cm3, nComponents = 4);
        SC1206Mat->AddElement(EleCu, fracMass = 4.12 * perCent);
        SC1206Mat->AddElement(EleNi, fracMass = 9.96 * perCent);
        SC1206Mat->AddElement(EleSn, fracMass = 1.34 * perCent);
        SC1206Mat->AddMaterial(BaTiO3Mat, fracMass = 84.58 * perCent);
        MaterialStore.push_back(SC1206Mat);
        // SC0402,
        G4Material *SC0402Mat = new G4Material("SC0402", density = 3.8 * g / cm3, nComponents = 4);
        SC0402Mat->AddElement(EleCu, fracMass = 4.29 * perCent);
        SC0402Mat->AddElement(EleNi, fracMass = 7.14 * perCent);
        SC0402Mat->AddElement(EleSn, fracMass = 5.00 * perCent);
        SC0402Mat->AddMaterial(BaTiO3Mat, fracMass = 83.57 * perCent);
        MaterialStore.push_back(SC0402Mat);
        // SN74LVC125A,
        G4Material *SN74LVC125AMat = new G4Material("SN74LVC125A", density = 3.64 * g / cm3, nComponents = 10);
        SN74LVC125AMat->AddElement(EleCu, fracMass = 50.03 * perCent);
        SN74LVC125AMat->AddElement(EleSi, fracMass = 39.53 * perCent);
        SN74LVC125AMat->AddElement(EleFe, fracMass = 1.33 * perCent);
        SN74LVC125AMat->AddElement(EleAg, fracMass = 0.07 * perCent);
        SN74LVC125AMat->AddElement(EleNi, fracMass = 2.25 * perCent);
        SN74LVC125AMat->AddElement(EleC, fracMass = 0.23 * perCent);
        SN74LVC125AMat->AddElement(EleP, fracMass = 0.01 * perCent);
        SN74LVC125AMat->AddElement(ElePd, fracMass = 0.09 * perCent);
        SN74LVC125AMat->AddElement(EleAu, fracMass = 0.02 * perCent);
        SN74LVC125AMat->AddMaterial(EpoxyMat, fracMass = 6.44 * perCent);
        MaterialStore.push_back(SN74LVC125AMat);
        // STC3528,
        G4Material *STC3528Mat = new G4Material("STC3528", density = 7 * g / cm3, nComponents = 11);
        STC3528Mat->AddElement(EleTa, fracMass = 21.86 * perCent);
        STC3528Mat->AddMaterial(MnO2Mat, fracMass = 14.34 * perCent);
        STC3528Mat->AddElement(EleC, fracMass = 9.88 * perCent);
        STC3528Mat->AddElement(EleAg, fracMass = 7.44 * perCent);
        STC3528Mat->AddElement(EleF, fracMass = 0.06 * perCent);
        STC3528Mat->AddElement(EleCu, fracMass = 7.79 * perCent);
        STC3528Mat->AddElement(EleFe, fracMass = 0.36 * perCent);
        STC3528Mat->AddElement(EleZn, fracMass = 2.07 * perCent);
        STC3528Mat->AddElement(EleNi, fracMass = 2.31 * perCent);
        STC3528Mat->AddElement(EleSn, fracMass = 1.21 * perCent);
        STC3528Mat->AddMaterial(SiO2Mat, fracMass = 32.68 * perCent);
        MaterialStore.push_back(STC3528Mat);
        // PW0016A,
        G4Material *PW0016AMat = new G4Material("PW0016A", density = 23.80 * g / cm3, nComponents = 9);
        PW0016AMat->AddElement(EleCu, fracMass = 46.86 * perCent);
        PW0016AMat->AddElement(EleSi, fracMass = 43.84 * perCent);
        PW0016AMat->AddElement(EleFe, fracMass = 1.13 * perCent);
        PW0016AMat->AddElement(EleAg, fracMass = 0.34 * perCent);
        PW0016AMat->AddElement(EleNi, fracMass = 0.16 * perCent);
        PW0016AMat->AddElement(EleC, fracMass = 0.25 * perCent);
        PW0016AMat->AddElement(EleP, fracMass = 0.04 * perCent);
        PW0016AMat->AddElement(EleZn, fracMass = 0.06 * perCent);
        PW0016AMat->AddMaterial(EpoxyMat, fracMass = 7.32 * perCent);
        MaterialStore.push_back(PW0016AMat);
        // _8SOP,
        G4Material *_8SOPMat = new G4Material("_8SOP", density = 1.37 * g / cm3, nComponents = 8);
        _8SOPMat->AddElement(EleCu, fracMass = 43.36 * perCent);
        _8SOPMat->AddElement(EleSi, fracMass = 44.31 * perCent);
        _8SOPMat->AddElement(EleAu, fracMass = 0.43 * perCent);
        _8SOPMat->AddElement(EleAg, fracMass = 1.66 * perCent);
        _8SOPMat->AddElement(EleNi, fracMass = 1.45 * perCent);
        _8SOPMat->AddElement(EleC, fracMass = 0.10 * perCent);
        _8SOPMat->AddElement(EleZn, fracMass = 3.84 * perCent);
        _8SOPMat->AddMaterial(EpoxyMat, fracMass = 4.86 * perCent);
        MaterialStore.push_back(_8SOPMat);
        // SOIC8,
        G4Material *SOIC8Mat = new G4Material("SOIC8", density = 3.20 * g / cm3, nComponents = 9);
        SOIC8Mat->AddElement(EleCu, fracMass = 37.0 * perCent);
        SOIC8Mat->AddElement(EleSi, fracMass = 51.4 * perCent);
        SOIC8Mat->AddElement(EleFe, fracMass = 0.81 * perCent);
        SOIC8Mat->AddElement(EleAg, fracMass = 0.17 * perCent);
        SOIC8Mat->AddElement(EleNi, fracMass = 1.83 * perCent);
        SOIC8Mat->AddElement(EleC, fracMass = 0.30 * perCent);
        SOIC8Mat->AddElement(EleP, fracMass = 0.01 * perCent);
        SOIC8Mat->AddElement(EleZn, fracMass = 0.05 * perCent);
        SOIC8Mat->AddMaterial(EpoxyMat, fracMass = 8.43 * perCent);
        MaterialStore.push_back(SOIC8Mat);
        // SOP58P810,
        G4Material *SOP58P810Mat = new G4Material("SOP58P810", density = 3.20 * g / cm3, nComponents = 10);
        SOP58P810Mat->AddElement(EleCu, fracMass = 42.50 * perCent);
        SOP58P810Mat->AddElement(EleSi, fracMass = 47.46 * perCent);
        SOP58P810Mat->AddElement(EleFe, fracMass = 1.00 * perCent);
        SOP58P810Mat->AddElement(EleAg, fracMass = 0.16 * perCent);
        SOP58P810Mat->AddElement(EleNi, fracMass = 1.12 * perCent);
        SOP58P810Mat->AddElement(EleC, fracMass = 0.16 * perCent);
        SOP58P810Mat->AddElement(EleP, fracMass = 0.01 * perCent);
        SOP58P810Mat->AddElement(EleZn, fracMass = 0.04 * perCent);
        SOP58P810Mat->AddElement(ElePd, fracMass = 0.05 * perCent);
        SOP58P810Mat->AddMaterial(EpoxyMat, fracMass = 7.50 * perCent);
        MaterialStore.push_back(SOP58P810Mat);
        // SD0805, // LED
        MaterialStore.push_back(FR4Mat);
        // PW24,
        G4Material *KS5Mat = new G4Material("KS5", density = 2.75 * g / cm3, nComponents = 9);
        KS5Mat->AddElement(EleCu, fracMass = 45.77 * perCent);
        KS5Mat->AddElement(EleSi, fracMass = 44.68 * perCent);
        KS5Mat->AddElement(EleFe, fracMass = 1.22 * perCent);
        KS5Mat->AddElement(EleAg, fracMass = 0.26 * perCent);
        KS5Mat->AddElement(EleNi, fracMass = 0.16 * perCent);
        KS5Mat->AddElement(EleC, fracMass = 0.26 * perCent);
        KS5Mat->AddElement(EleP, fracMass = 0.07 * perCent);
        KS5Mat->AddElement(EleZn, fracMass = 0.09 * perCent);
        KS5Mat->AddMaterial(EpoxyMat, fracMass = 7.49 * perCent);
        MaterialStore.push_back(KS5Mat);
        // KS5,
        G4Material *PW24Mat = new G4Material("PW24", density = 2.75 * g / cm3, nComponents = 9);
        PW24Mat->AddElement(EleCu, fracMass = 49.35 * perCent);
        PW24Mat->AddElement(EleSi, fracMass = 43.30 * perCent);
        PW24Mat->AddElement(EleFe, fracMass = 1.18 * perCent);
        PW24Mat->AddElement(EleNi, fracMass = 0.56 * perCent);
        PW24Mat->AddElement(EleC, fracMass = 0.09 * perCent);
        PW24Mat->AddElement(EleP, fracMass = 0.01 * perCent);
        PW24Mat->AddElement(EleZn, fracMass = 0.07 * perCent);
        PW24Mat->AddElement(EleAu, fracMass = 0.59 * perCent);
        PW24Mat->AddMaterial(EpoxyMat, fracMass = 4.85 * perCent);
        MaterialStore.push_back(PW24Mat);
        // TMP117,
        G4Material *TMP117Mat = new G4Material("TMP117", density = 4 * g / cm3, nComponents = 11);
        TMP117Mat->AddElement(EleCu, fracMass = 58.44 * perCent);
        TMP117Mat->AddElement(EleSi, fracMass = 32.26 * perCent);
        TMP117Mat->AddElement(EleFe, fracMass = 1.38 * perCent);
        TMP117Mat->AddElement(EleAg, fracMass = 2.01 * perCent);
        TMP117Mat->AddElement(EleNi, fracMass = 1.58 * perCent);
        TMP117Mat->AddElement(EleC, fracMass = 0.09 * perCent);
        TMP117Mat->AddElement(EleP, fracMass = 0.02 * perCent);
        TMP117Mat->AddElement(EleZn, fracMass = 0.09 * perCent);
        TMP117Mat->AddElement(ElePd, fracMass = 0.07 * perCent);
        TMP117Mat->AddElement(EleAu, fracMass = 0.02 * perCent);
        TMP117Mat->AddMaterial(EpoxyMat, fracMass = 4.04 * perCent);
        MaterialStore.push_back(TMP117Mat);
        // SOIC127P600,
        G4Material *SOIC127P600Mat = new G4Material("SOIC127P600", density = 4 * g / cm3, nComponents = 9);
        SOIC127P600Mat->AddElement(EleCu, fracMass = 29.61 * perCent);
        SOIC127P600Mat->AddElement(EleSi, fracMass = 59.14 * perCent);
        SOIC127P600Mat->AddElement(EleFe, fracMass = 0.79 * perCent);
        SOIC127P600Mat->AddElement(EleAg, fracMass = 0.44 * perCent);
        SOIC127P600Mat->AddElement(EleNi, fracMass = 0.11 * perCent);
        SOIC127P600Mat->AddElement(EleC, fracMass = 0.33 * perCent);
        SOIC127P600Mat->AddElement(EleP, fracMass = 0.04 * perCent);
        SOIC127P600Mat->AddElement(EleZn, fracMass = 0.06 * perCent);
        // SOIC127P600Mat->AddElement(ElePd, fracMass = 0.004 * perCent);
        // SOIC127P600Mat->AddElement(EleAu, fracMass = 0.00 * perCent);
        SOIC127P600Mat->AddMaterial(EpoxyMat, fracMass = 9.48 * perCent);
        MaterialStore.push_back(SOIC127P600Mat);
        // RJ6,
        MaterialStore.push_back(FR4Mat);
        // RT3,
        MaterialStore.push_back(FR4Mat);
        // SR0402,
        G4Material *SR0402Mat = new G4Material("SR0402", density = 3.9 * g / cm3, nComponents = 10);
        SR0402Mat->AddMaterial(Al2O3Mat, fracMass = 66.48 * perCent);
        SR0402Mat->AddMaterial(CaOMat, fracMass = 0.42 * perCent);
        SR0402Mat->AddMaterial(MgOMat, fracMass = 0.35 * perCent);
        SR0402Mat->AddElement(EleAg, fracMass = 2.00 * perCent);
        SR0402Mat->AddElement(ElePd, fracMass = 0.16 * perCent);
        SR0402Mat->AddElement(EleCu, fracMass = 17.55 * perCent);
        SR0402Mat->AddElement(EleNi, fracMass = 4.39 * perCent);
        SR0402Mat->AddElement(EleSn, fracMass = 4.30 * perCent);
        SR0402Mat->AddMaterial(SiO2Mat, fracMass = 2.01 * perCent);
        SR0402Mat->AddMaterial(EpoxyMat, fracMass = 2.34 * perCent);
        MaterialStore.push_back(SR0402Mat);
        // D8,
        G4Material *D8Mat = new G4Material("D8", density = 3.20 * g / cm3, nComponents = 9);
        D8Mat->AddElement(EleCu, fracMass = 37.0 * perCent);
        D8Mat->AddElement(EleSi, fracMass = 51.4 * perCent);
        D8Mat->AddElement(EleFe, fracMass = 0.81 * perCent);
        D8Mat->AddElement(EleAg, fracMass = 0.17 * perCent);
        D8Mat->AddElement(EleNi, fracMass = 1.83 * perCent);
        D8Mat->AddElement(EleC, fracMass = 0.30 * perCent);
        D8Mat->AddElement(EleP, fracMass = 0.01 * perCent);
        D8Mat->AddElement(EleZn, fracMass = 0.05 * perCent);
        D8Mat->AddMaterial(EpoxyMat, fracMass = 8.43 * perCent);
        MaterialStore.push_back(D8Mat);
        // BB02,
        MaterialStore.push_back(FR4Mat);
        // FH29BW,
        MaterialStore.push_back(FR4Mat);
        // STC3216,
        MaterialStore.push_back(STC3528Mat);
        // _0603
        MaterialStore.push_back(SR0603Mat);
        // BGA,
        MaterialStore.push_back(FR4Mat);
        // MCH,
        MaterialStore.push_back(FR4Mat);
        // SC0603
        G4Material *SC0603Mat = new G4Material("SC0603", density = 3.9 * g / cm3, nComponents = 4);
        SC0603Mat->AddElement(EleCu, fracMass = 5.40 * perCent);
        SC0603Mat->AddElement(EleNi, fracMass = 13.49 * perCent);
        SC0603Mat->AddElement(EleSn, fracMass = 4.38 * perCent);
        SC0603Mat->AddMaterial(BaTiO3Mat, fracMass = 76.73 * perCent);
        MaterialStore.push_back(SC0603Mat);
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
        G4double HalfCubicWorld = 60 * m;
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

        // HCAL
        G4double HcalUnitSizeX = HcalUnitParameter.House_X; // HcalUnitParameter.SensitiveX + 2. * HcalUnitParameter.PassiveSideThick;
        G4double HcalUnitSizeY = HcalUnitParameter.House_X; // HcalUnitParameter.SensitiveY + 2. * HcalUnitParameter.PassiveSideThick;
        G4double HcalUnitSizeZ = HcalUnitParameter.House_Z; // HcalUnitParameter.SensitiveLength + 2. * HcalUnitParameter.PassiveCoverThick + ((G4int)(HcalUnitParameter.ifDoubleSidedReadout) + 1) * HcalUnitParameter.AttachThick;
        G4Box *HcalAbsSolid = nullptr;
        G4LogicalVolume *HcalAbsLogical = nullptr;
        G4LogicalVolume *HcalPCBLogical = nullptr;
        G4Box *HcalPCB_Cu_Solid = nullptr;
        G4LogicalVolume *HcalPCB_Cu_Logical = nullptr;
        G4double HcalXYsize = std::max(HcalCellNumberX * HcalUnitSizeX, HcalCellNumberY * HcalUnitSizeY);

        if (HcalAbsorberThick > 0)
        {
            HcalAbsSolid = new G4Box("HcalAbsSolid", HcalXYsize / 2., HcalXYsize / 2., HcalAbsorberThick / 2.);
            HcalAbsLogical = new G4LogicalVolume(HcalAbsSolid, GetCaloMaterial(HcalAbsorberMatIndex), "HcalAbsLogical");
        }
        if (HcalPCBThick > 0)
        {
            // HcalPCBSolid = new G4Box("HcalPCBSolid", HcalXYsize / 2., HcalXYsize / 2., HcalPCBThick / 2.);
            // auto HcalPCBSolid = CADMesh::TessellatedMesh::FromSTL("/home/diaohb/CEPC/AHCAL-simulation_stl/HBU_V4_meshed_25.stl");
            // HcalPCBLogical = new G4LogicalVolume(HcalPCBSolid->GetSolid(), GetCaloMaterial(HcalPCBMatIndex), "HcalPCBLogical");
            HcalPCBLogical = new PCBVolume("HcalPCBLogical", &pcbParameter, this);
        }
        if (HcalPCB_Cu_Thick > 0)
        {
            HcalPCB_Cu_Solid = new G4Box("HcalPCB_Cu_Solid", HcalXYsize / 2., HcalXYsize / 2., HcalPCB_Cu_Thick / 2.);
            HcalPCB_Cu_Logical = new G4LogicalVolume(HcalPCB_Cu_Solid, GetCaloMaterial(HcalPCB_Cu_MatIndex), "HcalPCB_Cu_Logical");
        }

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
        Zpos -= HcalUnitSizeZ / 2.; // Additional cover

        Zpos += HcaltriggerThick;
        if (HcaltriggerThick > 0)
        {
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
        // Zpos += 2. * mm;
        Zpos += (HCALCoverThick + HcalUnitSizeZ) / 2.;
        for (G4int z = 0; z < HcalLayerNumber; z++)
        {
            for (G4int y = 0; y < HcalCellNumberY; y++)
            {
                HcalCopyNum = (z + 1) * HcalCellMaxCount * HcalCellMaxCount + (y + 1) * HcalCellMaxCount;
                for (G4int x = 0; x < HcalCellNumberX; x++)
                {
                    HcalCopyNum++;
                    new CaloUnitVolume("HcalUnit", HcalUnitInv, G4ThreeVector(HcalUnitSizeX * (HcalCellNumberX / 2. - 0.5 - x), HcalUnitSizeY * (HcalCellNumberY / 2. - 0.5 - y), Zpos), ifcheckOverlaps, World_Logical, HcalUnitLogical, HcalSensitiveLogical, &HcalUnitParameter, false, HcalCopyNum, this);
                }
            }
            Zpos += (HcalUnitSizeZ + HcalPCBThick) / 2.;
            if (HcalPCBThick > 0)
            {
                new G4PVPlacement(0, G4ThreeVector(-241.8, 0, Zpos), HcalPCBLogical, "HcalPCBPhysical", World_Logical, false, HcalCopyNum / HcalCellMaxCount / HcalCellMaxCount * HcalCellMaxCount * HcalCellMaxCount, ifcheckOverlaps);
                new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), HcalPCBLogical, "HcalPCBPhysical", World_Logical, false, HcalCopyNum / HcalCellMaxCount / HcalCellMaxCount * HcalCellMaxCount * HcalCellMaxCount, ifcheckOverlaps);
                new G4PVPlacement(0, G4ThreeVector(241.8, 0, Zpos), HcalPCBLogical, "HcalPCBPhysical", World_Logical, false, HcalCopyNum / HcalCellMaxCount / HcalCellMaxCount * HcalCellMaxCount * HcalCellMaxCount, ifcheckOverlaps);
            }

            Zpos += (HcalPCBThick + HcalPCB_Cu_Thick) / 2.;
            if (HcalPCB_Cu_Thick > 0)
            {
                new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), HcalPCB_Cu_Logical, "HcalPCB_Cu_Physical", World_Logical, false, HcalCopyNum / HcalCellMaxCount / HcalCellMaxCount * HcalCellMaxCount * HcalCellMaxCount, ifcheckOverlaps);
            }

            Zpos += (HcalPCB_Cu_Thick + HcalAbsorberThick) / 2. + HcalPCB_Abs_gap * mm; // HcalPCBGap 4 mm
            if (HcalAbsorberThick > 0)
            {
                new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), HcalAbsLogical, "HcalAbsPhysical", World_Logical, false, HcalCopyNum / HcalCellMaxCount / HcalCellMaxCount * HcalCellMaxCount * HcalCellMaxCount, ifcheckOverlaps);
            }
            Zpos += (HcalAbsorberThick + HcalUnitSizeZ) / 2.;
        }
        Zpos -= HcalUnitSizeZ / 2.;
        // Zpos += 2. * mm;
        Zpos += HCALCoverThick / 2.;
        new G4PVPlacement(0, G4ThreeVector(0, 0, Zpos), HCALCoverLogical, "HCALCoverPhysicalBack", World_Logical, false, 0, ifcheckOverlaps);

        // Visualization attributes
        G4VisAttributes *AbsVisAtt = new G4VisAttributes(G4Colour(0.28, 0.82, 0.8, 0.3)); // Light Blue
        AbsVisAtt->SetVisibility(true);
        if (HcalModuleType > 0 && HcalAbsorberThick > 0)
            HcalAbsLogical->SetVisAttributes(AbsVisAtt);
        HcalAbsLogical->SetVisAttributes(AbsVisAtt);
        AbsVisAtt = new G4VisAttributes(G4Colour(0.9, 0, 0));
        if (HcalPCB_Cu_Thick > 0)
            HcalPCB_Cu_Logical->SetVisAttributes(AbsVisAtt);
        AbsVisAtt = new G4VisAttributes(G4Colour(0, 0.9, 0, 1));
        HcalPCBLogical->SetVisAttributes(AbsVisAtt);
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
