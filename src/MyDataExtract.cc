#include"../include/MyDataExtract.hh"
#include"TFile.h"
#include"TTree.h"
#include<vector>
#include"G4Event.hh"
#include"../include/vec_op.hh"
#include"G4UnitsTable.hh"
#include"G4SystemOfUnits.hh"
#include"G4VVisManager.hh"
#include"G4Circle.hh"
#include"G4VisAttributes.hh"
#include "TROOT.h"
MyDataExtract::MyDataExtract(G4HCofThisEvent *HCE):
    MyVDataExtract(HCE)
{
}

MyDataExtract::MyDataExtract(G4HCofThisEvent *HCE,TString name_processed_file):
MyVDataExtract(HCE,name_processed_file)
{
}

void MyDataExtract::RunInitiate()
{
        for(int i=0;i<n_hitsCollection;i++)
        {
            v_information_to_save.push_back(InformationToSave(1.+i,1+i,{1.+i,1.+i,1.+i}));
            v_whether_hit.push_back(true);
        }

        DataFileInitiate();
        SetBranches();
}

MyDataExtract::~MyDataExtract()
{

}

void MyDataExtract::SetBranches()
{
    gROOT->ProcessLine("#include <vector>");
    for(int i=0;i<n_hitsCollection;i++)
    {
        v_trees[i]->Branch("Edep_event",
                       &v_information_to_save[i].Edep_event,
                       "Edep_event/D");
        v_trees[i]->Branch("evtID",
                            &v_information_to_save[i].evtID,
                            "evtID/I");
        v_trees[i]->Branch("xyz_center",
                  v_information_to_save[i].center_Edep,
                           "center_Edep[3]/D");


        v_trees[i]->Branch("step_pdgID", &v_information_to_save[i].step_pdgID);
        v_trees[i]->Branch("step_trackID", &v_information_to_save[i].step_trackID);
        v_trees[i]->Branch("step_x", &v_information_to_save[i].step_x);
        v_trees[i]->Branch("step_y", &v_information_to_save[i].step_y);
        v_trees[i]->Branch("step_z",&v_information_to_save[i].step_z);
        v_trees[i]->Branch("step_t", &v_information_to_save[i].step_t);
        v_trees[i]->Branch("step_Edep", &v_information_to_save[i].step_Edep);
        v_trees[i]->Branch("step_Equench", &v_information_to_save[i].step_Equench);
        v_trees[i]->Branch("step_dx", &v_information_to_save[i].step_dx);
        v_trees[i]->Branch("step_chamberID", &v_information_to_save[i].step_Chamber_ID);
    }
}

void MyDataExtract::ResetVariable() {

    for (int i = 0; i < n_hitsCollection; i++) {
        v_information_to_save[i].step_x.clear();
        v_information_to_save[i].step_y.clear();
        v_information_to_save[i].step_z.clear();
        v_information_to_save[i].step_pdgID.clear();
        v_information_to_save[i].step_trackID.clear();
        v_information_to_save[i].step_t.clear();
        v_information_to_save[i].step_Edep.clear();
        v_information_to_save[i].step_Equench.clear();
        v_information_to_save[i].step_dx.clear();
        v_information_to_save[i].step_Chamber_ID.clear();
    }
}

    void MyDataExtract::UpdateInformation(const G4Event* event)
{
        for(int i=0;i<n_hitsCollection;i++)
        {
            if(v_whether_hit[i]==true)
            {
            MyTrackerHitsCollection* fHitsCollection=v_MytrackerHitsCollection[i];

            v_information_to_save[i].evtID=event->GetEventID();

//            G4ThreeVector v_position_EdepCenter=GetEdepCenter(fHitsCollection,v_information_to_save[i].Edep_event);
//            for(int j=0;j<v_position_EdepCenter.SIZE;j++)
//                v_information_to_save[i].center_Edep[j] = v_position_EdepCenter[j]*100;

            for(int j=0;j<fHitsCollection->entries();j++)
            {
                auto hit = (*fHitsCollection)[j];
                v_information_to_save[i].step_x.push_back(hit->GetPos()[0]);
                v_information_to_save[i].step_y.push_back(hit->GetPos()[1]);
                v_information_to_save[i].step_z.push_back(hit->GetPos()[2]);
                v_information_to_save[i].step_pdgID.push_back(hit->GetPdgID());
                v_information_to_save[i].step_trackID.push_back(hit->GetTrackID());
                v_information_to_save[i].step_t.push_back(hit->time);
                v_information_to_save[i].step_Edep.push_back(hit->GetEdep());
                v_information_to_save[i].step_Equench.push_back(hit->fEquench);
                v_information_to_save[i].step_dx.push_back(hit->fStepLength);
                v_information_to_save[i].step_Chamber_ID.push_back(hit->GetChamberNb());
            }

            }
        }

}

void MyDataExtract::ShowInformation()
{
    for ( G4int i=0; i<n_hitsCollection; i++ )
    {
        G4cout<<v_SD_name[i]<<" ";
        v_information_to_save[i].Print();
    }
}

void MyDataExtract::JudgeWhetherHit()
{
    for(int i=0;i<n_hitsCollection;i++)
    {
        v_information_to_save[i].Edep_event=0;

        G4double Edep_total=0;
        MyTrackerHitsCollection* fHitsCollection=v_MytrackerHitsCollection[i];
        G4int nofHits = fHitsCollection->entries();
        for ( G4int j=0; j<nofHits; j++ ) Edep_total+=(*fHitsCollection)[j]->fEdep;
        v_information_to_save[i].Edep_event=Edep_total;

    }
    for(int i=0;i<n_hitsCollection;i++)
    {
        G4cout << v_SD_name[i] << " " << v_MytrackerHitsCollection[i]->entries() << G4endl;
        if (v_SD_name[i].contains("PMT"))
        {
            if (v_MytrackerHitsCollection[i]->entries()!=0)
                v_whether_hit[i] =  true;
            else
                v_whether_hit[i] = false;
        }
        else
        {
            if(v_information_to_save[i].Edep_event!=0.)
                v_whether_hit[i]=true;
            else
                v_whether_hit[i]=false;
        }

    }

}

G4ThreeVector MyDataExtract::GetEdepCenter(MyTrackerHitsCollection *fHitsCollection,const G4double& Edep_total)
{
    G4ThreeVector v_position_EdepCenter{0.,0.,0.};
    for ( G4int i=0; i<fHitsCollection->entries(); i++ )
    {
    G4ThreeVector v_position_hit=(*fHitsCollection)[i]->fPos;
//    G4cout<<"v_position_event: "<<G4BestUnit(v_position_hit,"Length")<<G4endl;
//    G4cout<<"Edep: "<<G4BestUnit((*fHitsCollection)[i]->GetEdep(),"Energy")<<G4endl;
    v_position_EdepCenter+=v_position_hit*(*fHitsCollection)[i]->GetEdep();

    }
    v_position_EdepCenter/=Edep_total;
    return v_position_EdepCenter;


}

void MyDataExtract::DrawEdepCenter()
{
    G4VVisManager* pVVisManager =G4VVisManager::GetConcreteInstance();
    for(int i=0;i<n_hitsCollection;i++)
    {
//    G4cout<<"Draw center of Edep circle "<<G4endl;
        if(v_whether_hit[i]==true)
        {
            G4ThreeVector v_EdepCenter;
            for(int j=0;j<3;j++)v_EdepCenter[j]=v_information_to_save[i].center_Edep[j]*cm;
            G4Circle circle(v_EdepCenter);
            circle.SetScreenSize(5.*mm);
            circle.SetFillStyle(G4Circle::filled);
            G4Color colour(1.,0.,0.);
            G4VisAttributes attribs(colour);
            circle.SetVisAttributes(attribs);
            pVVisManager->Draw(circle);
            G4cout<<"Draw center of Edep circle "<<i<<G4endl;
        }
    }
}

InformationToSave::InformationToSave(const Double_t E,const Int_t EventID,const std::vector<Double_t> xyz_center)
{
    Edep_event=E;
    evtID=EventID;
    for(int i=0;i<xyz_center.size();i++)center_Edep[i]=xyz_center[i];
}

//void InformationToSave::SetValue(const Double_t E, const Int_t EventID, const std::vector<Double_t> xyz_center)
//{
//    Edep_event=E;
//    evtID=EventID;
//    for(int i=0;i<xyz_center.size();i++)center_Edep[i]=xyz_center[i];
//}

void InformationToSave::Print()
{
    G4cout<<"evtID: "<<evtID<<" Edep_event: "<<Edep_event<<G4endl;
    G4cout<<"Edep_Center: "<<center_Edep[0]<<" "<<center_Edep[1]<<" "<<center_Edep[2]<<G4endl;



}
