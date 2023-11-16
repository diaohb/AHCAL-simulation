#ifndef EcalUnitSD_h
#define EcalUnitSD_h 1

#include "globals.hh"
#include "G4VSensitiveDetector.hh"
#include "EcalUnitHit.hh"
#include <map>

class G4HCofThisEvent;
class G4Step;

namespace SimCalModule
{
    class DetectorConstruction;

    class EcalUnitSD : public G4VSensitiveDetector
    {
    public:
        EcalUnitSD(G4String name);
        ~EcalUnitSD() override;

        void Initialize(G4HCofThisEvent *) override;
        G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *) override;
        G4double BirksAttenuation(const G4Step *aStep);

    private:
        EcalUnitHitsCollection *fEcalUnitHitsCollection;
        G4int fHitCID;
        G4int fHitIndex;
        std::map<G4int, G4int> CellIDDecoder;
        const DetectorConstruction *detector;
    };
}

#endif
