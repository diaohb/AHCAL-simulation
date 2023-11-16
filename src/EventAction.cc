#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "EcalUnitSD.hh"
#include "HcalUnitSD.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"

namespace SimCalModule
{
    EventAction::EventAction(RunAction *aRun)
        : G4UserEventAction(), fRunAction(aRun), fEcalCollID(-1), fHcalCollID(-1)
    {
    }

    EventAction::~EventAction()
    {
    }

    void EventAction::BeginOfEventAction(const G4Event *anEvent)
    {
        ResetEventData();

        EvtID = anEvent->GetEventID();
        if (EvtID % 100 == 0)
            G4cout << "Start to simulate the event " << EvtID << G4endl;

        const PrimaryGeneratorAction *primary = static_cast<const PrimaryGeneratorAction *>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
        ParticleEnergy = primary->GetParticleGun()->GetParticleEnergy() / MeV;

        const DetectorConstruction *detector = static_cast<const DetectorConstruction *>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());

        if (fEcalCollID < 0 && detector->GetEcalModuleType() > 0)
        {
            auto sdManager = G4SDManager::GetSDMpointer();
            fEcalCollID = sdManager->GetCollectionID("EcalUnitHitsCollection");
        }

        if (fHcalCollID < 0 && detector->GetHcalModuleType() > 0)
        {
            auto sdManager = G4SDManager::GetSDMpointer();
            fHcalCollID = sdManager->GetCollectionID("HcalUnitHitsCollection");
        }
    }

    void EventAction::EndOfEventAction(const G4Event *anEvent)
    {
        G4HCofThisEvent *hitsCE = anEvent->GetHCofThisEvent();
        EcalUnitHitsCollection *EcalUnitHC = nullptr;
        HcalUnitHitsCollection *HcalUnitHC = nullptr;
        if (hitsCE && fEcalCollID >= 0)
            EcalUnitHC = (EcalUnitHitsCollection *)(hitsCE->GetHC(fEcalCollID));
        if (hitsCE && fHcalCollID >= 0)
            HcalUnitHC = (HcalUnitHitsCollection *)(hitsCE->GetHC(fHcalCollID));

        if (EcalUnitHC)
        {
            size_t n_hit = EcalUnitHC->entries();
            G4double fEdepCell;
            G4double fVisibleEdepCell;
            for (size_t i = 0; i < n_hit; i++)
            {
                fEdepCell = (*EcalUnitHC)[i]->GetEdepCell() / MeV;
                fVisibleEdepCell = (*EcalUnitHC)[i]->GetVisibleEdepCell() / MeV;
                EcalEdepSum += fEdepCell;
                EcalVisibleEdepSum += fVisibleEdepCell;
                vecEcalCellID.push_back((*EcalUnitHC)[i]->GetCellID());
                vecEcalStepsCell.push_back((*EcalUnitHC)[i]->GetStepsCell());
                vecEcalEdepCell.push_back(fEdepCell);
                vecEcalVisibleEdepCell.push_back(fVisibleEdepCell);
                vecEcalHitTimeCell.push_back((*EcalUnitHC)[i]->GetTimeCell() / ns);
                vecEcalToaCell.push_back((*EcalUnitHC)[i]->GetToaCell() / ns);
            }
            if (vecEcalEdepCell.size() > 0)
            {
                auto fMaxEnergy_it = std::max_element(vecEcalEdepCell.begin(), vecEcalEdepCell.end());
                EcalMaxEdepCell = *fMaxEnergy_it;
            }
        }

        if (HcalUnitHC)
        {
            size_t n_hit = HcalUnitHC->entries();
            G4double fEdepCell;
            G4double fVisibleEdepCell;
            for (size_t i = 0; i < n_hit; i++)
            {
                fEdepCell = (*HcalUnitHC)[i]->GetEdepCell() / MeV;
                fVisibleEdepCell = (*HcalUnitHC)[i]->GetVisibleEdepCell() / MeV;
                HcalEdepSum += fEdepCell;
                HcalVisibleEdepSum += fVisibleEdepCell;
                vecHcalCellID.push_back((*HcalUnitHC)[i]->GetCellID());
                vecHcalStepsCell.push_back((*HcalUnitHC)[i]->GetStepsCell());
                vecHcalEdepCell.push_back(fEdepCell);
                vecHcalVisibleEdepCell.push_back(fVisibleEdepCell);
                vecHcalHitTimeCell.push_back((*HcalUnitHC)[i]->GetTimeCell() / ns);
                vecHcalToaCell.push_back((*HcalUnitHC)[i]->GetToaCell() / ns);
            }
            if (vecHcalEdepCell.size() > 0)
            {
                auto fMaxEnergy_it = std::max_element(vecHcalEdepCell.begin(), vecHcalEdepCell.end());
                HcalMaxEdepCell = *fMaxEnergy_it;
            }
        }

        CaloEdepSum = EcalEdepSum + HcalEdepSum;
        CaloVisibleEdepSum = EcalVisibleEdepSum + HcalVisibleEdepSum;

        fRunAction->TransferData(EvtID, EvtID_Data);
        fRunAction->TransferData(ParticleEnergy, ParticleEnergy_Data);
        fRunAction->TransferData(CaloEdepSum, CaloEdepSum_Data);
        fRunAction->TransferData(CaloVisibleEdepSum, CaloVisibleEdepSum_Data);
        fRunAction->TransferData(EcalEdepSum, EcalEdepSum_Data);
        fRunAction->TransferData(EcalVisibleEdepSum, EcalVisibleEdepSum_Data);
        fRunAction->TransferData(HcalEdepSum, HcalEdepSum_Data);
        fRunAction->TransferData(HcalVisibleEdepSum, HcalVisibleEdepSum_Data);
        fRunAction->TransferData(EcalMaxEdepCell, EcalMaxEdepCell_Data);
        fRunAction->TransferData(HcalMaxEdepCell, HcalMaxEdepCell_Data);
        fRunAction->TransferData(vecEcalCellID, vecEcalCellID_Data);
        fRunAction->TransferData(vecEcalStepsCell, vecEcalStepsCell_Data);
        fRunAction->TransferData(vecEcalEdepCell, vecEcalEdepCell_Data);
        fRunAction->TransferData(vecEcalVisibleEdepCell, vecEcalVisibleEdepCell_Data);
        fRunAction->TransferData(vecEcalHitTimeCell, vecEcalHitTimeCell_Data);
        fRunAction->TransferData(vecEcalToaCell, vecEcalToaCell_Data);
        fRunAction->TransferData(vecHcalCellID, vecHcalCellID_Data);
        fRunAction->TransferData(vecHcalStepsCell, vecHcalStepsCell_Data);
        fRunAction->TransferData(vecHcalEdepCell, vecHcalEdepCell_Data);
        fRunAction->TransferData(vecHcalVisibleEdepCell, vecHcalVisibleEdepCell_Data);
        fRunAction->TransferData(vecHcalHitTimeCell, vecHcalHitTimeCell_Data);
        fRunAction->TransferData(vecHcalToaCell, vecHcalToaCell_Data);

        fRunAction->FillEvent();
    }

    void EventAction::ResetEventData()
    {
        EvtID = 0;
        ParticleEnergy = 0;
        CaloEdepSum = 0;
        CaloVisibleEdepSum = 0;
        EcalEdepSum = 0;
        EcalVisibleEdepSum = 0;
        HcalEdepSum = 0;
        HcalVisibleEdepSum = 0;
        EcalMaxEdepCell = 0;
        HcalMaxEdepCell = 0;
        vecEcalCellID.clear();
        vecEcalStepsCell.clear();
        vecEcalEdepCell.clear();
        vecEcalVisibleEdepCell.clear();
        vecEcalHitTimeCell.clear();
        vecEcalToaCell.clear();
        vecHcalCellID.clear();
        vecHcalStepsCell.clear();
        vecHcalEdepCell.clear();
        vecHcalVisibleEdepCell.clear();
        vecHcalHitTimeCell.clear();
        vecHcalToaCell.clear();
    }
}
