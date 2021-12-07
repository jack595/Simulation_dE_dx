#ifndef MYDATAEXTRACT_HH
#define MYDATAEXTRACT_HH

#include"G4HCofThisEvent.hh"
#include"TTree.h"
#include"MytrackerHit.hh"
#include<vector>
#include"G4Event.hh"
#include"TFile.h"

class MyVDataExtract
{
public:
    MyVDataExtract(){};
    MyVDataExtract(G4HCofThisEvent* HCE);
    MyVDataExtract(G4HCofThisEvent* HCE,TString name_processed_file);
    virtual ~MyVDataExtract();
    void PrintHitCollections();

    void DataFileInitiate();
    //Aim to open the file with the name "name_processed_file" and create v_trees

    virtual void RunInitiate(){}; //we need to create struct to store the variables we wanna save,so this function has to be
                                //realized after the struct created

    virtual void SetBranches(){};  //This function is needed to modify by the subclass MyDataExtract to set the right address
    //The Structure of trees should be     TFlie------->TTree--->TBranches
                             //(name_processed_file)-->(SDName)--->(ExactInformation)

    void GetHCE(G4HCofThisEvent* HCE);

    virtual void UpdateInformation(const G4Event*){};

    void Fill();

    void Close();

//    virtual void RawDataSaveToTree(TString name_raw_file){};
//    //The Structure of trees should be TFlie------->TTree--->TBranches--->Leaves
//                                //(name_raw_file)-->(SDName)-->(EventID)--->(ExactInformation)


    public:
    std::vector<MyTrackerHitsCollection*> v_MytrackerHitsCollection;
    static G4int n_hitsCollection;
//    static TFile file_save_raw_data;
    static TFile* file_save_processed_data; //Static variables must be initialized in main.cc
    static std::vector<TTree*> v_trees;
    static std::vector<G4String> v_SD_name;
    static TString name_processed_file;
    static std::vector<bool>v_whether_hit;


};

#endif // MYDATAEXTRACT_HH
