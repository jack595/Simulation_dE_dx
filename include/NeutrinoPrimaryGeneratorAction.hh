#ifndef NeutrinoPrimaryGeneratorAction_h
#define NeutrinoPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "TTree.h"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"
#include <vector>

class G4GeneralParticleSource;
class G4ParticleGun;
class G4Event;

class NeutrinoPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction{
    public:
        NeutrinoPrimaryGeneratorAction();
        NeutrinoPrimaryGeneratorAction(bool);
        ~NeutrinoPrimaryGeneratorAction() override;

    public:
        void GeneratePrimaries(G4Event* anEvent) override;
        void CreateTree();
        void FillTree();
        void WriteTree();

    private:
        G4GeneralParticleSource* generator;
        G4ParticleGun* fParticleGun;
        static TTree* tree_generator;
        static double Energy_init;
        static std::vector<double> XYZ;

        bool add_radio_source;

        G4Box* f_LS;
        G4double GdLS_r;
        G4double GdLS_Length;

};

#endif
