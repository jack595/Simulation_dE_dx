#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GeneralParticleSource.hh"
#include "../include/NeutrinoPrimaryGeneratorAction.hh"
#include"Randomize.hh"
#include "G4Box.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ChargedGeantino.hh"
#include <vector>
#include "TROOT.h"
#include "G4ThreeVector.hh"
NeutrinoPrimaryGeneratorAction::NeutrinoPrimaryGeneratorAction()
{}

NeutrinoPrimaryGeneratorAction::NeutrinoPrimaryGeneratorAction(bool add_source)
{
    add_radio_source = add_source;
    m_evtID_gen = -1;
    if (add_source)
        G4cout << "========>  Executing adding source mode !!!!!! <========" << G4endl;
    if (add_radio_source)
    {
        G4int n_particle = 1;
        fParticleGun = new G4ParticleGun(n_particle);

        // default particle kinematic
        G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        G4ParticleDefinition* particle
                = particleTable->FindParticle("chargedgeantino");
        fParticleGun->SetParticleDefinition(particle);
        fParticleGun->SetParticleEnergy(0*eV);


    }
    else
        generator = new G4GeneralParticleSource;
    XYZ.resize(3);
}

NeutrinoPrimaryGeneratorAction::~NeutrinoPrimaryGeneratorAction()
{
//    if(add_radio_source)
//        delete fParticleGun;
//    else
//        delete generator;
}

void NeutrinoPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    G4ThreeVector v_XYZ;
    if (add_radio_source)
    {
        G4ParticleDefinition* particle = fParticleGun->GetParticleDefinition();
        // Set Fluorine particle gun
        if (particle == G4ChargedGeantino::ChargedGeantino()) {
            //fluorine
//            G4int Z = 9, A = 18;
            // Sodium
//            G4int Z = 11, A = 22;
//            G4double ionCharge   = 0.*eplus;
//            G4double excitEnergy = 0.*keV;

            // Bi207
            G4int Z = 83, A = 207;
            // Cs137
//            G4int Z = 55, A = 137;
            G4double ionCharge   = 0.*eplus;
            G4double excitEnergy = 0.*keV;

            G4ParticleDefinition* ion
                    = G4IonTable::GetIonTable()->GetIon(Z,A,excitEnergy);
            fParticleGun->SetParticleDefinition(ion);
            fParticleGun->SetParticleCharge(ionCharge);
        }

//        if (!f_LS)
//        {
//            G4LogicalVolume* envLV
//                    = G4LogicalVolumeStore::GetInstance()->GetVolume("GdLS_log");
//            if ( envLV ) f_LS = dynamic_cast<G4Box*>(envLV->GetSolid());
//        }

//        if ( f_LS ) {
//            GdLS_Length = 2*f_LS->GetYHalfLength();
//            GdLS_r = f_LS->GetXHalfLength();
//        }
//        else  {
//            G4ExceptionDescription msg;
//            msg << "Envelope volume of box shape not found.\n";
//            msg << "Perhaps you have changed geometry.\n";
//            msg << "The gun will be place at the center.";
//            G4Exception("B1PrimaryGeneratorAction::GeneratePrimaries()",
//                        "MyCode0002",JustWarning,msg);
//        }

        G4double costheta = 1.0 - 2.0*G4UniformRand();
        G4double sintheta = sqrt(1 - costheta*costheta);
        G4double phi = 360.0*G4UniformRand()*degree;

        fParticleGun->SetParticleMomentumDirection(G4ThreeVector(sintheta*cos(phi), sintheta*sin(phi), costheta));
//        G4cout << "GdLS:\t"<< GdLS_r << "\t"<<GdLS_Length<<G4endl;
//        G4ThreeVector gen_vertex(GdLS_r*2*(G4UniformRand()-0.5), GdLS_Length*(G4UniformRand()-0.5+2) , GdLS_r*2*(G4UniformRand()-0.5));
//        G4ThreeVector gen_vertex(0, GdLS_Length , 0);
        G4ThreeVector gen_vertex(0, -30*cm , 3.8*cm);
        G4cout<<"Vertex:\t"<<gen_vertex.getX()<<"\t"<<gen_vertex.getY()<<"\t"<<gen_vertex.getZ()<<G4endl;

        fParticleGun->SetParticlePosition(gen_vertex);
//        fParticleGun->SetParticlePosition(G4ThreeVector(3*cm, 3*cm, 3*cm));
        fParticleGun->GeneratePrimaryVertex(anEvent);
        Energy_init = fParticleGun->GetParticleEnergy();
        v_XYZ = fParticleGun->GetParticlePosition();
    }
    else{
        generator->GeneratePrimaryVertex(anEvent);
        G4cout << "Particle energy:\t"<<generator->GetParticleEnergy() << G4endl;
        Energy_init = generator->GetParticleEnergy();
        v_XYZ = generator->GetParticlePosition();
    }
    m_evtID_gen ++;
    XYZ[0]=v_XYZ.getX();XYZ[1]=v_XYZ.getY();XYZ[2]=v_XYZ.getZ();

}

void NeutrinoPrimaryGeneratorAction::CreateTree() {
    G4cout << "------------>Creating TTree for generator "<<G4endl;
    gROOT->ProcessLine("#include <vector>");
    tree_generator = new TTree("genInfo", "genInfo");
    tree_generator->Branch("E_init", &Energy_init, "E_init/D");
    tree_generator->Branch("XYZ", &XYZ);
    tree_generator->Branch("evtID", &m_evtID_gen, "evtID/I");
}

void  NeutrinoPrimaryGeneratorAction::FillTree()
{
    tree_generator->Fill();
}

void  NeutrinoPrimaryGeneratorAction::WriteTree()
{
    tree_generator->Write();
}

