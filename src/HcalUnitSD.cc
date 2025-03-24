#include "HcalUnitSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ParticleTypes.hh"
#include "G4VProcess.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"

namespace SimCalModule
{
    HcalUnitSD::HcalUnitSD(G4String name)
        : G4VSensitiveDetector(name), fHcalUnitHitsCollection(nullptr), fHitCID(-1), fHitIndex(0), detector(nullptr)
    {
        detector = static_cast<const DetectorConstruction *>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        CellIDDecoder.clear();
        collectionName.insert("HcalUnitHitsCollection");
    }

    HcalUnitSD::~HcalUnitSD()
    {
    }

    void HcalUnitSD::Initialize(G4HCofThisEvent *hitsCE)
    {
        fHcalUnitHitsCollection = new HcalUnitHitsCollection(SensitiveDetectorName, collectionName[0]);
        if (fHitCID < 0)
        {
            fHitCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
        }
        hitsCE->AddHitsCollection(fHitCID, fHcalUnitHitsCollection);
        fHitIndex = 0;
        CellIDDecoder.clear();
    }

    G4bool HcalUnitSD::ProcessHits(G4Step *aStep, G4TouchableHistory *)
    {
        if (aStep->GetTrack()->GetGlobalTime() > detector->GetHcalStepTimeLimit())
            return false;

        G4StepPoint *thePrePoint = aStep->GetPreStepPoint();
        G4StepPoint *thePostPoint = aStep->GetPostStepPoint();
        G4int copyNo = thePrePoint->GetTouchableHandle()->GetCopyNumber(2);
        G4int copyNo0 = thePrePoint->GetTouchableHandle()->GetCopyNumber();
        G4int copyNo0post = thePostPoint->GetTouchableHandle()->GetCopyNumber();
        G4int flag = 0;
        if(copyNo0 == 1){
            flag = 1;
        }
        else if(copyNo0 == 2){
            copyNo = thePrePoint->GetTouchableHandle()->GetCopyNumber(3);
            if(copyNo0post == 1){
                flag = 1;
            }
            else if(copyNo0post == 3){
                flag = 2;
            }
        }
        else if(copyNo0 == 3){
            flag = 3;
            copyNo = thePrePoint->GetTouchableHandle()->GetCopyNumber(4);
        }
        // flag=1 ;
        G4double edep = aStep->GetTotalEnergyDeposit();
        if (edep == 0 && flag != 2)
            return false;

        // G4cout<<"copyNo0: "<<copyNo0<<" copyNo0post: "<<copyNo0post<<" copyNo: "<<copyNo<<"  "<<thePrePoint->GetPosition().getX()<< "  "<<thePostPoint->GetPosition().getX()<<G4endl;
        // G4cout<<edep/aStep->GetStepLength()<<"  "<<edep<<"  "<<aStep->GetStepLength()<<G4endl;
        HcalUnitHit *hit = nullptr;
        if(flag != 0){
            if (CellIDDecoder.find(copyNo) != CellIDDecoder.end())
            {
                hit = (*fHcalUnitHitsCollection)[CellIDDecoder[copyNo]];
                if(flag == 1){
                    hit->AddEdepCell(edep);
                    hit->AddVisibleEdepCell(BirksAttenuation(aStep));
                    hit->AddStepsCell(1);
                    if (hit->GetEdepTemp() < edep)
                    {
                        hit->SetTimeCell((thePrePoint->GetGlobalTime() + thePostPoint->GetGlobalTime()) / 2.);
                        hit->SetEdepTemp(edep);
                    }
                }
                else if(flag == 2){
                    hit->AddSiPMHit(1);
                    hit->AddSiPMEdep(edep);
                }
                else if(flag == 3){
                    hit->AddSiPMEdep(edep);
                }
                hit->SetToaCell(aStep->GetTrack()->GetGlobalTime());
            }
            else
            {
                hit = new HcalUnitHit(copyNo);
                if(flag == 1){
                    hit->SetEdepCell(edep);
                    hit->SetVisibleEdepCell(BirksAttenuation(aStep));
                    hit->SetEdepTemp(edep);
                    hit->SetTimeCell((thePrePoint->GetGlobalTime() + thePostPoint->GetGlobalTime()) / 2.);
                    hit->SetToaCell(aStep->GetTrack()->GetGlobalTime());
                    hit->SetStepsCell(1);
                    hit->SetSiPMHit(0);
                    hit->SetSiPMEdep(0);
                }
                else if(flag == 2){
                    hit->SetEdepCell(0);
                    hit->SetVisibleEdepCell(0);
                    hit->SetEdepTemp(0);
                    hit->SetTimeCell((thePrePoint->GetGlobalTime() + thePostPoint->GetGlobalTime()) / 2.);
                    hit->SetToaCell(aStep->GetTrack()->GetGlobalTime());
                    hit->SetStepsCell(0);
                    hit->SetSiPMHit(1);
                    hit->SetSiPMEdep(edep);
                    // G4cout<<"diaohb  "<<hit->GetSiPMHit()<<G4endl;
                }
                else if(flag == 3){
                    hit->SetEdepCell(0);
                    hit->SetVisibleEdepCell(0);
                    hit->SetEdepTemp(0);
                    hit->SetTimeCell((thePrePoint->GetGlobalTime() + thePostPoint->GetGlobalTime()) / 2.);
                    hit->SetToaCell(aStep->GetTrack()->GetGlobalTime());
                    hit->SetStepsCell(0);
                    hit->SetSiPMHit(0);
                    hit->SetSiPMEdep(edep);
                }

                fHcalUnitHitsCollection->insert(hit);
                CellIDDecoder.insert(std::make_pair(copyNo, fHitIndex));
                fHitIndex++;
            }
        }


        return true;
    }

    G4double HcalUnitSD::BirksAttenuation(const G4Step *aStep)
    {
        // Example of Birk attenuation law in organic scintillators.
        // adapted from Geant3 PHYS337. See MIN 80 (1970) 239-244
        G4Material *material = aStep->GetTrack()->GetMaterial();
        G4double birk1 = material->GetIonisation()->GetBirksConstant();
        G4double destep = aStep->GetTotalEnergyDeposit();
        G4double stepl = aStep->GetStepLength();
        G4double charge = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
        G4double response = destep;
        if (birk1 * destep * stepl * charge != 0.)
        {
            response = destep / (1. + birk1 * destep / stepl);
        }
        return response;
    }
}