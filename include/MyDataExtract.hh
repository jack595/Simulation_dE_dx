#pragma once
#include"MyVDataExtract.hh"
#include<vector>
#include"TTree.h"
#include"G4Vector3D.hh"
#include"G4Run.hh"

struct InformationToSave
{
    Double_t Edep_event;
    Int_t evtID;
    Double_t center_Edep[3]{};
    std::vector<int> step_Chamber_ID;
    std::vector<double> step_x;
    std::vector<double> step_y;
    std::vector<double> step_z;
    std::vector<double> step_t;
    std::vector<double> step_Edep;
    std::vector<double> step_Equench;
    std::vector<double> step_dx;
    std::vector<double> step_KineticE;
    std::vector<double> step_op_start_time;
    std::vector<int> step_pdgID;
    std::vector<int> step_trackID;
    std::vector<int> step_isCherenkov;
    std::vector<int> step_ParentID;
    std::vector<int> step_seed;
    std::vector<int> step_isReemission;
    std::vector<int> step_processName;

    std::vector<double> step_X_GoOutLS;
    std::vector<double> step_Y_GoOutLS;
    std::vector<double> step_Z_GoOutLS;




    InformationToSave(){};
    InformationToSave(const Double_t E,
                      const Int_t EventID,
                      const std::vector<Double_t> xyz_center);
//    void SetValue(Double_t E,
//                  Int_t EventID,
//                  std::vector<Double_t> xyz_center);

    void Print();
};    //Information needed to saved,and filled into the TTree

class MyDataExtract: public MyVDataExtract
{
public:
    MyDataExtract(){};
    MyDataExtract(G4HCofThisEvent* HCE);
    MyDataExtract(G4HCofThisEvent* HCE,TString name_processed_file);
    virtual void RunInitiate();
    virtual ~MyDataExtract();
//    virtual void ProcessedDataToTree(){};
    virtual void SetBranches();
    virtual void UpdateInformation(const G4Event *event); //input G4Event is to get evtID
    void ShowInformation();
    void JudgeWhetherHit();
    G4ThreeVector GetEdepCenter(MyTrackerHitsCollection* fHitsCollection, const G4double &Edep_total);
    void DrawEdepCenter();
    void ResetVariable();
private:
    static std::vector<InformationToSave>v_information_to_save;

};

