#ifndef HcalUnitHit_h
#define HcalUnitHit_h 1

#include "globals.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

namespace SimCalModule
{
    class HcalUnitHit : public G4VHit
    {
    public:
        HcalUnitHit();
        HcalUnitHit(G4int);
        HcalUnitHit(const HcalUnitHit &right);
        ~HcalUnitHit() override;

        const HcalUnitHit &operator=(const HcalUnitHit &right);
        G4bool operator==(const HcalUnitHit &right) const;

        inline void *operator new(size_t);
        inline void operator delete(void *aHit);

        void Draw() override;
        void Print() override;

        inline void SetCellID(G4int fID) { fCellID = fID; }
        inline G4int GetCellID() const { return fCellID; }

        inline void SetStepsCell(G4int steps) { fStepsCell = steps; };
        inline void AddStepsCell(G4int steps) { fStepsCell += steps; };
        inline G4int GetStepsCell() const { return fStepsCell; };

        inline void SetEdepCell(G4double edep) { fEdepCell = edep; };
        inline void AddEdepCell(G4double edep) { fEdepCell += edep; };
        inline G4double GetEdepCell() const { return fEdepCell; };

        inline void SetVisibleEdepCell(G4double edep) { fVisibleEdepCell = edep; };
        inline void AddVisibleEdepCell(G4double edep) { fVisibleEdepCell += edep; };
        inline G4double GetVisibleEdepCell() const { return fVisibleEdepCell; };

        inline void SetEdepTemp(G4double edep) { fEdepTemp = edep; }
        inline G4double GetEdepTemp() const { return fEdepTemp; }

        // Time stamp corresonding to the maximum edep per hit
        inline void SetTimeCell(G4double time) { fTimeCell = time; };
        inline G4double GetTimeCell() const { return fTimeCell; };

        // Time stamp corresonding to the fastest (the first) hit
        inline void SetToaCell(G4double toa) { fToaCell = toa; };
        inline G4double GetToaCell() const { return fToaCell; };


    private:
        G4int fCellID;
        G4int fStepsCell;
        G4double fEdepCell;
        G4double fVisibleEdepCell;
        G4double fEdepTemp;
        G4double fTimeCell;
        G4double fToaCell;
    };

    using HcalUnitHitsCollection = G4THitsCollection<HcalUnitHit>;
    extern G4ThreadLocal G4Allocator<HcalUnitHit> *HcalUnitHitAllocator;

    inline void *HcalUnitHit::operator new(size_t)
    {
        if (!HcalUnitHitAllocator)
            HcalUnitHitAllocator = new G4Allocator<HcalUnitHit>;
        return (void *)HcalUnitHitAllocator->MallocSingle();
    }

    inline void HcalUnitHit::operator delete(void *aHit)
    {
        HcalUnitHitAllocator->FreeSingle((HcalUnitHit *)aHit);
    }
}

#endif
