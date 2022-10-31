#include "NeutrinoAnalysis.hh"
#include "NeutrinoRunAction.hh"
#include "NeutrinoRunMessenger.hh"

#include "G4Run.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4Event.hh"
#include "../include/MyVDataExtract.hh"
#include "../include/NeutrinoPrimaryGeneratorAction.hh"
#include "MyCounterSD.hh"

NeutrinoRunAction::NeutrinoRunAction()
{
    // run messenger
    runMessenger = new NeutrinoRunMessenger(this);

    // analysis manager
    /*
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetNtupleMerging(true);
    analysisManager->SetVerboseLevel(1);
    analysisManager->SetFileName("test.root");

    analysisManager->CreateNtuple("evt", "event information");
    analysisManager->CreateNtupleIColumn("evtID");
     */
}

NeutrinoRunAction::~NeutrinoRunAction()
{
    delete runMessenger;
//    delete G4AnalysisManager::Instance();
}

void NeutrinoRunAction::BeginOfRunAction(const G4Run* aRun)
{
//    auto analysisManager = G4AnalysisManager::Instance();
//    analysisManager->OpenFile();
}

void NeutrinoRunAction::EndOfRunAction(const G4Run* aRun)
{

    MyVDataExtract data_extract;
    NeutrinoPrimaryGeneratorAction generator;
    generator.WriteTree();
    MyCounterSD::WriteTTree();
    data_extract.Close();

//    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
//    analysisManager->Write();
//    analysisManager->CloseFile();
}
