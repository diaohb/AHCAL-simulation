#include "EcalUnitHit.hh"

#include "G4Colour.hh"
#include "G4ios.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VVisManager.hh"
#include "G4SystemOfUnits.hh"

namespace SimCalModule
{
    G4ThreadLocal G4Allocator<EcalUnitHit> *EcalUnitHitAllocator = nullptr;

    EcalUnitHit::EcalUnitHit()
        : G4VHit(), fCellID(0), fStepsCell(0), fEdepCell(0), fVisibleEdepCell(0), fEdepTemp(0),
          fTimeCell(0), fToaCell(0)
    {
    }

    EcalUnitHit::EcalUnitHit(G4int cellid)
        : G4VHit(), fCellID(cellid), fStepsCell(0), fEdepCell(0), fVisibleEdepCell(0), fEdepTemp(0), fTimeCell(0), fToaCell(0)
    {
    }

    EcalUnitHit::EcalUnitHit(const EcalUnitHit &right)
        : G4VHit(), fCellID(right.fCellID), fStepsCell(right.fStepsCell), fEdepCell(right.fEdepCell), fVisibleEdepCell(right.fVisibleEdepCell),
          fEdepTemp(right.fEdepTemp), fTimeCell(right.fTimeCell), fToaCell(right.fToaCell)
    {
    }

    EcalUnitHit::~EcalUnitHit()
    {
    }

    const EcalUnitHit &EcalUnitHit::operator=(const EcalUnitHit &right)
    {
        fCellID = right.fCellID;
        fStepsCell = right.fStepsCell;
        fEdepCell = right.fEdepCell;
        fVisibleEdepCell = right.fVisibleEdepCell;
        fEdepTemp = right.fEdepTemp;
        fTimeCell = right.fTimeCell;
        fToaCell = right.fToaCell;
        return *this;
    }

    G4bool EcalUnitHit::operator==(const EcalUnitHit &right) const
    {
        return (fCellID == right.fCellID);
    }

    void EcalUnitHit::Draw()
    {
    }

    void EcalUnitHit::Print()
    {
    }
}