#include "Global.h"
#include "TFile.h"
#include "TTree.h"
#include <iostream>
using namespace std;
void BeamDataStructure(TString InputFile, TString OutputFile, Int_t ECALOption)
{
    // if (gSystem->AccessPathName(InputDir + "/" + InputFileName))
    // {
    //     std::cout << "Error! ROOT file: " << InputDir + "/" + InputFileName << " does not exist." << std::endl;
    //     return;
    // }
    //------------------------------------------------------
    Int_t EventNum;
	double total_Energy=0;
    std::vector<Int_t> DetectorID;
    std::vector<Int_t> CellID;
    std::vector<Double_t> Hit_Energy;
    std::vector<Double_t> Hit_Time;
    std::vector<Double_t> Hit_X;
    std::vector<Double_t> Hit_Y;
    std::vector<Double_t> Hit_Z;
    std::vector<Double_t> SiPM_Energy;
    std::vector<Int_t> SiPM_Hit;
    // TString OutputFileName = "MC_" + InputFileName;
    // if (!gSystem->AccessPathName(OutputDir + "/" + OutputFileName))
    // {
    // std::cout << "The MC file already exists. Skipped..." << std::endl;
    // return;
    // }
    TFile *ConvertFile = TFile::Open(OutputFile, "RECREATE");
    TTree *EventTree = new TTree("EventTree", "Info stored at event level");
    EventTree->Branch("EventNum", &EventNum, "EventNum/I");
    EventTree->Branch("DetectorID", &DetectorID);
    EventTree->Branch("CellID", &CellID);
    EventTree->Branch("Energy_HCAL", &total_Energy);
    EventTree->Branch("Hit_Energy", &Hit_Energy);
    EventTree->Branch("Hit_Time", &Hit_Time);
    EventTree->Branch("Hit_X", &Hit_X);
    EventTree->Branch("Hit_Y", &Hit_Y);
    EventTree->Branch("Hit_Z", &Hit_Z);
    EventTree->Branch("SiPM_Energy", &SiPM_Energy);
    EventTree->Branch("SiPM_Hit",&SiPM_Hit);
    //------------------------------------------------------
    Int_t EvtID;
    Double_t ParticleEnergy;
    Double_t CaloEdepSum;
    Double_t CaloVisibleEdepSum;
    Double_t EcalEdepSum;
    Double_t EcalVisibleEdepSum;
    Double_t HcalEdepSum;
    Double_t HcalVisibleEdepSum;
    Double_t EcalMaxEdepCell;
    Double_t HcalMaxEdepCell;
    std::vector<Int_t>* vecEcalCellID = nullptr;
    std::vector<Int_t>* vecEcalStepsCell = nullptr;
    std::vector<Double_t>* vecEcalEdepCell = nullptr;
    std::vector<Double_t>* vecEcalVisibleEdepCell = nullptr;
    std::vector<Double_t>* vecEcalHitTimeCell = nullptr;
    std::vector<Double_t>* vecEcalToaCell = nullptr;
    std::vector<Int_t>* vecHcalCellID = nullptr;
    std::vector<Int_t>* vecHcalStepsCell = nullptr;
    std::vector<Double_t>* vecHcalEdepCell = nullptr;
    std::vector<Double_t>* vecHcalVisibleEdepCell = nullptr;
    std::vector<Double_t>* vecHcalHitTimeCell = nullptr;
    std::vector<Double_t>* vecHcalToaCell = nullptr;
    std::vector<Int_t>* vecSiPMHit = nullptr;
    std::vector<Double_t>* vecSiPMEdep = nullptr;
    TFile *DataFile = TFile::Open(InputFile);
    TTree *treeEvt = nullptr;
    DataFile->GetObject("treeEvt",treeEvt);
    treeEvt->SetBranchAddress("EvtID", &EvtID);
    treeEvt->SetBranchAddress("ParticleEnergy", &ParticleEnergy);
    treeEvt->SetBranchAddress("CaloVisibleEdepSum", &CaloVisibleEdepSum);
    treeEvt->SetBranchAddress("EcalEdepSum", &EcalEdepSum);
    treeEvt->SetBranchAddress("EcalVisibleEdepSum", &EcalVisibleEdepSum);
    treeEvt->SetBranchAddress("HcalEdepSum", &HcalEdepSum);
    treeEvt->SetBranchAddress("HcalVisibleEdepSum", &HcalVisibleEdepSum);
    treeEvt->SetBranchAddress("EcalMaxEdepCell", &HcalMaxEdepCell);
    treeEvt->SetBranchAddress("vecEcalCellID", &vecEcalCellID);
    treeEvt->SetBranchAddress("vecEcalStepsCell", &vecEcalStepsCell);
    treeEvt->SetBranchAddress("vecEcalEdepCell", &vecEcalEdepCell);
    treeEvt->SetBranchAddress("vecEcalVisibleEdepCell", &vecEcalVisibleEdepCell);
    treeEvt->SetBranchAddress("vecEcalHitTimeCell", &vecEcalHitTimeCell);
    treeEvt->SetBranchAddress("vecEcalToaCell", &vecEcalToaCell);
    treeEvt->SetBranchAddress("vecHcalCellID", &vecHcalCellID);
    treeEvt->SetBranchAddress("vecHcalStepsCell", &vecHcalStepsCell);
    treeEvt->SetBranchAddress("vecHcalEdepCell", &vecHcalEdepCell);
    treeEvt->SetBranchAddress("vecHcalVisibleEdepCell", &vecHcalVisibleEdepCell);
    treeEvt->SetBranchAddress("vecHcalHitTimeCell", &vecHcalHitTimeCell);
    treeEvt->SetBranchAddress("vecHcalToaCell", &vecHcalToaCell);
    treeEvt->SetBranchAddress("vecSiPMHit", &vecSiPMHit);
    treeEvt->SetBranchAddress("vecSiPMEdep", &vecSiPMEdep);
    //------------------------------------------------------
    Int_t NEvent = treeEvt->GetEntries();
    for (Int_t i = 0; i < NEvent; i++)
    {
        DetectorID.clear();
        CellID.clear();
        Hit_Energy.clear();
        Hit_Time.clear();
        Hit_X.clear();
        Hit_Y.clear();
        Hit_Z.clear();
        SiPM_Energy.clear();
        SiPM_Hit.clear();
        total_Energy = 0;
        if(i%10000==0)cout<<i<<"  /"<<NEvent<<endl;
        treeEvt->GetEntry(i);
        EventNum = i;
        if (ECALOption == 1)
            for (auto it = vecEcalCellID->begin(); it != vecEcalCellID->end(); it++)
            {
                Int_t ID_X = *it % 100;
                Int_t ID_Y = *it % 10000 / 100;
                Int_t ID_Z = *it / 10000;
                if (ID_Z % 2 == 0)
                    std::swap(ID_X, ID_Y);
                Int_t ChipID = (ID_Y - 1) / 7;
                Int_t MemoID = 0;
                Int_t ChannelID = (ID_Y - 1) % 7 * 5 + (ID_X - 1);
                DetectorID.push_back(0);
                CellID.push_back((ID_Z - 1) * 1e5 + ChipID * 1e4 + MemoID * 1e2 + ChannelID);
                Hit_Energy.push_back(vecEcalVisibleEdepCell->at(std::distance(vecEcalCellID->begin(), it)));
                Hit_Time.push_back(vecEcalToaCell->at(std::distance(vecEcalCellID->begin(), it)));
                if (ID_Z % 2 == 0)
                {
                    Hit_X.push_back(5.3 * (20.5 - ID_Y + 1));
                    Hit_Y.push_back(45.3 * (2 - ID_X + 1));
                    Hit_Z.push_back((ID_Z / 2 - 1) * 19.9 + 11.2); // + 1.25
                }
                else
                {
                    Hit_X.push_back(45.3 * (2 - ID_X + 1));
                    Hit_Y.push_back(5.3 * (20.5 - ID_Y + 1));
                    Hit_Z.push_back((ID_Z - 1) / 2 * 19.9);
                }
            }
        for (auto it = vecHcalCellID->begin(); it != vecHcalCellID->end(); it++)
        {
            Int_t ID_X = *it % 100;
            Int_t ID_Y = *it % 10000 / 100;
            Int_t ID_Z = *it / 10000;
            Int_t ChipID = (ID_X - 1) / 6 + (ID_Y - 1) / 6 * 3;
            Int_t MemoID = 0;
            Int_t ChannelID = (ID_X - 1) % 6 + (ID_Y - 1) % 6 * 6;
            DetectorID.push_back(1);
			// double tmp_energy=vecHcalVisibleEdepCell->at(std::distance(vecHcalCellID->begin(), it));
			double tmp_energy=vecHcalEdepCell->at(std::distance(vecHcalCellID->begin(), it));
            // Hit_Energy.push_back(vecHcalEdepCell->at(std::distance(vecHcalCellID->begin(), it)));
            total_Energy+=tmp_energy;
			Hit_Energy.push_back(tmp_energy);
            double tmp_sipm_energy=vecSiPMEdep->at(std::distance(vecHcalCellID->begin(), it));
            SiPM_Energy.push_back(tmp_sipm_energy);
            SiPM_Hit.push_back(vecSiPMHit->at(std::distance(vecHcalCellID->begin(), it)));
            Hit_Time.push_back(vecHcalToaCell->at(std::distance(vecHcalCellID->begin(), it)));
            Hit_X.push_back(40.3 * (9 - 0.5 - ID_X + 1));
            Hit_Y.push_back(40.3 * (9 - 0.5 - ID_Y + 1));
			inverse(40.3 * (9 - 0.5 - ID_X + 1),40.3 * (9 - 0.5 - ID_Y + 1),ChipID,ChannelID);
            CellID.push_back((ID_Z - 1) * 1e5 + ChipID * 1e4 + MemoID * 1e2 + ChannelID);
            if (ECALOption == 1 )
                Hit_Z.push_back((ID_Z - 1) * 30. + 230 + 19.9 * 16 + 1.75 + 2. - 1.25); // 1.75 for 1/2 HCAL PS, 2 for HCAL cover, 1.25 for 1/2 ECAL PS
            else
                Hit_Z.push_back((ID_Z - 1) * 30.);
        }
        EventTree->Fill();
    }
    cout << "fill done" << endl;
    delete treeEvt;
    DataFile->Close();
    ConvertFile->cd();
    EventTree->Write();
    delete EventTree;
    ConvertFile->Close();
    cout << "transform done" << endl;
}
