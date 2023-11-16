#ifndef HcalUnitSD_h
#define HcalUnitSD_h 1

#include "globals.hh"
#include "G4VSensitiveDetector.hh"
#include "HcalUnitHit.hh"
#include <map>

class G4HCofThisEvent;
class G4Step;

namespace SimCalModule
{
    class DetectorConstruction;

    class HcalUnitSD : public G4VSensitiveDetector
    {
    public:
        HcalUnitSD(G4String name);
        ~HcalUnitSD() override;

        void Initialize(G4HCofThisEvent *) override;
        G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *) override;
        G4double BirksAttenuation(const G4Step *aStep);

    private:
        HcalUnitHitsCollection *fHcalUnitHitsCollection;
        G4int fHitCID;
        G4int fHitIndex;
        std::map<G4int, G4int> CellIDDecoder;
        const DetectorConstruction *detector;
    };
}

#endif
