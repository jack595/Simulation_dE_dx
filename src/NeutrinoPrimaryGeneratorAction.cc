#include "NeutrinoPrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GeneralParticleSource.hh"
#include "../include/NeutrinoPrimaryGeneratorAction.hh"


NeutrinoPrimaryGeneratorAction::NeutrinoPrimaryGeneratorAction()
{
    generator = new G4GeneralParticleSource;
}

NeutrinoPrimaryGeneratorAction::~NeutrinoPrimaryGeneratorAction()
{
    delete generator;
}

void NeutrinoPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    generator->GeneratePrimaryVertex(anEvent);
    G4cout << "Particle energy:\t"<<generator->GetParticleEnergy() << G4endl;
    Energy_init = generator->GetParticleEnergy();
}

void NeutrinoPrimaryGeneratorAction::CreateTree() {
    G4cout << "------------>Creating TTree for generator "<<G4endl;
    tree_generator = new TTree("genInfo", "genInfo");
    tree_generator->Branch("E_init", &Energy_init, "E_init/D");
}

void  NeutrinoPrimaryGeneratorAction::FillTree()
{
    tree_generator->Fill();
}

void  NeutrinoPrimaryGeneratorAction::WriteTree()
{
    tree_generator->Write();
}

