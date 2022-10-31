#include"../include/MyTrackerSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include"G4Track.hh"
#include "../include/JUNOTool.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessVector.hh"
#include "G4ProcessManager.hh"
#include "../include/NormalTrackInfo.hh"


MyTrackerSD::MyTrackerSD(const G4String &name, const G4String &hitsCollectionName)
    :G4VSensitiveDetector(name),fHitsCollection(NULL)
{
    name_SDHitsCollection = hitsCollectionName;
    collectionName.insert(hitsCollectionName);
}

MyTrackerSD::~MyTrackerSD()
{
}

void MyTrackerSD::Initialize(G4HCofThisEvent *hitsCollection)
{
    fHitsCollection=new MyTrackerHitsCollection(SensitiveDetectorName,
                                                collectionName[0]);

    G4int hcID
            =G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    hitsCollection->AddHitsCollection(hcID,fHitsCollection);  //here we only use one fHitsCollection in the G4HCofThisEvent
//    G4cout<<"collectionNumber: "<<hitsCollection->GetNumberOfCollections()<<G4endl;
//    G4cout<<"hcID: "<<hcID<<G4endl;
}

G4bool MyTrackerSD::ProcessHits(G4Step *aStep, G4TouchableHistory *)
{
    G4double edep=aStep->GetTotalEnergyDeposit();
//    std::cout<< "I'm in SD volume" <<std::endl;
//    if(edep==0. && name_SDHitsCollection.Contains("LS")) return false;

//    if(edep==0. && not name_SDHitsCollection.Contains("Si")  )
//        return false;

    if(edep==0. && not name_SDHitsCollection.Contains("calib")   ) {
        return false;
    }
//    std::cout<< "SD Volume:\t"<< name_SDHitsCollection << std::endl;

    if (aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding()==20022 && (name_SDHitsCollection.Contains("LS") ||name_SDHitsCollection.Contains("PSVol") ))
        return false;


    if (name_SDHitsCollection.Contains("PMT") and not name_SDHitsCollection.Contains("calib"))
    {
        // TODO: now it only support the single PE.
        // = only accept the optical photon

        G4Track* track = aStep->GetTrack();
        if (track->GetDefinition() != G4OpticalPhoton::Definition()) {
            return false;
        }
        G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
        G4StepPoint* postStepPoint = aStep->GetPostStepPoint();

        // LT
        // = Due to update of Geant4, now OpAbsorption will also cause non-zero edep.
        // = Hence we need to check the OP boundary.
        G4bool isOnBoundary = (postStepPoint->GetStepStatus() == fGeomBoundary);
        if (not isOnBoundary) {
            return false;
        }

        static G4ThreadLocal G4OpBoundaryProcess* boundary_proc=NULL;
        if (!boundary_proc) {
            G4ProcessManager* OpManager =
                    G4OpticalPhoton::OpticalPhoton()->GetProcessManager();
            if (OpManager) {
                G4int MAXofPostStepLoops =
                        OpManager->GetPostStepProcessVector()->entries();
                G4ProcessVector* fPostStepDoItVector =
                        OpManager->GetPostStepProcessVector(typeDoIt);
                for ( G4int i=0; i<MAXofPostStepLoops; i++) {
                    G4VProcess* fCurrentProcess = (*fPostStepDoItVector)[i];
                    G4OpBoundaryProcess* op =  dynamic_cast<G4OpBoundaryProcess*>(fCurrentProcess);
                    if (op) {
                        boundary_proc = op;
                        break;
                    }
                }
            }

        }

        if (!boundary_proc) {
            G4cout << "Can't locate OpBoundaryProcess." << G4endl;
            return false;
        }

        G4OpBoundaryProcessStatus theStatus = Undefined;
        theStatus = boundary_proc->GetStatus();

        if (theStatus != Detection) {
            return false;
        }

    }

    auto newHit=new MytrackerHit();

    newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
    newHit->SetChamberNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
    newHit->SetPos(aStep->GetPostStepPoint()->GetPosition());
    newHit->SetPDGID(aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
    newHit->time = aStep->GetPreStepPoint()->GetGlobalTime();

    newHit->SetEdep(edep);
    newHit->fEquench = EdepToQquench(aStep);
    newHit->SetParticleName(aStep->GetTrack()->GetParticleDefinition()->GetParticleName());
    newHit->fStepLength = aStep->GetStepLength();
    newHit->fKineticE = aStep->GetPreStepPoint()->GetKineticEnergy();
    newHit->ParentID = aStep->GetTrack()->GetParentID();
    newHit->seed = seed_SD;

    auto trackInfo = (NormalTrackInfo*)aStep->GetTrack()->GetUserInformation();
    if (aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding()==20022)
    {
        std::vector<double> XYZ_GoOutLS = trackInfo->getXYZGoOutLS();
        newHit->m_XYZ_GoOutLS = XYZ_GoOutLS;
//        G4cout << "XYZ(Go Out of LS):\t" << XYZ_GoOutLS[0]<< "\t"<<
//               XYZ_GoOutLS[1]<<"\t"<< XYZ_GoOutLS[2]<< G4endl;
    }
    newHit->isReemission = trackInfo->isReemission();
    newHit->m_op_start_time = trackInfo->getOriginalOPStartTime();
    newHit->isCherenkov = trackInfo->isFromCerenkov();


//    if (newHit->fpdgID==20022 && aStep->GetTrack()->GetCreatorProcess()->GetProcessName()=="Cerenkov")
//        newHit->isCherenkov = 1;
//    else
//        newHit->isCherenkov = 0;

    fHitsCollection->insert(newHit);

//    G4cout << name_SDHitsCollection << G4endl;
//    newHit->Print();
//    G4cout<<"How to prove I am running!"<<G4endl;

    return true;

}

void MyTrackerSD::EndOfEvent(G4HCofThisEvent *)
{
//        G4int nofHits=fHitsCollection->entries();
//        G4cout << G4endl
//               << "-------->Hits Collection: in this event they are " << nofHits
//               << " hits in the tracker chambers: " << G4endl;
//        G4cout<<"SensitiveDetectorName: "<<SensitiveDetectorName<<G4endl;
//        G4cout<<"collectionName[0]: "<<collectionName[0]<<G4endl;
//        for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();


}




