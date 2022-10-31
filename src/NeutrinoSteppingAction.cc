#include "../include/NeutrinoSteppingAction.hh"
#include "G4PhysicalConstants.hh"
#include "NeutrinoRunAction.hh"
#include "../include/NormalTrackInfo.hh"
#include "G4SystemOfUnits.hh"

#include "G4Step.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

NeutrinoSteppingAction::NeutrinoSteppingAction()
{
    if (m_save_step)
    {
        m_file = TFile::Open("stepRecording.root", "recreate");
        m_tree = new TTree("stepRecord", "stepRecord");
        m_tree->Branch("evtID", &evtID);
        m_tree->Branch("trackID", &trackID);
        m_tree->Branch("x", &step_x);
        m_tree->Branch("y", &step_y );
        m_tree->Branch("z", &step_z);
        m_tree->Branch("t", &step_t);
        m_tree->Branch("VolumeName", &VolumeName);
    }

}

NeutrinoSteppingAction::~NeutrinoSteppingAction()
{
    if (m_save_step)
    {
        m_file->cd();
        m_tree->Write();
        m_file->Close();
    }

}

void NeutrinoSteppingAction::UserSteppingAction(const G4Step* aStep)
{
    G4int stepNo = aStep->GetTrack()->GetCurrentStepNumber();
    if (stepNo >= 100000)
    {
        G4cout << "StepNo is greater than 1e5, stop the step!!!!!" << G4endl;
        aStep->GetTrack()->SetTrackStatus(fStopAndKill);
    }

    if ( (aStep->GetPostStepPoint()->GetStepStatus()==fGeomBoundary) &&
            (aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()=="GdLS_phys")&&
            (aStep->IsLastStepInVolume())&&
            (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="Tank_phys"))
    {
        auto info = (NormalTrackInfo *) (aStep->GetTrack()->GetUserInformation());
        std::vector<double> XYZ_GoOutLS{aStep->GetPostStepPoint()->GetPosition().getX(),
                                          aStep->GetPostStepPoint()->GetPosition().getY(),
                                          aStep->GetPostStepPoint()->GetPosition().getZ()};
        info->setXYZGoOutLS(XYZ_GoOutLS);
    }

    if (m_save_step and aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding()==20022)
    {
        evtID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
        trackID = aStep->GetTrack()->GetTrackID();
        step_x = aStep->GetPreStepPoint()->GetPosition().getX();
        step_y = aStep->GetPreStepPoint()->GetPosition().getY();
        step_z = aStep->GetPreStepPoint()->GetPosition().getZ();
        step_t = aStep->GetPreStepPoint()->GetGlobalTime();
        if (aStep->IsFirstStepInVolume())
            VolumeName = aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
        else
            VolumeName = "";
//        G4cout << VolumeName << "\t" << aStep->GetPostStepPoint()->GetVelocity()/(m/s)<<"\t" << aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding()<< G4endl;

               m_tree->Fill();

        if (aStep->IsLastStepInVolume())
        {
            evtID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
            trackID = aStep->GetTrack()->GetTrackID();
            step_x = aStep->GetPostStepPoint()->GetPosition().getX();
            step_y = aStep->GetPostStepPoint()->GetPosition().getY();
            step_z = aStep->GetPostStepPoint()->GetPosition().getZ();
            step_t = aStep->GetPostStepPoint()->GetGlobalTime();
//            VolumeName = aStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName();
            if (aStep->IsFirstStepInVolume())
                VolumeName = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
            else
                VolumeName = "";

            m_tree->Fill();
        }
    }

}
