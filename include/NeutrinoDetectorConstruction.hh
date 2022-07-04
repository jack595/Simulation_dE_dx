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
        void SetThicknessOfTank(float thickness_tank) {m_thickness_tank=thickness_tank;}
        void SetAddESR(bool add_ESR) {m_add_ESR=add_ESR;}
        void SetAddCalib(bool add_calib) {m_add_distance2PE_calib = add_calib;}
        void WhetherTurnOnTank(bool turn_on_tank){use_tank=turn_on_tank;}
        void WhetherUseQuartz(bool use_quartz) {m_use_quartz=use_quartz;}
        void SetRofLS(float r_LS) {m_GdLS_r = r_LS;}

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
        G4Material* Photocathode_mat_R7600U;
        G4Material* Acrylic;
        G4Material* PVC;
        G4Material* GdLS;
        G4Material* LS;
        G4Material* Oil;
        G4Material* LAB;
        G4Material* Al;
        G4Material* Pb;
        G4Material* Quartz;
        G4Material* ESR;
        G4Material* Tyvek;
        G4Material* ESR_surface;
        
        // world
        G4VSolid*           solidWorld;
        G4LogicalVolume*    logicWorld;
        G4VPhysicalVolume*  physWorld;

        const bool box_LS = true;
        bool use_tank= true;
        const bool add_periphery_detectors=false;
        const bool add_speed_bump = false;
        const bool add_new_detector = false;
        const bool add_ESR_for_new_detector = false;
        bool m_add_ESR;
        const bool m_use_diffuse_reflection = true;

        bool m_use_quartz ;
        bool m_add_distance2PE_calib;
        const G4String v_name_PMT_calib[2] = {"1inches", "3inches"};

        float m_L_LS;
        float m_L_SpeedBump;
        float m_distance_PMT_near;
        float m_thickness_tank;
        float m_GdLS_r;



        G4bool fCheckOverlaps;
};

#endif
