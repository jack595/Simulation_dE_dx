#ifndef NeutrinoPrimaryGeneratorAction_h
#define NeutrinoPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "TTree.h"

class G4GeneralParticleSource;
class G4ParticleGun;
class G4Event;

class NeutrinoPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction{
    public:
        NeutrinoPrimaryGeneratorAction();
        ~NeutrinoPrimaryGeneratorAction() override;

    public:
        void GeneratePrimaries(G4Event* anEvent);
        void CreateTree();
        void FillTree();
        void WriteTree();

    private:
        G4GeneralParticleSource* generator;
        static TTree* tree_generator;
        static double Energy_init;
};

#endif
