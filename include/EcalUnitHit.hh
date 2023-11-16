#ifndef EcalUnitHit_h
#define EcalUnitHit_h 1

#include "globals.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

namespace SimCalModule
{
    class EcalUnitHit : public G4VHit
    {
    public:
        EcalUnitHit();
        EcalUnitHit(G4int);
        EcalUnitHit(const EcalUnitHit &right);
        ~EcalUnitHit() override;

        const EcalUnitHit &operator=(const EcalUnitHit &right);
        G4bool operator==(const EcalUnitHit &right) const;

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

    using EcalUnitHitsCollection = G4THitsCollection<EcalUnitHit>;
    extern G4ThreadLocal G4Allocator<EcalUnitHit> *EcalUnitHitAllocator;

    inline void *EcalUnitHit::operator new(size_t)
    {
        if (!EcalUnitHitAllocator)
            EcalUnitHitAllocator = new G4Allocator<EcalUnitHit>;
        return (void *)EcalUnitHitAllocator->MallocSingle();
    }

    inline void EcalUnitHit::operator delete(void *aHit)
    {
        EcalUnitHitAllocator->FreeSingle((EcalUnitHit *)aHit);
    }
}

#endif
