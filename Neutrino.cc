#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UserTrackingAction.hh"

#include "NeutrinoPrimaryGeneratorAction.hh"
#include "NeutrinoDetectorConstruction.hh"

#include "NeutrinoRunAction.hh"
#include "NeutrinoEventAction.hh"
#include "NeutrinoTrackingAction.hh"
#include "NeutrinoSteppingAction.hh"

#include "NeutrinoPhysicsList.hh"
#include "G4VModularPhysicsList.hh"

#include "G4VisManager.hh"
#include "G4VisExecutive.hh"

#include "G4UIExecutive.hh"
#include "G4UImanager.hh"

#include "./include/MyVDataExtract.hh"
#include "./include/MyDataExtract.hh"
#include "NeutrinoPrimaryGeneratorAction.hh"
#include "TString.h"

//static variable initialization
TFile*  MyVDataExtract::file_save_processed_data;
std::vector<TTree*> MyVDataExtract::v_trees;
//TTree *MyDataExtract::v_trees[2];
std::vector<InformationToSave> MyDataExtract::v_information_to_save;
std::vector<G4String> MyVDataExtract::v_SD_name;
TString MyVDataExtract::name_processed_file;
G4int MyVDataExtract::n_hitsCollection;
std::vector<bool> MyVDataExtract::v_whether_hit;
TTree* NeutrinoPrimaryGeneratorAction::tree_generator;
double NeutrinoPrimaryGeneratorAction::Energy_init;
std::vector<double> NeutrinoPrimaryGeneratorAction::XYZ;
int main(int argc, char **argv){

    int seed = 10010;
    char* macName = NULL;
    char* name_outfile = NULL;
    float L_LS = 3;// mm
    float L_SpeedBump = 1;//cm
    bool add_rad_source = false;
    float distance_PMT_near = 1; //cm

    for(int i=1;i<argc;i++){
        if(strcmp(argv[i],"-seed") == 0){
            i++;
            seed = std::atoi(argv[i]);
        }
        else if (strcmp(argv[i], "-d_PMT")==0)
        {
            i++;
            distance_PMT_near = std::atof(argv[i]);
        }
        else if(strcmp(argv[i],"-mac") == 0){
            i++;
            macName = argv[i];
        }else if(strcmp(argv[i],"-output") == 0){
            i++;
            name_outfile = argv[i];
        }else if(strcmp(argv[i], "-L_LS") == 0 ){
            i++;
            L_LS = std::atof(argv[i]);
        }else if(strcmp(argv[i], "-L_SpeedBump") == 0 )
        {
            i++;
            L_SpeedBump = std::atof(argv[i]);
        }else if(strcmp(argv[i], "-AddSource")==0)
        {
            i++;
            add_rad_source = true;
            G4cout << "Turn on add source mode!"<<G4endl;
        }
    }

    CLHEP::HepRandom::setTheSeed(seed);

    G4RunManager* runManager = new G4RunManager;
    /* Initialization classes (mandatory) */
    // detector
    NeutrinoDetectorConstruction* detectorConstruction = new NeutrinoDetectorConstruction();
    std::cout<< "LS Length:\t"<<L_LS<< " mm" <<std::endl;
    detectorConstruction->SetLengthOfLS(L_LS);
    detectorConstruction->SetLengthOfSpeedBump(L_SpeedBump);
    detectorConstruction->SetDistanceOfNearPMT(distance_PMT_near);
    runManager->SetUserInitialization(detectorConstruction);
    
    // physics list
    G4VModularPhysicsList* physicsList = new NeutrinoPhysicsList;
    runManager->SetUserInitialization(physicsList);
    
    // generator
    G4VUserPrimaryGeneratorAction* genAction = new NeutrinoPrimaryGeneratorAction(add_rad_source);
    runManager->SetUserAction(genAction);


    /* User Action classes */
    // run action
    NeutrinoRunAction* runAction = new NeutrinoRunAction();
    runManager->SetUserAction(runAction);

    if (!name_outfile)
        name_outfile = "../try_save_information.root";

    // event action
    NeutrinoEventAction* eventAction = new NeutrinoEventAction((TString)name_outfile);
    runManager->SetUserAction(eventAction);
    
    // tracking action
    G4UserTrackingAction* trackingAction = new NeutrinoTrackingAction();
    runManager->SetUserAction(trackingAction);
    
    // stepping action
    G4UserSteppingAction* steppingAction = new NeutrinoSteppingAction();
    runManager->SetUserAction(steppingAction);

    runManager->Initialize();

    // To Turn on interactive mode , uncomment code block below !!!!

    /* Visualization */
    // initialize visualization
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
//
//    // detect interactive mode
//    G4UIExecutive* ui = NULL;
//    if(argc == 1)
//    {
//        ui = new G4UIExecutive(argc, argv);
//    }
//
//    // get pointer to the User Interface manager
//    G4UImanager* UImanager = G4UImanager::GetUIpointer();
//
//    if(ui)
//    {
//        // interactive mode
//        UImanager->ApplyCommand("/control/execute vis.mac");
//        ui->SessionStart();
//        delete ui;
//    }
//    else
//    {
//        // batch mode
//        G4String command = "/control/execute ";
//        UImanager->ApplyCommand(command + macName);
//    }

    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    if (!macName)
    {
        macName = "vis.mac";
    }

    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command + macName);

    delete visManager;
    delete runManager;

    return 0;
}
