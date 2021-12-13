#ifndef NeutrinoDetectorConstruction_H
#define NeutrinoDetectorConstruction_h 1

#include "globals.hh"
#include "G4VSolid.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VUserDetectorConstruction.hh"
#include "MyDataExtract.hh"
#include "G4EventManager.hh"

class NeutrinoDetectorConstruction : public G4VUserDetectorConstruction
{
    public:
        NeutrinoDetectorConstruction();
        ~NeutrinoDetectorConstruction();
        
        // construct materials
        void ConstructMaterials();

        // construct the detector
        G4VPhysicalVolume* Construct();

        // construct world
        void ConstructWorld();

        // construct detector
        void ConstructDetector();

        // construct SD volume
        virtual void ConstructSDandField();

    MyDataExtract* data_extract;

    protected:
        G4EventManager* fpEventManager;
        int n_event=0;

    private:
        // materials
        G4Material* water;
        G4Material* air;
        G4Material* pyrex;
        G4Material* steel;
        G4Material* vacuum;
        G4Material* vacuum_pure;
        G4Material* Photocathode_mat_Ham20inch;
        G4Material* Photocathode_mat_MCP20inch;
        G4Material* Acrylic;
        G4Material* GdLS;
        G4Material* LS;
        G4Material* Oil;
        G4Material* LAB;
        
        // world
        G4VSolid*           solidWorld;
        G4LogicalVolume*    logicWorld;
        G4VPhysicalVolume*  physWorld;

        const bool box_LS = true;
        const bool use_tank=true;
        const bool add_periphery_detectors=false;


        G4bool fCheckOverlaps;
};

#endif
