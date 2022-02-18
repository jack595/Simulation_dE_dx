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

        // Set Attributes
        void SetLengthOfLS(float );
        void SetLengthOfSpeedBump( float );
        void SetDistanceOfNearPMT(float d_PMT){m_distance_PMT_near=d_PMT;}

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
        G4Material* Photocathode_mat_Ham3inch;
        G4Material* Photocathode_mat_MCP20inch;
        G4Material* Acrylic;
        G4Material* PVC;
        G4Material* GdLS;
        G4Material* LS;
        G4Material* Oil;
        G4Material* LAB;
        G4Material* Al;
        G4Material* Pb;
        
        // world
        G4VSolid*           solidWorld;
        G4LogicalVolume*    logicWorld;
        G4VPhysicalVolume*  physWorld;

        const bool box_LS = true;
        const bool use_tank= false;
        const bool add_periphery_detectors=false;
        const bool add_speed_bump = false;

        float m_L_LS;
        float m_L_SpeedBump;
        float m_distance_PMT_near;



        G4bool fCheckOverlaps;
};

#endif
