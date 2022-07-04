#include"../include/MyVDataExtract.hh"
#include"G4HCofThisEvent.hh"
#include"../include/MytrackerHit.hh"
#include"TFile.h"
#include"TTree.h"

MyVDataExtract::MyVDataExtract(G4HCofThisEvent *HCE)
{
    n_hitsCollection=HCE->GetCapacity();
    for(int i=0;i<n_hitsCollection;i++)
    {
        v_MytrackerHitsCollection.push_back((MyTrackerHitsCollection*)HCE->GetHC(i));
        v_SD_name.push_back(v_MytrackerHitsCollection[i]->GetSDname());
    }
}

MyVDataExtract::MyVDataExtract(G4HCofThisEvent *HCE,TString name_file)
{
    n_hitsCollection=HCE->GetCapacity();
    for(int i=0;i<n_hitsCollection;i++)
    {
        v_MytrackerHitsCollection.push_back((MyTrackerHitsCollection*)HCE->GetHC(i));
        v_SD_name.push_back(v_MytrackerHitsCollection[i]->GetSDname());
    }
    name_processed_file=name_file;
}

MyVDataExtract::~MyVDataExtract()
{
    v_MytrackerHitsCollection.clear();
}

void MyVDataExtract::PrintHitCollections()
{
    for(int i=0;i<n_hitsCollection;i++)
    {
    MyTrackerHitsCollection* fHitsCollection=v_MytrackerHitsCollection[i];
    G4int nofHits=fHitsCollection->entries();
    G4cout << G4endl
           << "-------->Hits Collection: in this event they are " << nofHits
           << " hits in the tracker chambers: " << G4endl;
    G4cout<<"SensitiveDetectorName: "<<fHitsCollection->GetSDname()<<G4endl;
    G4cout<<"collectionName[0]: "<<fHitsCollection->GetName()<<G4endl;
    for ( G4int j=0; j<nofHits; j++ ) (*fHitsCollection)[j]->Print();
    if(nofHits==0) G4cout<<G4endl;
    }
}


void MyVDataExtract::DataFileInitiate()
{
    for(int i=0;i<n_hitsCollection;i++)
    {
        v_trees.push_back(new TTree(v_SD_name[i],v_SD_name[i]));
            }
    file_save_processed_data=TFile::Open(name_processed_file,"recreate");
}

void MyVDataExtract::GetHCE(G4HCofThisEvent *HCE)
{
    for(int i=0;i<n_hitsCollection;i++)
    {
        v_MytrackerHitsCollection[i]=(MyTrackerHitsCollection*)HCE->GetHC(i);
//        v_SD_name.push_back(v_MytrackerHitsCollection[i]->GetSDname());
    }

}

void MyVDataExtract::Fill()
{
    for(int i=0;i<n_hitsCollection;i++)
    {
        if(v_whether_hit[i]==true)
        {
            v_trees[i]->Fill();
            G4cout<<"tree Fill "<<i<<G4endl;
            v_whether_hit[i]=false;
        }
    }

}

void MyVDataExtract::Close()
{
    for(int i=0;i<n_hitsCollection;i++)
    {
        v_trees[i]->Write();
    }
    file_save_processed_data->Close();
}


