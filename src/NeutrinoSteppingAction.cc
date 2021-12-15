#include "NeutrinoSteppingAction.hh"
#include "NeutrinoRunAction.hh"

#include "G4Step.hh"

NeutrinoSteppingAction::NeutrinoSteppingAction()
{

}

NeutrinoSteppingAction::~NeutrinoSteppingAction()
{

}

void NeutrinoSteppingAction::UserSteppingAction(const G4Step* aStep)
{
    G4int stepNo = aStep->GetTrack()->GetCurrentStepNumber();
    if (stepNo >= 100000)
    {
        G4cout << "StepNo is greater than 1e5, stop the step!!!!!" << G4endl;
        aStep->GetTrack()->SetTrackStatus(fStopAndKill);
    }

}
