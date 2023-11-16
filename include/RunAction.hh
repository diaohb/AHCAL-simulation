#ifndef RunAction_h
#define RunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "TFile.h"
#include "TTree.h"

namespace SimCalModule
{
    enum SimuData
    {
        EvtID_Data,
        ParticleEnergy_Data,
        CaloEdepSum_Data,
        CaloVisibleEdepSum_Data,
        EcalEdepSum_Data,
        EcalVisibleEdepSum_Data,
        HcalEdepSum_Data,
        HcalVisibleEdepSum_Data,
        EcalMaxEdepCell_Data,
        HcalMaxEdepCell_Data,
        vecEcalCellID_Data,
        vecEcalStepsCell_Data,
        vecEcalEdepCell_Data,
        vecEcalVisibleEdepCell_Data,
        vecEcalHitTimeCell_Data,
        vecEcalToaCell_Data,
        vecHcalCellID_Data,
        vecHcalStepsCell_Data,
        vecHcalEdepCell_Data,
        vecHcalVisibleEdepCell_Data,
        vecHcalHitTimeCell_Data,
        vecHcalToaCell_Data
    };

    class EventAction;
    class PrimaryGeneratorAction;

    class RunAction : public G4UserRunAction
    {

    public:
        RunAction(PrimaryGeneratorAction *);
        ~RunAction() override;
        void BeginOfRunAction(const G4Run *) override;
        void EndOfRunAction(const G4Run *) override;
        void FillEvent();
        void TransferData(G4int, SimuData);
        void TransferData(G4double, SimuData);
        void TransferData(std::vector<G4int>, SimuData);
        void TransferData(std::vector<G4double>, SimuData);

    private:
        PrimaryGeneratorAction *fPrimaryGen;
        TFile *fileRun;
        TTree *treeEvt;
        G4int EvtID;
        G4double ParticleEnergy;
        G4double CaloEdepSum;
        G4double CaloVisibleEdepSum;
        G4double EcalEdepSum;
        G4double EcalVisibleEdepSum;
        G4double HcalEdepSum;
        G4double HcalVisibleEdepSum;
        G4double EcalMaxEdepCell;
        G4double HcalMaxEdepCell;
        std::vector<G4int> vecEcalCellID;
        std::vector<G4int> vecEcalStepsCell;
        std::vector<G4double> vecEcalEdepCell;
        std::vector<G4double> vecEcalVisibleEdepCell;
        std::vector<G4double> vecEcalHitTimeCell;
        std::vector<G4double> vecEcalToaCell;
        std::vector<G4int> vecHcalCellID;
        std::vector<G4int> vecHcalStepsCell;
        std::vector<G4double> vecHcalEdepCell;
        std::vector<G4double> vecHcalVisibleEdepCell;
        std::vector<G4double> vecHcalHitTimeCell;
        std::vector<G4double> vecHcalToaCell;
    };
}

#endif
