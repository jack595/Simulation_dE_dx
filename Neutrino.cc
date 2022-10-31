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
#include "MyTrackerSD.hh"
#include "MyCounterSD.hh"

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
int NeutrinoPrimaryGeneratorAction::m_evtID_gen;
int MyTrackerSD::seed_SD;
std::map<TString, TTree*> MyCounterSD::map_name2TTree;
void MyCounterSD::WriteTTree()
{
//    for (auto tree_counter:v_tree_counter)
//        tree_counter->Write();
    for (const auto & [name, tree]:map_name2TTree)
        tree->Write();
}

int main(int argc, char **argv){

    int seed = 10010;
    char* macName = NULL;
    char* name_outfile = NULL;
    float L_LS = 3;// mm
    float L_PS = 10; //mm
    float LY_PS = 6000 ;// MeV
    float thickness_tank = 1; //mm
    float L_SpeedBump = 1;//cm
    float shiftLength_RAYLEIGH = 0.;//m
    bool add_rad_source = false;
    float distance_PMT_near = 1; //cm
    bool optical = false;
    bool use_tank = false;
    bool use_quartz = true;
    float r_LS = 2.5; //cm
    bool add_ESR = false;
    bool add_calib = false;
    bool turn_PS_into_LS = false;

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
        }
        else if(strcmp(argv[i], "-L_SpeedBump") == 0 )
        {
            i++;
            L_SpeedBump = std::atof(argv[i]);
        }
        else if(strcmp(argv[i], "-ShiftLength_RAYLEIGH") == 0 )
        {
            i++;
            shiftLength_RAYLEIGH = std::atof(argv[i]);
        }
        else if(strcmp(argv[i], "-AddSource")==0)
        {
            add_rad_source = true;
            G4cout << "Turn on add source mode!"<<G4endl;
        }else if(strcmp(argv[i], "-optical")==0)
        {
            optical = true;
            G4cout << "Running optical mode.........."<<G4endl;
        }else if(strcmp(argv[i], "-L_tank")==0)
        {
            i++;
            thickness_tank = std::atof(argv[i]);
        }else if(strcmp(argv[i], "-UseTank")==0)
        {
            use_tank = true;
            G4cout << "Added LS tank.........."<<G4endl;
        }
        else if(strcmp(argv[i], "-UseAcrylic")==0)
        {
            use_quartz = false;
            G4cout<< "Using acrylic tank........" <<G4endl;
        } else if (strcmp(argv[i], "-r_LS")==0)
        {
            i++;
            r_LS = std::atof(argv[i]);
        }
        else if(strcmp(argv[i], "-Add_ESR")==0)
        {
            add_ESR = true;
        }
        else if(strcmp(argv[i], "-Add_calib")==0)
        {
            add_calib = true;
            G4cout << "Turn on Calib Detector.." << G4endl;
        }
        else if(strcmp(argv[i], "-PS_Into_LS")==0)
        {
            // Change Plastic Scintillator into Liquid Scintillator
            // All the settings for PS still apply to LS sample
            turn_PS_into_LS = true;
            std::cout << "Turn PS into LS ...." << std::endl;
        }
        else if (strcmp(argv[i], "-L_PS") == 0 ){
            i++;
            L_PS = std::atof(argv[i]);
        }
         else if (strcmp(argv[i], "-LY_PS") == 0 ){
            i++;
            LY_PS = std::atof(argv[i]);
        }


    }

    // ----------- Print Simulation Configures ---------------------------
    std::cout<< "LS Length:\t"<<L_LS<< " mm" <<std::endl;
    G4cout << "Radius of LS:\t"<<r_LS<< " cm"<<G4endl;
    G4cout << "Thickness of Tank:\t"<<thickness_tank<<" mm"<<G4endl;
    G4cout << "d_PMT_near:\t" << distance_PMT_near<<" cm" << G4endl;
    G4cout << "Thickness of PS:\t" << L_PS << "mm"<< G4endl;
    G4cout << "LY of PS :\t"<< LY_PS << "/MeV" << G4endl;
    if (not turn_PS_into_LS) G4cout << "Using PS ...." << G4endl;



    CLHEP::HepRandom::setTheSeed(seed);

    G4RunManager* runManager = new G4RunManager;
    /* Initialization classes (mandatory) */
    // detector
    NeutrinoDetectorConstruction* detectorConstruction = new NeutrinoDetectorConstruction();
    detectorConstruction->SetLengthOfLS(L_LS);
    detectorConstruction->SetLengthOfPS(L_PS);
    detectorConstruction->SetShiftLengthOfRAYLEIGH(shiftLength_RAYLEIGH);
    detectorConstruction->SetLengthOfSpeedBump(L_SpeedBump);
    detectorConstruction->SetTurnPSintoLS(turn_PS_into_LS);
    detectorConstruction->SetAddESR(add_ESR);
    detectorConstruction->SetAddCalib(add_calib);
    detectorConstruction->SetDistanceOfNearPMT(distance_PMT_near);
    detectorConstruction->SetThicknessOfTank(thickness_tank);
    detectorConstruction->WhetherTurnOnTank(use_tank);
    detectorConstruction->WhetherUseQuartz(use_quartz);
    detectorConstruction->SetRofLS(r_LS);
    detectorConstruction->SetLightYieldOfPS(LY_PS);
    runManager->SetUserInitialization(detectorConstruction);

    MyTrackerSD::seed_SD = seed;
    
    // physics list
//    G4VModularPhysicsList* physicsList = new NeutrinoPhysicsList;
    NeutrinoPhysicsList* physicsList = new NeutrinoPhysicsList;
    physicsList->SetOpticalList(optical);
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
