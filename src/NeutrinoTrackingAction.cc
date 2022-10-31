#include "NeutrinoTrackingAction.hh"

#include "NeutrinoRunAction.hh"
#include "G4Track.hh"
#include "../include/NormalTrackInfo.hh"
#include "G4OpticalPhoton.hh"
#include "G4EventManager.hh"


NeutrinoTrackingAction::NeutrinoTrackingAction()
{

}

NeutrinoTrackingAction::~NeutrinoTrackingAction()
{

}

void NeutrinoTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
//    G4cout << "We are in UserTracking Action"<<G4endl;
    if(aTrack->GetParentID()==0 && aTrack->GetUserInformation()==0)
    {
        NormalTrackInfo *anInfo = new NormalTrackInfo(aTrack);
        G4Track *theTrack = (G4Track *) aTrack;
        theTrack->SetUserInformation(anInfo);
    }

    NormalTrackInfo *info = (NormalTrackInfo *) (aTrack->GetUserInformation());


    if (!info) {
        return;
    }
    if (aTrack->GetDefinition() == G4OpticalPhoton::Definition()
        and aTrack->GetCreatorProcess()) {
//        G4cout << "###+++ "<< aTrack ->GetCreatorProcess()->GetProcessName() <<std::endl;
    }

    // original OP
    // set the info
    if(true)
    {
        if (aTrack->GetDefinition() == G4OpticalPhoton::Definition()
            and info->isOriginalOP()
            and info->getOriginalOPStartTime() == 0.0) {
            // make sure this track info is not changed before.
            assert(info->getOriginalOPStartTime() == 0.0);
//            G4cout << "------ original OP" << std::endl;
            info->setOriginalOPStartTime(aTrack->GetGlobalTime());
        }

        // An example: Get the parent particle name from track info
        if (info && aTrack->GetDefinition()!=G4OpticalPhoton::Definition()) {
            // const G4String& parent_name = info->getParentName();
            // G4cout << " The parent of " << aTrack->GetDefinition()->GetParticleName()
            //        << " is " << parent_name
            //        << G4endl;
        }

    }


}

void NeutrinoTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{

    if (aTrack->GetParentID() == 0) {
        // this is the primary particle
        const G4ThreeVector& pos = aTrack->GetPosition();
//        LogDebug << "!!!Primary Track " << aTrack->GetTrackID() << ": ";
//        LogDebug << "+ " << pos.x() << " " << pos.y() << " " << pos.z() << std::endl;
//        LogDebug << "+ " << aTrack->GetKineticEnergy() << std::endl;
    }
    G4TrackingManager* tm = G4EventManager::GetEventManager()
            -> GetTrackingManager();
    G4TrackVector* secondaries = tm->GimmeSecondaries();
    if(secondaries)
    {
        NormalTrackInfo* info = (NormalTrackInfo*)(aTrack->GetUserInformation());

        if (!info) {
            return;
        }

        size_t nSeco = secondaries->size();
        if(nSeco>0)
        {
            for(size_t i=0;i<nSeco;i++)
            {

                // make sure the secondaries' track info is empty
                // if already attached track info, skip it.
                if ((*secondaries)[i]->GetUserInformation()) {
//                    LogDebug << "The secondary already has user track info. skip creating new one" << std::endl;
                    continue;
                }
                NormalTrackInfo* infoNew = new NormalTrackInfo(info);
                if(true)
                {
                    // cerekov tag
                    if ((*secondaries)[i]->GetCreatorProcess()
                        and (*secondaries)[i]->GetCreatorProcess()->GetProcessName() == "Cerenkov") {
                        infoNew->setFromCerenkov();
//                        LogDebug << "### infoNew->setFromCerenkov()" << std::endl;
                    }
                    // reemission tag
                    // + parent track is an OP
                    // + secondary is also an OP
                    // + the creator process is Scintillation
                    if (aTrack->GetDefinition() == G4OpticalPhoton::Definition()
                        and (*secondaries)[i]->GetDefinition() == G4OpticalPhoton::Definition()
                        and (*secondaries)[i]->GetCreatorProcess()->GetProcessName() == "Scintillation") {
                        infoNew->setReemission();
                    }
                    // original optical photon tag
                    if (aTrack->GetDefinition() != G4OpticalPhoton::Definition()
                        and (*secondaries)[i]->GetDefinition() == G4OpticalPhoton::Definition()
                            ) {
//                        LogDebug << "------ original OP" << std::endl;
                        infoNew->setOriginalOP();
                    }
                }
                // save parent track info
                infoNew->setParentName(aTrack->GetDefinition()->GetParticleName());

                (*secondaries)[i]->SetUserInformation(infoNew);
            }
        }
    }



}
