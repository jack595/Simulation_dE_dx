#include"../include/MyCounterSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include"G4Track.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessVector.hh"
#include "G4ProcessManager.hh"
#include "../include/NormalTrackInfo.hh"
#include "TROOT.h"


MyCounterSD::MyCounterSD(const G4String &name, const G4String &hitsCollectionName)
    :G4VSensitiveDetector(name)
{
    name_SDHitsCollection = hitsCollectionName;
//    collectionName.insert(hitsCollectionName);
    G4cout << "------------>Creating TTree for "<<hitsCollectionName<<" Counter "<<G4endl;
    gROOT->ProcessLine("#include <vector>");
    map_name2TTree.insert(std::pair<TString, TTree*>(name_SDHitsCollection, new TTree(name_SDHitsCollection, name_SDHitsCollection)));
    map_name2TTree.at(name_SDHitsCollection)->Branch("v_n_hits", &v_n_hits);

}

MyCounterSD::~MyCounterSD()
{
}

void MyCounterSD::Initialize(G4HCofThisEvent *hitsCollection)
{
    v_n_hits.clear();
}

G4bool MyCounterSD::ProcessHits(G4Step *aStep, G4TouchableHistory *)
{
    G4double edep=aStep->GetTotalEnergyDeposit();

    if(edep==0.  )
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

    int chamberID = aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();

    // Count N of Hits in SD
    if (chamberID+1>v_n_hits.size())
        v_n_hits.push_back(0);
    else
        v_n_hits[chamberID] += 1;

//    std::cout<< "Length:\t" << v_n_hits.size() << std::endl;
//    std::cout<< "ChamberID:\t" << chamberID << std::endl;
//    std::cout<< "n_hits:\n" ;
//    for (int n_hit:v_n_hits)
//        std::cout << n_hit << "\t";
//    std::cout << std::endl;

    return true;

}

void MyCounterSD::EndOfEvent(G4HCofThisEvent *)
{
   map_name2TTree.at(name_SDHitsCollection)->Fill();
}





