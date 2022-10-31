#ifndef NeutrinoSteppingAction_h
#define NeutrinoSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "TTree.h"
#include "TFile.h"

class NeutrinoSteppingAction : public G4UserSteppingAction{
    public:
        NeutrinoSteppingAction();
        virtual ~NeutrinoSteppingAction();

        virtual void UserSteppingAction(const G4Step*);

    private:
        const bool m_save_step= false;
        TFile* m_file;
        TTree* m_tree;

        int evtID;
        double step_x, step_y, step_z;
        double step_t;
        TString VolumeName;
        int trackID;

};

#endif
