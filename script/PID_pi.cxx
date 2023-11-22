void PID_pi()
{
    const Double_t Energy[11] = {10,20,30,40,50,60,70,80,90,100,120};
    const Int_t NEnergy = sizeof(Energy)/sizeof(Energy[0]);
    const char* Run[11] = {"158_20221025_021822","64_20221022_013927","89_20221022_204324","58_20221021_184832","83_20221022_182955","49_20221021_042312","76_20221022_123739","41_20221020_215218","101_20221023_110341","33_20221020_163057","68_20221022_104332"};
    const Double_t k1[11] = {0.203,2.203,0.163,0.163,0.183,0.183,0.183,0.183,0.183,0.183,0.183};
    const Double_t b1[11] = {0.543,0.643,0.643,0.643,0.623,0.623,0.723,0.723,0.723,0.95 ,1.05};
    const Double_t k2[11] = {0.243,0.263,0.258,0.238,0.18 ,0.205,0.205,0.205,0.19 ,0.19 ,0.19};
    const Double_t b2[11] = {1.1  ,1.415,1.6  ,1.68 ,1.551,1.735,1.825,1.875,1.865,1.885,1.97};
    const Double_t MIPEnergy = 0.461; // Visible
    const Double_t MIPCut = 0.3;
    const char* Particle[1] = {"pi+"};

    for (Int_t NoEnergy = 0; NoEnergy < NEnergy; NoEnergy++)
    {
        TString InputRootFile = Form("/cefs/higgs/xiaxin/cepc/Beamtest/analysis/ParticleID/RootAna/PID/result/Experiment/No_Energy_Cut/%s_V1/%.0fGeV/AHCAL_Run%s.root",Particle[0],Energy[NoEnergy],Run[NoEnergy]);
        TString OutputRootFile = Form("AHCAL_Run158_PID.root");

        //-------------------------------------------------------------------------------
        TFile* outputrootfile = TFile::Open(OutputRootFile, "RECREATE");
        TTree* TB_PID_Hit = new TTree("TB_PID_Hit","TB_PID_Hit");
        std::vector<Double_t> Hit_X;
        std::vector<Double_t> Hit_Y;
        std::vector<Double_t> Hit_Z;
        std::vector<Double_t> Hit_E;
        std::vector<Int_t> cherenkov;
        Double_t StartLayer;
        TB_PID_Hit->Branch("Hit_X", &Hit_X);
        TB_PID_Hit->Branch("Hit_Y", &Hit_Y);   
        TB_PID_Hit->Branch("Hit_Z", &Hit_Z);
        TB_PID_Hit->Branch("Hit_E", &Hit_E);
        TB_PID_Hit->Branch("cherenkov", &cherenkov);
        TB_PID_Hit->Branch("StartLayer", &StartLayer);

        //-------------------------------------------------------------------------------
        TFile *inputrootfile = TFile::Open(InputRootFile);
        TTree *Calib_Hit = nullptr;
        inputrootfile->GetObject("Calib_Hit",Calib_Hit);
        std::vector<Double_t> *TmpX = nullptr;
        std::vector<Double_t> *TmpY = nullptr;
        std::vector<Double_t> *TmpZ = nullptr;
        std::vector<Double_t> *Hit_En = nullptr;
        std::vector<Int_t> *Cherenkov = nullptr;
        Double_t Hit_Mean_En;
        Double_t FD_2D;
        Calib_Hit->SetBranchAddress("TmpX", &TmpX);
        Calib_Hit->SetBranchAddress("TmpY", &TmpY);
        Calib_Hit->SetBranchAddress("TmpZ", &TmpZ);
        Calib_Hit->SetBranchAddress("Hit_En", &Hit_En);
        Calib_Hit->SetBranchAddress("Cherenkov", &Cherenkov);
        Calib_Hit->SetBranchAddress("Hit_Mean_En", &Hit_Mean_En);
        Calib_Hit->SetBranchAddress("FD_2D", &FD_2D);
        Int_t nEvent = Calib_Hit->GetEntries();
        for (Int_t iEvt=10; iEvt<nEvent; iEvt++)
        {
            Calib_Hit->GetEntry(iEvt);
            if (FD_2D > 0 && Hit_Mean_En > 0.4 && FD_2D > -k1[NoEnergy]*Hit_Mean_En+b1[NoEnergy] && FD_2D < -k2[NoEnergy]*Hit_Mean_En+b2[NoEnergy])
            {
                Double_t E[40] = {0};
                Double_t N[40] = {0};
                Double_t A[40] = {0};
                cout<<TmpX->size()<<" "<<TmpY->size()<<" "<<TmpZ->size()<<" "<<Hit_En->size()<<" "<<Cherenkov->size()<<endl;
                for (Int_t i = 0; i < TmpX->size(); i++)
                {
                    Hit_X.push_back(TmpX->at(i));
                    Hit_Y.push_back(TmpY->at(i));
                    Hit_Z.push_back(TmpZ->at(i));
                    Hit_E.push_back(Hit_En->at(i));
                    cherenkov.push_back(Cherenkov->at(i));
                    for (Int_t L = 0; L < 40; L++)
                    {
                        if((TmpZ->at(i)-1.75)/26 == L)
                        {
                            E[L] += Hit_En->at(i);
                            N[L] += 1;
                        }
                    }
                }
                for (Int_t L = 0; L < 40; L++)
                {
                    Double_t a = 0;
                    for(Int_t j=L; j >= 0; j--) {a += E[j];}
                    A[L] = a/(L+1);
                }
                for (Int_t k=0; k < 40-1; k++)
                {
                    if (A[k] + A[k+1] > 3*MIPEnergy && N[k] + N[k+1] > 4 && E[k+1] > (0.2*Energy[NoEnergy]+8)*MIPEnergy)
                    {
                        StartLayer = k + 1;
                        break;
                    }
                }
                TB_PID_Hit->Fill();
                Hit_X.clear();
                Hit_Y.clear();
                Hit_Z.clear();
                Hit_E.clear();
                cherenkov.clear();
            }
        }
        delete Calib_Hit,TmpX,TmpY,TmpZ,Hit_En,Cherenkov;
        inputrootfile->Close();

        outputrootfile->cd();
        TB_PID_Hit->Write();
        delete TB_PID_Hit;
        outputrootfile->Close();
    }
}
