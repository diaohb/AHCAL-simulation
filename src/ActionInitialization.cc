#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "RunAction.hh"

namespace SimCalModule
{
    ActionInitialization::ActionInitialization()
        : G4VUserActionInitialization()
    {
    }

    ActionInitialization::~ActionInitialization()
    {
    }

    void ActionInitialization::BuildForMaster() const
    {
        PrimaryGeneratorAction *primary = new PrimaryGeneratorAction;
        SetUserAction(new RunAction(primary));
    }

    void ActionInitialization::Build() const
    {
        PrimaryGeneratorAction *primary = new PrimaryGeneratorAction;
        SetUserAction(primary);
        RunAction *run = new RunAction(primary);
        SetUserAction(run);
        SetUserAction(new EventAction(run));
    }
}