#include "HcalUnitHit.hh"

#include "G4Colour.hh"
#include "G4ios.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VVisManager.hh"
#include "G4SystemOfUnits.hh"

namespace SimCalModule
{
    G4ThreadLocal G4Allocator<HcalUnitHit> *HcalUnitHitAllocator = nullptr;

    HcalUnitHit::HcalUnitHit()
        : G4VHit(), fCellID(0), fStepsCell(0), fEdepCell(0), fVisibleEdepCell(0), fEdepTemp(0),
          fTimeCell(0), fToaCell(0), fSiPMHit(0), fSiPMEdep(0)
    {
    }

    HcalUnitHit::HcalUnitHit(G4int cellid)
        : G4VHit(), fCellID(cellid), fStepsCell(0), fEdepCell(0), fVisibleEdepCell(0), fEdepTemp(0), fTimeCell(0), fToaCell(0), fSiPMHit(0), fSiPMEdep(0)
    {
    }

    HcalUnitHit::HcalUnitHit(const HcalUnitHit &right)
        : G4VHit(), fCellID(right.fCellID), fStepsCell(right.fStepsCell), fEdepCell(right.fEdepCell), fVisibleEdepCell(right.fVisibleEdepCell),
          fEdepTemp(right.fEdepTemp), fTimeCell(right.fTimeCell), fToaCell(right.fToaCell), fSiPMHit(right.fSiPMHit), fSiPMEdep(right.fSiPMEdep)
    {
    }

    HcalUnitHit::~HcalUnitHit()
    {
    }

    const HcalUnitHit &HcalUnitHit::operator=(const HcalUnitHit &right)
    {
        fCellID = right.fCellID;
        fStepsCell = right.fStepsCell;
        fEdepCell = right.fEdepCell;
        fVisibleEdepCell = right.fVisibleEdepCell;
        fEdepTemp = right.fEdepTemp;
        fTimeCell = right.fTimeCell;
        fToaCell = right.fToaCell;
        fSiPMHit = right.fSiPMHit;
        fSiPMEdep = right.fSiPMEdep;
        return *this;
    }

    G4bool HcalUnitHit::operator==(const HcalUnitHit &right) const
    {
        return (fCellID == right.fCellID);
    }

    void HcalUnitHit::Draw()
    {
    }

    void HcalUnitHit::Print()
    {
    }
}