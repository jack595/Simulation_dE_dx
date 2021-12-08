#include"../include/MyTrackerSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include"G4Track.hh"
#include "../include/JUNOTool.hh"


MyTrackerSD::MyTrackerSD(const G4String &name, const G4String &hitsCollectionName)
    :G4VSensitiveDetector(name),fHitsCollection(NULL)
{
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
    if(edep==0.) return false;
    auto newHit=new MytrackerHit();

    newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
    newHit->SetChamberNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
    newHit->SetEdep(edep);
    newHit->fEquench = EdepToQquench(aStep);
    newHit->SetPos(aStep->GetPostStepPoint()->GetPosition());
    newHit->SetParticleName(aStep->GetTrack()->GetParticleDefinition()->GetParticleName());
    newHit->SetPDGID(aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
    newHit->time = aStep->GetPreStepPoint()->GetGlobalTime();
    newHit->fStepLength = aStep->GetStepLength();

    fHitsCollection->insert(newHit);

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




