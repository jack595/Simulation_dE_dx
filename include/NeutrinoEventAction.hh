#ifndef NeutrinoEventAction_h
#define NeutrinoEventAction_h 1

#include "globals.hh"
#include "G4UserEventAction.hh"
#include "NeutrinoRunAction.hh"
#include "MyDataExtract.hh"
#include "G4EventManager.hh"
#include "NeutrinoPrimaryGeneratorAction.hh"
#include "TString.h"

class G4Event;
class NeutrinoRunAction;

class NeutrinoEventAction : public G4UserEventAction{
    public:
        NeutrinoEventAction(TString name_file);
        ~NeutrinoEventAction();

    public:
        void BeginOfEventAction(const G4Event*);
        void EndOfEventAction(const G4Event*);


        NeutrinoRunAction* fRunAction;

        MyDataExtract* data_extract;
    protected:
        G4EventManager* fpEventManager;
        NeutrinoPrimaryGeneratorAction generator;
        int n_event=0;
        TString name_output_file;
};

#endif
