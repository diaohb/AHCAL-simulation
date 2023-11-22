const Double_t MIPEnergy = 0.461; // Visible
const Double_t MIPCut = 0.5;
// const Double_t TimeCut = 150; // ns
const Double_t ADCHLRatio = 37.5;
const Double_t ADCError = 0.0002;
const Int_t ADCLimit = 4000;
const Int_t ADCSWITCH = 3000;
const Int_t ADCBaseline = 350;
const Double_t MIPResponse[2] = {17., 20.};
const Double_t PEChargeMean[2] = {19.75, 29.4};
const Double_t PEChargeSigma[2] = {3., 6.};
const Double_t PEChargeNoise = 3;
// --------------------------------------------------------------------
auto SiPMResponseFit = new TF1("SiPMResponseFit", "0.876*((1-[1])*[0]*(1-exp(-x/[0]))+[1]*x)*([2]+1)/( [2]+x / ([0]*(1-exp(-x/[0]))) )*(1+[3]*exp(-x/[0]))*(1+[4])");

Int_t ADCDigi(Int_t fADC)
{
    Int_t sAdc = -1;
    while(sAdc < 0)
        sAdc = std::round(gRandom->Gaus(fADC + ADCBaseline, (fADC + ADCBaseline) * ADCError));
    if (sAdc > ADCLimit)
        sAdc = ADCLimit;
    if (sAdc > ADCSWITCH)
    {
        sAdc = -1;
        while(sAdc < 0)
            sAdc = std::round(gRandom->Gaus(fADC/ADCHLRatio + ADCBaseline, (fADC/ADCHLRatio + ADCBaseline) * ADCError));
        if (sAdc > ADCLimit)
            sAdc = ADCLimit;
        return std::round((sAdc - ADCBaseline) * ADCHLRatio);
    }
    else
    {
        return (sAdc - ADCBaseline);
    }
}

Double_t SiPMDigi(Double_t edep, Int_t i, Int_t m)
{
    // The Poisson distribution of scintillation process is not considered
    // sPhoton = gRandom->Poisson(edep * LightYieldperMeV); 10000p.e./MeV
    Int_t sPix = 0;
    sPix = gRandom->Poisson(edep / MIPEnergy * MIPResponse[i]);
    if (sPix > 2500)
        sPix = std::round(SiPMResponseFit->Eval(sPix)); // HPK and NDL SiPMs has similar pixel number, assume equal
    Double_t sChargeOutMean = sPix * PEChargeMean[i];
    Double_t sChargeOutSigma = sqrt(sPix * PEChargeSigma[i] * PEChargeSigma[i] + PEChargeNoise * PEChargeNoise);
    Int_t sChargeOut = -1;
    while(sChargeOut < 0)
        sChargeOut = std::round(gRandom->Gaus(sChargeOutMean, sChargeOutSigma));
    if (m > 0)
        sChargeOut = ADCDigi(sChargeOut);
    Double_t sMIP = sChargeOut / PEChargeMean[i] / MIPResponse[i];
    if (sMIP < MIPCut)
        return 0;
    else
        return sMIP * MIPEnergy;
}

// --------------------------------------------------------------------
void EnergySumDigi(TString InputDir, TString OutputDir, TString InputFileName)
{
    if (gSystem->AccessPathName(InputDir + "/" + InputFileName))
    {
        std::cout << "Error! ROOT file: " << InputDir + "/" + InputFileName << " does not exist." << std::endl;
        return;
    }
    //------------------------------------------------------
    SiPMResponseFit->SetParameters(3082.88, 1.35524, 4.0577, 0.0206382, 0.109543);
    Int_t Digi_EventNum;
    Double_t Digi_Energy_ECAL;
    Double_t Digi_Energy_HCAL;
    std::vector<Int_t> Digi_DetectorID;
    std::vector<Int_t> Digi_CellID;
    std::vector<Double_t> Digi_Hit_Energy;
    std::vector<Double_t> Digi_Hit_Time;
    std::vector<Double_t> Digi_Hit_X;
    std::vector<Double_t> Digi_Hit_Y;
    std::vector<Double_t> Digi_Hit_Z;
    TString OutputFileName = "Digi_" + InputFileName;
    TFile *DigiFile = TFile::Open(OutputDir + "/" + OutputFileName, "RECREATE");
    TTree *DigiEventTree = new TTree("EventTree", "Info stored at event level");
    DigiEventTree->Branch("EventNum", &Digi_EventNum, "EventNum/I");
    DigiEventTree->Branch("DetectorID", &Digi_DetectorID);
    DigiEventTree->Branch("CellID", &Digi_CellID);
    DigiEventTree->Branch("Digi_Hit_Energy", &Digi_Hit_Energy);
    DigiEventTree->Branch("Digi_Energy_ECAL", &Digi_Energy_ECAL, "Digi_Energy_ECAL/D");
    DigiEventTree->Branch("Digi_Energy_HCAL", &Digi_Energy_HCAL, "Digi_Energy_HCAL/D");
    DigiEventTree->Branch("Hit_Time", &Digi_Hit_Time);
    DigiEventTree->Branch("Hit_X", &Digi_Hit_X);
    DigiEventTree->Branch("Hit_Y", &Digi_Hit_Y);
    DigiEventTree->Branch("Hit_Z", &Digi_Hit_Z);
    //------------------------------------------------------
    Int_t EventNum;
    std::vector<Int_t>* DetectorID = nullptr;
    std::vector<Int_t>* CellID = nullptr;
    std::vector<Double_t>* Hit_Energy = nullptr;
    std::vector<Double_t>* Hit_Time = nullptr;
    std::vector<Double_t>* Hit_X = nullptr;
    std::vector<Double_t>* Hit_Y = nullptr;
    std::vector<Double_t>* Hit_Z = nullptr;
    TFile *DataFile = TFile::Open(InputDir + "/" + InputFileName);
    TTree *EventTree = nullptr;
    DataFile->GetObject("EventTree",EventTree);
    EventTree->SetBranchAddress("EventNum", &EventNum);
    EventTree->SetBranchAddress("DetectorID", &DetectorID);
    EventTree->SetBranchAddress("CellID", &CellID);
    EventTree->SetBranchAddress("Hit_Energy", &Hit_Energy);
    EventTree->SetBranchAddress("Hit_Time", &Hit_Time);
    EventTree->SetBranchAddress("Hit_X", &Hit_X);
    EventTree->SetBranchAddress("Hit_Y", &Hit_Y);
    EventTree->SetBranchAddress("Hit_Z", &Hit_Z);
    //------------------------------------------------------
    Int_t NEvent = EventTree->GetEntries();
    for (Int_t i = 0; i < NEvent; i++)
    {
        Double_t fDigiECALEnergy = 0;
        Double_t fDigiHCALEnergy = 0;
        EventTree->GetEntry(i);
        for (size_t count = 0; count < Hit_Energy->size(); count++)
        {
            if (DetectorID->at(count) == 1) // Only HCAL
            {
                Double_t Dis_X = Hit_X->at(count) - 1;
                Double_t Dis_Y = Hit_Y->at(count) - 1;
                Double_t Dis_Z = Hit_Z->at(count) + 100;
                // Double_t Tof = std::sqrt(Dis_X * Dis_X + Dis_Y * Dis_Y + Dis_Z * Dis_Z) / 299.8;
                // if ( Hit_Time->at(count) - Tof < TimeCut )
                // {
                Double_t TempDigiEnergy = SiPMDigi(Hit_Energy->at(count), (CellID->at(count)/100000 - 40) / 38 , 1);
                Digi_DetectorID.push_back(DetectorID->at(count));
                Digi_CellID.push_back(CellID->at(count));
                Digi_Hit_Energy.push_back(TempDigiEnergy);
                Digi_Hit_Time.push_back(Hit_Time->at(count));
                Digi_Hit_X.push_back(Hit_X->at(count));
                Digi_Hit_Y.push_back(Hit_Y->at(count));
                Digi_Hit_Z.push_back(Hit_Z->at(count));
                fDigiHCALEnergy += TempDigiEnergy;
                // }
            }
        }
        Digi_EventNum = EventNum;
        Digi_Energy_ECAL = fDigiECALEnergy;
        Digi_Energy_HCAL = fDigiHCALEnergy;
        DigiEventTree->Fill();
        Digi_DetectorID.clear();
        Digi_CellID.clear();
        Digi_Hit_Energy.clear();
        Digi_Hit_Time.clear();
        Digi_Hit_X.clear();
        Digi_Hit_Y.clear();
        Digi_Hit_Z.clear();
    }
    DigiFile->cd();
    DigiEventTree->Write();
    delete DigiEventTree;
    DigiFile->Close();
    delete EventTree;
    DataFile->Close();
}
