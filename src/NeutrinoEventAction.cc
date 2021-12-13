#include "G4ios.hh"
#include "G4Event.hh"
#include "../include/MyDataExtract.hh"
#include "NeutrinoAnalysis.hh"
#include "../include/NeutrinoEventAction.hh"


NeutrinoEventAction::NeutrinoEventAction(TString name_file)
{
    name_output_file = name_file;
}

NeutrinoEventAction::~NeutrinoEventAction()
{
}

void NeutrinoEventAction::BeginOfEventAction(const G4Event* anEvent)
{
    /*
    int evtID = anEvent->GetEventID();
    G4cout<<"Begin of "<<evtID<<"th Event..."<<G4endl;

    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillNtupleIColumn(0, 0, evtID);

    analysisManager->AddNtupleRow(0);
     */
}

void NeutrinoEventAction::EndOfEventAction(const G4Event* anEvent)
{
    n_event+=1;
    if(n_event%500==0)std::cout<<n_event<<std::endl;

    G4HCofThisEvent* HCE= fpEventManager->GetEventManager()->GetConstCurrentEvent()->GetHCofThisEvent();
    if(anEvent->GetEventID()==0)
    {
        data_extract=new MyDataExtract(HCE,name_output_file);
        data_extract->RunInitiate();
        generator.CreateTree();
    }
    else data_extract->GetHCE(HCE);

    data_extract->JudgeWhetherHit();
    data_extract->UpdateInformation(anEvent);
//    data_extract->ShowInformation();
//    data_extract->PrintHitCollections();
//    data_extract->DrawEdepCenter();
    data_extract->Fill();
    data_extract->ResetVariable();
    generator.FillTree();

    G4cout<<"End of "<<anEvent->GetEventID()<<"th Event..."<<G4endl;
}
