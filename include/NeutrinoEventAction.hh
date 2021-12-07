#ifndef NeutrinoEventAction_h
#define NeutrinoEventAction_h 1

#include "globals.hh"
#include "G4UserEventAction.hh"
#include "NeutrinoRunAction.hh"
#include "MyDataExtract.hh"
#include "G4EventManager.hh"

class G4Event;
class NeutrinoRunAction;

class NeutrinoEventAction : public G4UserEventAction{
    public:
        NeutrinoEventAction();
        ~NeutrinoEventAction();

    public:
        void BeginOfEventAction(const G4Event*);
        void EndOfEventAction(const G4Event*);


        NeutrinoRunAction* fRunAction;

        MyDataExtract* data_extract;
    protected:
        G4EventManager* fpEventManager;
        int n_event=0;
};

#endif
