#include "../include/NeutrinoDetectorConstruction.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4Box.hh"
#include "G4Element.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "OpticalProperty.icc"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "TMath.h"
#include "G4VisAttributes.hh"

#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4NistManager.hh"
#include "G4Isotope.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4SDManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4PVReplica.hh"
#include "../include/MyTrackerSD.hh"
#include "../include/MyCounterSD.hh"


NeutrinoDetectorConstruction::NeutrinoDetectorConstruction() :
    G4VUserDetectorConstruction(),
    solidWorld(NULL), logicWorld(NULL), physWorld(NULL),
    fCheckOverlaps(false)
{
}

NeutrinoDetectorConstruction::~NeutrinoDetectorConstruction()
{
}

void NeutrinoDetectorConstruction::SetLengthOfLS(float L_LS)
{
    m_L_LS = L_LS;
}

void NeutrinoDetectorConstruction::SetLengthOfSpeedBump(float L_SpeedBump)
{
    m_L_SpeedBump = L_SpeedBump;
}

G4VPhysicalVolume* NeutrinoDetectorConstruction::Construct()
{
    // initialize materials
    ConstructMaterials();

    // world volume
    ConstructWorld();

    // detector
    ConstructDetector();

    // return physical world
    return physWorld;
}

void NeutrinoDetectorConstruction::ConstructWorld()
{
    G4double world_x = 4.*m, world_y = 4.*m, world_z = 4.*m;
    
    solidWorld = new G4Box("solidWorld", 
                           0.5*world_x, 
                           0.5*world_y, 
                           0.5*world_z);
    
    logicWorld = new G4LogicalVolume(solidWorld,
                                     vacuum_pure,
                                     "logicWorld");
    
    physWorld  = new G4PVPlacement(0,
                                   G4ThreeVector(),
                                   logicWorld,
                                   "physWorld",
                                   0,
                                   false,
                                   0,
                                   0);
}

void NeutrinoDetectorConstruction::ConstructDetector()
{
    //------------------- experimental hall ------------------
    G4double expHallLength_x = 6 * m, expHallLength_y = 6 * m, expHallLength_z = 6 * m;
    auto expHall = new G4Box("expHall", 0.5 * expHallLength_x, 0.5 * expHallLength_y, 0.5 * expHallLength_z);
    auto expHall_log = new G4LogicalVolume(expHall, vacuum_pure, "expHall", 0, 0, 0);

    G4VPhysicalVolume *expHall_phys = new G4PVPlacement(0,            // no rotation
                                                        G4ThreeVector(0, 0, 0),    // at (x,y,z)
                                                        expHall_log,   // its logical volume
                                                        "expHall_phys",    // its name
//                                                            rock_log,      // its mother  volume
                                                        logicWorld,       // its mother  volume
                                                        false,           // no boolean operations
                                                        0);              // copy number



    //-------------------- Chamber ------------------------
    G4double ChamberLength_x = 220. * cm, ChamberLength_y = 220. * cm, ChamberLength_z = 50. * cm;
    auto *Chamber = new G4Box("Chamber", 0.5 * ChamberLength_x, 0.5 * ChamberLength_y, 0.5 * ChamberLength_z);
    auto *Chamber_log = new G4LogicalVolume(Chamber, vacuum, "Chamber_log", 0, 0, 0);
    G4ThreeVector Chamberpos(0. * mm, 0. * mm, 0. * mm);
    G4RotationMatrix Chamberrot;
    Chamberrot.rotateY(90. * deg);
    G4Transform3D Chambertrans(Chamberrot, Chamberpos);
    G4VPhysicalVolume *Chamber_phys = new G4PVPlacement(
            Chambertrans,
            Chamber_log,   // its logical volume
            "Chamber_phys",    // its name
            expHall_log,      // its mother  volume
            false,           // no boolean operations
            0);              // copy number

    // Set PMT and Si Oil
    G4double R_PMT = 38.1 * mm; // 3 inches
    G4double R_PMT_2inches = 25.4 * mm; // 2 inches
    G4double R_PMT_1inches = 12.7 * mm; // 1 inches
    G4double R_SIMP = 0.5642 * cm; // 1 inches
    G4double PMT_thickness = 1 * cm;
    G4double PMT_calib_thickness = 1*mm;
    G4double distance_calib_PMT_to_LS = 1 * cm;

    G4double PMT_thickness_base = 0.8 * cm;
    G4double PMT_thickness_R7600U = 0.1 * cm;
    G4double PMT_thickness_base_R7600U = 0.08 * cm;
    G4double distance_far_PMT_to_LS = 50 * cm;


    G4double l_PMT_R7600 = 36*mm;

    G4double Al_thickness = 0.1 * mm;
    G4double Acrylic_thickness = m_thickness_tank * mm;
    G4double GdLS_Length = m_L_LS * mm;
    G4double GdLS_r = m_GdLS_r * cm;
    G4double PS_r =  m_GdLS_r * cm;
    G4double PS_thickness = m_thickness_PS * mm ;
    G4double PS_y = -20*cm;
    G4double Acrylic_r = GdLS_r + Acrylic_thickness;
    G4double Acrylic_Length = GdLS_Length + 2 * Acrylic_thickness;

    G4double SiOil_r = Acrylic_r;

    // Set ESR film
    G4double ESR_thickness = 0.0508 * mm;
//    G4double ESR_thickness = 3 * mm;

    G4double ESR_R;
    if (use_tank)
        ESR_R = Acrylic_r;
    else
        ESR_R = GdLS_r;

    // ESR for Plastic Sintillator


    G4double distance_PMT_near = m_distance_PMT_near * cm;

    G4LogicalVolume* mother_log_of_LS = NULL;
    G4VPhysicalVolume *Tank_phys;
    G4LogicalVolume *Tank_log;
    if (use_tank)
    {
        //-------------------- Acrylic tank  ----------------------------------------
        G4CSGSolid* Tank ;
        if (box_LS)
            Tank = new G4Box("Tank",
                             Acrylic_r,
                             0.5 * Acrylic_Length,
                             Acrylic_r
                             );
        else
            Tank = new G4Tubs("Tank",
                                  0. * mm,
                                  Acrylic_r,
                                  0.5 * Acrylic_Length,
                                  0. * deg,
                                  360. * deg);
        if(m_use_quartz){
            Tank_log = new G4LogicalVolume(Tank,
                                       Quartz,
                                       "Tank_log",
                                       0,
                                       0,
                                       0);
        }
        else
        {
            if (m_use_OpaqueAcrylic)
            {
                Tank_log = new G4LogicalVolume(Tank,
                                               OpaqueAcrylic,
                                               "Tank_log",
                                               0,
                                               0,
                                               0);
            }
            else
            {
                Tank_log = new G4LogicalVolume(Tank,
                                               Acrylic,
                                               "Tank_log",
                                               0,
                                               0,
                                               0);
            }
        }
        Tank_phys = new G4PVPlacement(0,
                                      G4ThreeVector(0, 0, 0),    // at (x,y,z)
                                      Tank_log,   // its logical volume
                                      "Tank_phys",    // its name
                                      Chamber_log,      // its mother  volume
                                      false,           // no boolean operations
                                      0);
        G4VisAttributes *Tank_visatt = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)); //green
        Tank_visatt->SetForceWireframe(true);
        Tank_visatt->SetForceAuxEdgeVisible(true);
        Tank_log->SetVisAttributes(Tank_visatt);
        mother_log_of_LS = Tank_log;
    }
    else
        mother_log_of_LS = Chamber_log;


//-------------------- GdLS ------------------------
    G4CSGSolid* GdLS_solid ;
    if (box_LS)
        GdLS_solid = new G4Box("GdLS_solid",
                                GdLS_r,
                                0.5 * GdLS_Length,
                                GdLS_r);
    else
        GdLS_solid = new G4Tubs("GdLS_solid",
                                        0. * mm,
                                        GdLS_r,
                                        0.5 * GdLS_Length,
                                        0. * deg,
                                        360. * deg);


    G4LogicalVolume *GdLS_log = new G4LogicalVolume(GdLS_solid,
                                                    LS,
                                                    "GdLS_log",
                                                    0,
                                                    0,
                                                    0);


    G4VPhysicalVolume *GdLS_phys = new G4PVPlacement(0,
                                                     G4ThreeVector(0, 0, 0),    // at (x,y,z)
                                                     GdLS_log,   // its logical volume
                                                     "GdLS_phys",    // its name
                                                     mother_log_of_LS,      // its mother  volume
                                                     false,           // no boolean operations
                                                     0);
    G4VisAttributes *GdLS_visatt = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));//blue
    GdLS_visatt->SetForceWireframe(true);
    GdLS_visatt->SetForceAuxEdgeVisible(true);
    GdLS_log->SetVisAttributes(GdLS_visatt);

    G4VPhysicalVolume *ESR_phys_upper;
    G4VPhysicalVolume *ESR_phys_bottom;
    if (m_add_ESR)
    {
        auto* ESR_solid = new G4Box("ESR_solid",
                               ESR_R,
                               0.5 * ESR_thickness,
                               ESR_R);
        auto* ESR_log = new G4LogicalVolume(ESR_solid,
                                                        Acrylic,
                                                        "ESR_log",
                                                        0,
                                                        0,
                                                        0);

        G4double ESR_y;
        if (use_tank)
            ESR_y = (Acrylic_Length +ESR_thickness)*0.5;
        else
            ESR_y = (GdLS_Length + ESR_thickness)*0.5;

        ESR_phys_upper = new G4PVPlacement(0,
                                           G4ThreeVector(0, ESR_y, 0),    // at (x,y,z)
                                           ESR_log,   // its logical volume
                                           "ESR_phys_upper",    // its name
                                           Chamber_log,      // its mother  volume
                                           false,           // no boolean operations
                                           0);
        ESR_phys_bottom = new G4PVPlacement(0,
                                            G4ThreeVector(0, -ESR_y, 0),    // at (x,y,z)
                                            ESR_log,   // its logical volume
                                            "ESR_phys_bottom",    // its name
                                            Chamber_log,      // its mother  volume
                                            false,           // no boolean operations
                                            1);
    }

    G4VPhysicalVolume *ESR_phys_left_new_detector;
    G4VPhysicalVolume *ESR_phys_right_new_detector;
    if (add_new_detector)
    {
        double r_aperture = 2 * mm;
        double r_ESR_new_detector;
        double z_ESR_new_detector;
        double length_ESR_new_detector;

        if (add_ESR_for_new_detector)
        {

            G4double x_ESR_new_detector;
            if (use_tank)
            {
                x_ESR_new_detector = Acrylic_r + ESR_thickness*0.5;
                r_ESR_new_detector = (Acrylic_r-r_aperture)/2.;
                z_ESR_new_detector = (Acrylic_r+r_aperture)/2.;
                length_ESR_new_detector = Acrylic_Length;
            }
            else
            {
                x_ESR_new_detector =  GdLS_r + (ESR_thickness)*0.5;
                r_ESR_new_detector =  (GdLS_r-r_aperture)/2.;
                z_ESR_new_detector = (GdLS_r+r_aperture)/2.;
                length_ESR_new_detector = GdLS_Length;
            }

            G4cout << r_ESR_new_detector << "\t"<< r_aperture<< G4endl;
            auto* ESR_solid_new_detector = new G4Box("ESR_solid_new_detector",
                                                     0.5*ESR_thickness,
                                                     0.5*length_ESR_new_detector,
                                                     r_ESR_new_detector);
            auto* ESR_log_new_detector = new G4LogicalVolume(ESR_solid_new_detector,
                                                             Acrylic,
                                                             "ESR_log",
                                                             0,
                                                             0,
                                                             0);

            ESR_phys_left_new_detector = new G4PVPlacement(0,
                                               G4ThreeVector( x_ESR_new_detector,0, z_ESR_new_detector),    // at (x,y,z)
                                               ESR_log_new_detector,   // its logical volume
                                               "ESR_phys_upper",    // its name
                                               Chamber_log,      // its mother  volume
                                               false,           // no boolean operations
                                               0);
            ESR_phys_right_new_detector = new G4PVPlacement(0,
                                                G4ThreeVector(x_ESR_new_detector,0,-z_ESR_new_detector ),    // at (x,y,z)
                                                ESR_log_new_detector,   // its logical volume
                                                "ESR_phys_bottom",    // its name
                                                Chamber_log,      // its mother  volume
                                                false,           // no boolean operations
                                                1);

        }

        double R_Si_detector = 2.5 * mm;
        double thickness_Si_detector = 5 * mm;
        double distance_Si_detector = 5 * cm;
        double x_Si_detector;
        if (use_tank)
            x_Si_detector = Acrylic_r+distance_Si_detector;
        else
            x_Si_detector = GdLS_r + distance_Si_detector;

        auto solid_Si_detector = new G4Box("solid_Si_detector",
                                            0.5*thickness_Si_detector,
                                            R_Si_detector,
                                            R_Si_detector);

        auto log_Si_detector = new G4LogicalVolume(solid_Si_detector,
                                                        Acrylic,
                                                        "log_Si_detector");
        auto phys_Si_detector = new G4PVPlacement(0,
                                                        G4ThreeVector(x_Si_detector,0,0 ),    // at (x,y,z)
                                                        log_Si_detector,   // its logical volume
                                                        "phys_Si_detector",    // its name
                                                        Chamber_log,      // its mother  volume
                                                        false,           // no boolean operations
                                                        0);


    }
    // ---------------------- For Opaque Acrylic, Transparent window--------------------
    G4VPhysicalVolume *Acrylic_phys_bottom;
    if (m_use_OpaqueAcrylic)
    {
        auto* Acrylic_solid = new G4Box("Acrylic_win_solid",
                                         ESR_R,
                                         0.5 * Acrylic_thickness,
                                         ESR_R);
        auto* Acrylic_log = new G4LogicalVolume(Acrylic_solid,
                                                Acrylic,
                                                 "Acrylic_win_log",
                                                 0,
                                                 0,
                                                 0);

        G4double ESR_y = (Acrylic_thickness+ GdLS_Length)*0.5;

        Acrylic_phys_bottom = new G4PVPlacement(0,
                                                G4ThreeVector(0, -ESR_y, 0),    // at (x,y,z)
                                               Acrylic_log,   // its logical volume
                                                "Acrylic_win_phys_bottom",    // its name
                                                Tank_log,      // its mother  volume
                                                false,           // no boolean operations
                                                0);


    }



    // ------------------- Add Absorption Surface -----------------------
    G4VPhysicalVolume *Absorber_phys_upper;
    if ( add_UpperAbsorptionSurface )
    {
        auto* Absorber_solid = new G4Box("Absorber_solid",
                                        ESR_R,
                                        0.5 * ESR_thickness,
                                        ESR_R);
        auto* Absorber_log = new G4LogicalVolume(Absorber_solid,
                                                vacuum_pure,
                                                "Absorber_log",
                                                0,
                                                0,
                                                0);

        G4double ESR_y;
        if (use_tank)
            ESR_y = (Acrylic_Length +ESR_thickness)*0.5;
        else
            ESR_y = (GdLS_Length + ESR_thickness)*0.5;

        Absorber_phys_upper = new G4PVPlacement(0,
                                           G4ThreeVector(0, ESR_y, 0),    // at (x,y,z)
                                           Absorber_log,   // its logical volume
                                           "Absorber_phys_upper",    // its name
                                           Chamber_log,      // its mother  volume
                                           false,           // no boolean operations
                                           0);


    }
    if ( add_NarrowSideAbsorber)
    {

            G4double x_NarrowAbsorber;
            G4double z_NarrowAbsorber;
            G4double length_NarrowAbsorber;
            G4double r_NarrowAbsorber;
            if (use_tank)
            {
                x_NarrowAbsorber = Acrylic_r + ESR_thickness*0.5;
                z_NarrowAbsorber = Acrylic_r + ESR_thickness*0.5;
                length_NarrowAbsorber = Acrylic_Length;
                r_NarrowAbsorber = Acrylic_r;
            }
            else
            {
                x_NarrowAbsorber =  GdLS_r + ESR_thickness*0.5;
                z_NarrowAbsorber =  GdLS_r + ESR_thickness*0.5;
                length_NarrowAbsorber = GdLS_Length;
                r_NarrowAbsorber = GdLS_r;
            }

            auto* ESR_solid_new_detector = new G4Box("Absorber_solid",
                                                     0.5*ESR_thickness,
                                                     0.5*length_NarrowAbsorber,
                                                     r_NarrowAbsorber);
            auto* ESR_log_new_detector = new G4LogicalVolume(ESR_solid_new_detector,
                                                             vacuum_pure,
                                                             "Absorber_log",
                                                             0,
                                                             0,
                                                             0);
            std::vector<float> v_sign{-1., 1.};
            auto* Absorber_rotate = new G4RotationMatrix();
            Absorber_rotate->rotateY(90*deg);
            for (auto sign:v_sign)
            {
                ESR_phys_left_new_detector = new G4PVPlacement(0,
                                                               G4ThreeVector( sign*x_NarrowAbsorber,0, 0),    // at (x,y,z)
                                                               ESR_log_new_detector,   // its logical volume
                                                               "ESR_phys_xSide",    // its name
                                                               Chamber_log,      // its mother  volume
                                                               false,           // no boolean operations
                                                               (int)sign+1);

                ESR_phys_right_new_detector = new G4PVPlacement(Absorber_rotate,
                                                               G4ThreeVector( 0,0, sign*z_NarrowAbsorber),    // at (x,y,z)
                                                               ESR_log_new_detector,   // its logical volume
                                                               "ESR_phys_zSide",    // its name
                                                               Chamber_log,      // its mother  volume
                                                               false,           // no boolean operations
                                                                (int)sign+1);
            }
//            ESR_phys_right_new_detector = new G4PVPlacement(0,
//                                                            G4ThreeVector(x_NarrowAbsorber,0,-z_ESR_new_detector ),    // at (x,y,z)
//                                                            ESR_log_new_detector,   // its logical volume
//                                                            "ESR_phys_bottom",    // its name
//                                                            Chamber_log,      // its mother  volume
//                                                            false,           // no boolean operations
//                                                            1);

    }



    //-------------------- Speed Bump---------------------------
    if (add_speed_bump)
    {
        G4double SpeedBumpLength_x = 10.*cm , SpeedBumpLength_y = m_L_SpeedBump*cm, SpeedBumpLength_z = 10.*cm;
        G4double distance_SpeedBump_to_Chamber = 1.*cm;
        auto * SpeedBump = new G4Box("SpeedBump", 0.5*SpeedBumpLength_x, 0.5*SpeedBumpLength_y, 0.5*SpeedBumpLength_z);
        auto * SpeedBump_log = new G4LogicalVolume(SpeedBump, Al, "SpeedBump_log", 0, 0, 0 );
        G4ThreeVector SpeedBumpPos(0.*mm,  0.5 *(ChamberLength_y + 2 * distance_SpeedBump_to_Chamber+SpeedBumpLength_y), 0.*mm);
        auto * SpeedBump_phys = new G4PVPlacement(0,
                                                  SpeedBumpPos,
                                                  SpeedBump_log,
                                                  "SpeedBump_phys",
                                                  expHall_log,
                                                  false,
                                                  0);
    }


//-------------------- SiOil ------------------------
//    G4CSGSolid* SiOil;
//    if (box_LS)
//        SiOil = new G4Box("SiOil",
//                           SiOil_r,
//                           SiOil_r,
//                           0.5*distance_PMT_near);
//    else
//        SiOil = new G4Tubs("SiOil",
//                       0. * mm,
//                       SiOil_r,
//                       0.5 * distance_PMT_near,
//                       0. * deg,
//                       360. * deg);
//
//
//    G4LogicalVolume *SiOil_log = new G4LogicalVolume(SiOil,
//                                                     Oil,
//                                                     "SiOil_log",
//                                                     0,
//                                                     0,
//                                                     0);


    // --------------- PMT ---------------------------------
//    auto *PMT = new G4Tubs("PMT",
//                             0. * mm,
//                             R_PMT,
//                             0.5 * PMT_thickness,
//                             0. * deg,
//                             360. * deg);
//
//
//    auto* PMT_base = new G4Tubs("PMT_base",
//                                  0.*mm,
//                                  R_PMT,
//                                  0.5*PMT_thickness_base,
//                                  0.*deg,
//                                  360.*deg);

    auto *PMT = new G4Box("PMT",
                          R_SIMP,
                          R_SIMP,
                          0.5*PMT_thickness);
    auto *PMT_base = new G4Box("PMT_base",
                               R_SIMP,
                               R_SIMP,
                               0.5*PMT_thickness_base);

    auto *PMT_log = new G4LogicalVolume(PMT,
                                        pyrex,
                                        "PMT_log",
                                        0,
                                        0,
                                        0);

    auto *PMT_base_log = new G4LogicalVolume(PMT_base,
                                        pyrex,
                                        "PMT_base_log",
                                        0,
                                        0,
                                        0);

    auto* PMT_phys = new G4PVPlacement(0,
                     G4ThreeVector(0, 0, -0.5*(PMT_thickness-PMT_thickness_base)),
                     PMT_base_log,
                     "PMT_phys",
                     PMT_log,
                     false,
                     0);

    G4double theta_near = 45*deg;
    auto* PMT_rotate = new G4RotationMatrix();
//    G4double R_near = 0.5 *(Acrylic_r*2 +2 * distance_PMT_near+ PMT_thickness);
    PMT_rotate->rotateX(theta_near);
    auto* PMT_phys_near = new G4PVPlacement(PMT_rotate,
                    G4ThreeVector(0, -distance_PMT_near*TMath::Sin(theta_near),
                                  -distance_PMT_near*TMath::Cos(theta_near)),    // at (x,y,z)
                    PMT_log,   // its logical volume
                    "PMT_phys_near",    // its name
                    Chamber_log,      // its mother  volume
                    false,           // no boolean operations
                    0);

    auto* PMT_rotate2 = new G4RotationMatrix();
    PMT_rotate2->rotateY(180*deg).rotateX(theta_near);
    auto* PMT_phys_near2 = new G4PVPlacement(PMT_rotate2,
                      G4ThreeVector(0, -distance_PMT_near*TMath::Sin(theta_near),  distance_PMT_near*TMath::Cos(theta_near)),    // at (x,y,z)
                      PMT_log,   // its logical volume
                      "PMT_phys_near2",    // its name
                      Chamber_log,      // its mother  volume
                      false,           // no boolean operations
                      1);

//    auto* PMT_rotate_far = new G4RotationMatrix();
//    PMT_rotate_far->rotateY(-90*deg);
//    auto* PMT_phys_far = new G4PVPlacement(PMT_rotate_far,
//                                           G4ThreeVector(-0.5 *(Acrylic_r*2 + 2 * distance_far_PMT_to_LS +PMT_thickness),
//                                                                 0,
//                                                                 0),    // at (x,y,z)
//                                           PMT_log,   // its logical volume
//                                           "PMT_phys_far",    // its name
//                                           Chamber_log,      // its mother  volume
//                                           false,           // no boolean operations
//                                           2);


//     -------------- R7600U PMT ---------------------
    // Define Surface
    auto* PMTRefOpSurface2 = new G4OpticalSurface("PMTRefOpSurface2");
    PMTRefOpSurface2->SetType(dielectric_metal);
    PMTRefOpSurface2->SetMaterialPropertiesTable(Photocathode_mat_R7600U->GetMaterialPropertiesTable());

    // Define Geometry
    auto *PMT_R7600 = new G4Box("PMT_R7600",
                           0.5* l_PMT_R7600,
                           0.5* l_PMT_R7600,
                           0.5* PMT_thickness_R7600U
                           );

    auto* PMT_base_R7600 = new G4Box("PMT_base_R7600",
                                     0.5* l_PMT_R7600,
                                     0.5* l_PMT_R7600,
                                     0.5* PMT_thickness_base_R7600U
                                     );

    auto *PMT_log_R7600 = new G4LogicalVolume(PMT_R7600,
                                            pyrex,
                                            "PMT_log_R7600",
                                            0,
                                            0,
                                            0);

    auto *PMT_base_log_R7600 = new G4LogicalVolume(PMT_base_R7600,
                                                   pyrex,
                                                   "PMT_base_log_R7600",
                                                   0,
                                                   0,
                                                   0);

    auto* PMT_phys_R7600 = new G4PVPlacement(0,
                                       G4ThreeVector(0, 0, -0.5*(PMT_thickness_R7600U-PMT_thickness_base_R7600U)),
                                       PMT_base_log_R7600,
                                       "PMT_phys_R7600",
                                       PMT_log_R7600,
                                       false,
                                       0);
    int n_PMT_far = 12;
    G4double deltaTheta_PMT = 180./n_PMT_far *deg;

    for (int j=0;j<n_PMT_far+1;j++)
    {
        double R = -0.5 *(Acrylic_r*2 + 2 * distance_far_PMT_to_LS +
                         PMT_thickness_R7600U);
        auto* PMT_rotate_far = new G4RotationMatrix();
        PMT_rotate_far->rotateY(-90*deg).rotateX(deltaTheta_PMT*j);
        auto* PMT_phys_R7600_far = new G4PVPlacement(PMT_rotate_far,
                                            G4ThreeVector(R*TMath::Cos(deltaTheta_PMT*j),
                                                          R*TMath::Sin(deltaTheta_PMT*j),
                                                          0),    // at (x,y,z)
                                            PMT_log_R7600,   // its logical volume
                                            "PMT_phys_R7600_far",    // its name
                                            Chamber_log,      // its mother  volume
                                            false,           // no boolean operations
                                            j);
        new G4LogicalBorderSurface("PMTOpSurface2", PMT_phys_R7600_far, PMT_phys_R7600, PMTRefOpSurface2);
        new G4LogicalBorderSurface("PMTOpSurface2", PMT_phys_R7600, PMT_phys_R7600_far, PMTRefOpSurface2);
    }
    G4VisAttributes *PMT_visatt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));//red
    PMT_visatt->SetForceWireframe(true);
    PMT_visatt->SetForceAuxEdgeVisible(true);
    PMT_log->SetVisAttributes(PMT_visatt);
    PMT_log_R7600->SetVisAttributes(PMT_visatt);

    if (m_add_distance2PE_calib)
    {
        // only use one direction to place calib volume
        G4double v_R_PMT[3] = { R_SIMP, R_PMT_1inches, R_PMT};
        for (int j=0;j<3;j++) {
            const G4String name_PMT = "PMT_"+v_name_PMT_calib[j]+"_calib_distance2PE";
            auto *PMT_3inches_calib_distance2PE = new G4Tubs(name_PMT,
                                                             0. * mm,
                                                             v_R_PMT[j],
                                                             0.5 * PMT_calib_thickness,
                                                             0. * deg,
                                                             360. * deg);
            auto *PMT_3inches_calib_distance2PE_log = new G4LogicalVolume(PMT_3inches_calib_distance2PE,
                                                                          vacuum,
                                                                          name_PMT+"_log",
                                                                          0,
                                                                          0,
                                                                          0);
            auto *PMT_rotate_calib = new G4RotationMatrix();
            G4double theta_rotate = -45.*deg;

            PMT_rotate_calib->rotateY(-90 * deg).rotateX(theta_rotate);
            double scaled_distance;
            int n_calib_near = 20;
            for (int i = 1; i < n_calib_near + 3; i++) {
                if (i <= n_calib_near)
                    scaled_distance = distance_calib_PMT_to_LS * i;
                else
                    scaled_distance = (i - n_calib_near) * distance_calib_PMT_to_LS * 10 +
                                        (distance_calib_PMT_to_LS * n_calib_near);
                double distant_PMT_calib = 0.5 * (Acrylic_r * 2 +
                                                  2 * scaled_distance +
                                                  PMT_calib_thickness)+j*PMT_calib_thickness;
                auto *PMT_3inches_calib_distance2PE_phys = new G4PVPlacement(PMT_rotate_calib,
                                                                             G4ThreeVector(distant_PMT_calib*TMath::Cos(theta_rotate),
                                                                                           distant_PMT_calib*TMath::Sin(theta_rotate), 0),    // at (x,y,z)
                                                                             PMT_3inches_calib_distance2PE_log,   // its logical volume
                                                                             name_PMT+"_phys",    // its name
                                                                             Chamber_log,      // its mother  volume
                                                                             false,           // no boolean operations
                                                                             i);

            }
        }

    }

    // Plastic Scintillator
    G4CSGSolid* PS_solid ;
    PS_solid = new G4Box("PS_solid",
                           PS_r,
                           0.5 * PS_thickness,
                           PS_r);

    G4LogicalVolume *PS_log;
    if (m_turn_PS_into_LS)
        PS_log = new G4LogicalVolume(PS_solid,
                                     LS,
                                "PS_log",
                                0,
                                0,
                                0);
    else
        PS_log = new G4LogicalVolume(PS_solid,
                                     PS,
                                     "PS_log",
                                     0,
                                     0,
                                     0);



    G4VPhysicalVolume *PS_phys = new G4PVPlacement(0,
                                                     G4ThreeVector(0, PS_y, 0),    // at (x,y,z)
                                                     PS_log,   // its logical volume
                                                     "PS_phys",    // its name
                                                     Chamber_log,      // its mother  volume
                                                     false,           // no boolean operations
                                                     0);
    G4VisAttributes *PS_visatt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));//blue
    PS_visatt->SetForceWireframe(true);
    PS_visatt->SetForceAuxEdgeVisible(true);
    PS_log->SetVisAttributes(PS_visatt);

    // ESR for Plastic Scintillator
    auto* ESR_solid_PS = new G4Box("ESR_solid_PS",
                                PS_r,
                                0.5 * ESR_thickness,
                                PS_r);
    auto* ESR_log_PS = new G4LogicalVolume(ESR_solid_PS,
                                        Acrylic,
                                          "ESR_log",
                                        0,
                                        0,
                                        0);

    G4double ESR_y_PS = (PS_thickness + ESR_thickness)*0.5;

    auto* ESR_phys_upper_PS = new G4PVPlacement(0,
                                       G4ThreeVector(0, PS_y+ESR_y_PS, 0),    // at (x,y,z)
                                       ESR_log_PS,   // its logical volume
                                       "ESR_phys_upper_PS",    // its name
                                       Chamber_log,      // its mother  volume
                                       false,           // no boolean operations
                                       0);
    auto* ESR_phys_bottom_PS = new G4PVPlacement(0,
                                        G4ThreeVector(0, PS_y-ESR_y_PS, 0),    // at (x,y,z)
                                        ESR_log_PS,   // its logical volume
                                        "ESR_phys_bottom_PS",    // its name
                                        Chamber_log,      // its mother  volume
                                        false,           // no boolean operations
                                        1);


    auto* ESR_solid_narrow_PS = new G4Box("ESR_solid_narrow",
                                             0.5*ESR_thickness,
                                             0.5*PS_thickness,
                                             PS_r);
    auto* ESR_log_narrow_PS = new G4LogicalVolume(ESR_solid_narrow_PS,
                                                     Acrylic,
                                                     "ESR_log_narrow",
                                                     0,
                                                     0,
                                                     0);

    G4double x_NarrowAbsorber_PS =  PS_r + ESR_thickness*0.5;
    G4double z_NarrowAbsorber_PS =  PS_r + PMT_thickness*0.5;
    G4double length_NarrowAbsorber_PS = ESR_thickness;
    G4double r_NarrowAbsorber_PS = PS_r;

    std::vector<float> v_sign{-1., 1.};
    auto* ESR_PS_rotate = new G4RotationMatrix();
    ESR_PS_rotate->rotateY(90*deg);
    G4PVPlacement* ESR_phys_left_PS;
    G4PVPlacement* ESR_phys_right_PS;

    ESR_phys_left_PS = new G4PVPlacement(0,
                                         G4ThreeVector( x_NarrowAbsorber_PS,PS_y, 0),    // at (x,y,z)
                                         ESR_log_narrow_PS,   // its logical volume
                                         "ESR_phys_xSide_PS_left",    // its name
                                         Chamber_log,      // its mother  volume
                                         false,           // no boolean operations
                                         0);
    ESR_phys_right_PS = new G4PVPlacement(0,
                                         G4ThreeVector( -1*x_NarrowAbsorber_PS,PS_y, 0),    // at (x,y,z)
                                         ESR_log_narrow_PS,   // its logical volume
                                         "ESR_phys_xSide_PS_right",    // its name
                                         Chamber_log,      // its mother  volume
                                         false,           // no boolean operations
                                         0);

    // PMT for Plastic Scintillator
    auto *PMT_PS = new G4Box("PMT_PS",
                             PS_r,
                             0.5*PS_thickness,
                             0.5*PMT_thickness);
    auto *PMT_base_PS = new G4Box("PMT_base_PS",
                                  PS_r,
                                  0.5*PS_thickness,
                                  0.5*PMT_thickness_base);

    auto *PMT_log_PS = new G4LogicalVolume(PMT_PS,
                                        pyrex,
                                        "PMT_log_PS",
                                        0,
                                        0,
                                        0);

    auto *PMT_base_log_PS = new G4LogicalVolume(PMT_base_PS,
                                             pyrex,
                                             "PMT_base_log_PS",
                                             0,
                                             0,
                                             0);

    auto* PMT_phys_PS = new G4PVPlacement(0,
                                       G4ThreeVector(0, 0, -0.5*(PMT_thickness-PMT_thickness_base)),
                                       PMT_base_log_PS,
                                       "PMT_phys_PS",
                                       PMT_log_PS,
                                       false,
                                       0);

    auto* PMT_PS_phys_1 = new G4PVPlacement(0,
                                            G4ThreeVector(0, PS_y, -z_NarrowAbsorber_PS),// at (x,y,z)
                                            PMT_log_PS,   // its logical volume
                                            "PMT_phys_near_PS",    // its name
                                            Chamber_log,      // its mother  volume
                                            false,           // no boolean operations
                                            0);

    G4double theta_near_PS = 180*deg;
    auto* PMT_rotate_PS = new G4RotationMatrix();
    PMT_rotate_PS->rotateX(theta_near_PS);
    auto* PMT_PS_phys_2 = new G4PVPlacement(PMT_rotate_PS,
                                            G4ThreeVector(0, PS_y, z_NarrowAbsorber_PS),// at (x,y,z)
                                            PMT_log_PS,   // its logical volume
                                    "PMT_phys_near_PS",    // its name
                                Chamber_log,      // its mother  volume
                                    false,           // no boolean operations
                                    1);



//------------------------------ SkinSurface -----------------------------------
//        new G4LogicalSkinSurface("RefOpSurface", Alfilm_log, MirrorSurface);
    auto* PMTRefOpSurface1 = new G4OpticalSurface("PMTRefOpSurface");
    PMTRefOpSurface1->SetType(dielectric_metal);
    PMTRefOpSurface1->SetMaterialPropertiesTable(Photocathode_mat_Ham3inch->GetMaterialPropertiesTable());
    new G4LogicalBorderSurface("PMTOpSurface", PMT_phys_near, PMT_phys, PMTRefOpSurface1);
    new G4LogicalBorderSurface("PMTOpSurface", PMT_phys, PMT_phys_near, PMTRefOpSurface1);
    new G4LogicalBorderSurface("PMTOpSurface", PMT_phys_near2, PMT_phys, PMTRefOpSurface1);
    new G4LogicalBorderSurface("PMTOpSurface", PMT_phys, PMT_phys_near2, PMTRefOpSurface1);
//    new G4LogicalBorderSurface("PMTOpSurface", PMT_phys_far, PMT_phys, PMTRefOpSurface1);
//    new G4LogicalBorderSurface("PMTOpSurface", PMT_phys, PMT_phys_far, PMTRefOpSurface1);
    // PMT for Plastic Scintillator
    new G4LogicalBorderSurface("PMTOpSurface3", PMT_PS_phys_1, PMT_phys_PS, PMTRefOpSurface1);
    new G4LogicalBorderSurface("PMTOpSurface3", PMT_phys_PS, PMT_PS_phys_1, PMTRefOpSurface1);
    new G4LogicalBorderSurface("PMTOpSurface4", PMT_PS_phys_2, PMT_phys_PS, PMTRefOpSurface1);
    new G4LogicalBorderSurface("PMTOpSurface4", PMT_phys_PS, PMT_PS_phys_2, PMTRefOpSurface1);


//    new G4LogicalSkinSurface("PMTOpSurface", PMT_base_log, PMTRefOpSurface1);

// ------------------------------ Absorption Surface ------------------------------
     auto* AbsorptionSurface = new G4OpticalSurface("AbsorptionSurface");
     AbsorptionSurface->SetModel(unified);
     AbsorptionSurface->SetType(dielectric_metal);
     AbsorptionSurface->SetFinish(polished);
     AbsorptionSurface->SetMaterialPropertiesTable(AbsorptionSurfaceMat->GetMaterialPropertiesTable());


//----------------------------- ESRSurface ------------------------------------
    auto* ESRRefOpSurface = new G4OpticalSurface("ESRRefOpSurface");
    ESRRefOpSurface->SetModel(unified);
    ESRRefOpSurface->SetType(dielectric_metal);
    ESRRefOpSurface->SetFinish(polished);

    ESRRefOpSurface->SetMaterialPropertiesTable(ESR->GetMaterialPropertiesTable());

    auto* TyvekOpSurface = new G4OpticalSurface("TyvekOpSurface");
    TyvekOpSurface->SetModel(unified);
    TyvekOpSurface->SetType(dielectric_metal);
    TyvekOpSurface->SetFinish(ground);
    TyvekOpSurface->SetSigmaAlpha(0.2);
    TyvekOpSurface->SetMaterialPropertiesTable(Tyvek->GetMaterialPropertiesTable());


    if (m_add_ESR)
    {
        auto reflector = ESRRefOpSurface;
        if (m_use_diffuse_reflection)
            reflector = TyvekOpSurface;

        if (use_tank)
        {
            new G4LogicalBorderSurface("ESROpSurface", Tank_phys, ESR_phys_bottom, reflector);
            new G4LogicalBorderSurface("ESROpSurface",  Tank_phys, ESR_phys_upper, reflector);
        }
        else
        {
            new G4LogicalBorderSurface("ESROpSurface_bottom", GdLS_phys,ESR_phys_bottom,  reflector);
            new G4LogicalBorderSurface("ESROpSurface_upper",   GdLS_phys, ESR_phys_upper, reflector);
        }
    }

    if (add_new_detector & add_ESR_for_new_detector)
    {
        if (use_tank)
        {
            new G4LogicalBorderSurface("ESROpSurface", Tank_phys, ESR_phys_right_new_detector, ESRRefOpSurface);
            new G4LogicalBorderSurface("ESROpSurface",  Tank_phys, ESR_phys_left_new_detector, ESRRefOpSurface);
        }
        else
        {
            new G4LogicalBorderSurface("ESROpSurface_bottom", GdLS_phys,ESR_phys_left_new_detector, ESRRefOpSurface);
            new G4LogicalBorderSurface("ESROpSurface_upper",   GdLS_phys,ESR_phys_right_new_detector, ESRRefOpSurface);
        }

    }

    // ESR for Plastic Scintillator

    new G4LogicalBorderSurface("ESROpSurface_bottom_PS", PS_phys,ESR_phys_bottom_PS, ESRRefOpSurface);
    new G4LogicalBorderSurface("ESROpSurface_upper_PS",   PS_phys,ESR_phys_upper_PS, ESRRefOpSurface);
    new G4LogicalBorderSurface("ESROpSurface_left_PS",   PS_phys,ESR_phys_left_PS, ESRRefOpSurface);
    new G4LogicalBorderSurface("ESROpSurface_right_PS",   PS_phys,ESR_phys_right_PS, ESRRefOpSurface);


//    if (add_UpperAbsorptionSurface)
//    {
//        auto reflector = AbsorptionSurface;
//
//        if (use_tank)
//        {
////            new G4LogicalBorderSurface("ESROpSurface", Tank_phys, A_phys_bottom, reflector);
//            new G4LogicalBorderSurface("AbsorberOpSurface",  Tank_phys, Absorber_phys_upper, reflector);
//        }
//        else
//        {
////            new G4LogicalBorderSurface("ESROpSurface_bottom", GdLS_phys,ESR_phys_bottom,  reflector);
//            new G4LogicalBorderSurface("AbsorberOpSurface_upper",   GdLS_phys, Absorber_phys_upper, reflector);
//        }
//
//    }



}

void NeutrinoDetectorConstruction::ConstructMaterials()
{
    bool any_warnings = false;
    // elements
    G4Element* H  = new G4Element("Hydrogen", "H" , 1., 1.01*g/mole);
    G4Element* O  = new G4Element("Oxygen", "O", 8., 16.00*g/mole);
    G4Element* B  = new G4Element("Boron",  "B", 5, 10.811*g/mole);
    G4Element* Na = new G4Element("Sodium", "Na", 11., 22.98977*g/mole);
    G4Element* Si = new G4Element("Silicon", "Si", 14., 28.09*g/mole);
    G4Element* Ar = new G4Element("Argon", "Ar", 18, 39.948*g/mole);
    G4Element* C  = new G4Element("Carbon", "C" , 6., 12.01*g/mole);
    G4Element* N  = new G4Element("Nitrogen", "N", 7., 14.01*g/mole);
    G4Element* K  = new G4Element("Potassium", "K", 19., 39.0983*g/mole);
    G4Element* Fe = new G4Element("Iron", "Fe", 26., 55.845*g/mole);
    G4Element* Mn = new G4Element("Mn", "Mn", 25., 54.9381*g/mole);
    G4Element* P  = new G4Element("Phosphorus", "P", 15, 30.9738*g/mole);
    G4Element* S  = new G4Element("Sulfur", "S", 16., 32.066*g/mole);
    G4Element* Cr = new G4Element("Cr", "Cr", 24, 51.9961*g/mole);
    G4Element* Ni = new G4Element("Ni", "Ni", 28, 58.6934*g/mole);
    G4Element* Cl = new G4Element("Cl", "Cl", 17, 35.45*g/mole);



    G4Element* TS_H_of_Water = G4Element::GetElement("TS_H_of_Water", any_warnings);
    if (not TS_H_of_Water) {
        TS_H_of_Water = new G4Element("TS_H_of_Water", "H_WATER", 1., 1.01 * g / mole);
    }

    G4Element* TS_H_of_Polyethylene = G4Element::GetElement("TS_H_of_Polyethylene", any_warnings);
    if (not TS_H_of_Polyethylene) {
        TS_H_of_Polyethylene = new G4Element("TS_H_of_Polyethylene", "H_POLYETHYLENE" , 1., 1.01*g/mole);
    }

    G4Element* TS_C_of_Graphite = G4Element::GetElement("TS_C_of_Graphite", any_warnings);
    if (not TS_C_of_Graphite) {
        TS_C_of_Graphite = new G4Element("TS_C_of_Graphite", "C_GRAPHITE" , 6., 12.01*g/mole);
    }



    // Element Gd
    G4Isotope* Gd152 = new G4Isotope("Gd152", 64, 152, 152.0*g/mole);
    G4Isotope* Gd154 = new G4Isotope("Gd154", 64, 154, 154.0*g/mole);
    G4Isotope* Gd155 = new G4Isotope("Gd155", 64, 155, 155.0*g/mole);
    G4Isotope* Gd156 = new G4Isotope("Gd156", 64, 156, 156.0*g/mole);
    G4Isotope* Gd157 = new G4Isotope("Gd157", 64, 157, 157.0*g/mole);
    G4Isotope* Gd158 = new G4Isotope("Gd158", 64, 158, 158.0*g/mole);
    G4Isotope* Gd160 = new G4Isotope("Gd160", 64, 160, 160.0*g/mole);
    G4Element* Gd = new G4Element("Gadolinium","Gd",7);
    Gd->AddIsotope(Gd152,  0.2*perCent);
    Gd->AddIsotope(Gd154,  2.2*perCent);
    Gd->AddIsotope(Gd155, 14.9*perCent);   //beware: it is abundance,
    Gd->AddIsotope(Gd156, 20.6*perCent);   //        not fractionMass
    Gd->AddIsotope(Gd157, 15.7*perCent);
    Gd->AddIsotope(Gd158, 24.7*perCent);
    Gd->AddIsotope(Gd160, 21.7*perCent);

    // materials
    // water
    G4double density = 1.000*g/cm3;
    water = new G4Material("Water", density, 2);
    water->AddElement(H, 2);
    water->AddElement(O, 1);
    G4MaterialPropertiesTable* WaterMPT = new G4MaterialPropertiesTable();
    WaterMPT->AddProperty("RINDEX", fPP_Water_RIN, fWaterRINDEX,36);
    double water_abslen_scale_factor = 90.*m/(2651.815*cm);
    for (int j = 0; j < 316; ++j) {
        fWaterABSORPTION[j] *= water_abslen_scale_factor;
    }
    WaterMPT->AddProperty("ABSLENGTH", fPP_Water_ABS,fWaterABSORPTION, 316);
    water->SetMaterialPropertiesTable(WaterMPT);

    // pyrex
    density = 2.23*g/cm3;
    G4Material* SiO2 = new G4Material("SiO2", density, 2);
    SiO2->AddElement(Si, 1);
    SiO2->AddElement(O , 2);

    density = 2.23*g/cm3;
    G4Material* B2O2 = new G4Material("B2O2", density, 2);
    B2O2->AddElement(B,  2);
    B2O2->AddElement(O,  2);

    density = 2.23*g/cm3;
    G4Material* Na2O = new G4Material("Na2O", density, 2);
    Na2O->AddElement(Na, 2);
    Na2O->AddElement(O,  1);

    density = 2.23*g/cm3;
    pyrex = new G4Material("Pyrex", density, 3);
    pyrex->AddMaterial(SiO2, .80);
    pyrex->AddMaterial(B2O2, .13);
    pyrex->AddMaterial(Na2O, .07);
    G4MaterialPropertiesTable* PyrexMPT = new G4MaterialPropertiesTable();
    PyrexMPT->AddProperty("RINDEX", fPP_Pyrex, fPyrexRINDEX, 6);
    PyrexMPT->AddProperty("ABSLENGTH", fPP_PyrexABS, fPyrexABSORPTION, 9);
    pyrex->SetMaterialPropertiesTable(PyrexMPT);

    // steel
    density = 8.1*g/cm3;
    steel = new G4Material("Steel", density, 8);
    steel->AddElement(Fe, 0.70845);
    steel->AddElement(C, 0.0008);
    steel->AddElement(Mn, 0.02);
    steel->AddElement(P, 0.00045);
    steel->AddElement(S, 0.00030);
    steel->AddElement(Si, 0.01);
    steel->AddElement(Cr, 0.18);
    steel->AddElement(Ni, 0.08);
    G4MaterialPropertiesTable* SteelMPT = new G4MaterialPropertiesTable();
    SteelMPT->AddProperty("ABSLENGTH", fPP_SteelTank, fSteelTankABSORPTION, 4);
    steel->SetMaterialPropertiesTable(SteelMPT);

    // air
    density = 1.205e-3*g/cm3;
    air = new G4Material("Air", density, 4);
    air->AddElement(N, 0.7550);
    air->AddElement(O, 0.2321);
    air->AddElement(Ar, 0.0128);
    air->AddElement(C, 0.0001);
    G4double airPP[2] =
    {
        1.55*eV, 6.20*eV
    };
    G4double airRINDEX[2] =
    {
        1.000273, 1.000273
    };
    G4double airABSLENGTH[2] =
    {
        10000*m, 10000*m
    };
    G4MaterialPropertiesTable* airMPT = new G4MaterialPropertiesTable();
    airMPT->AddProperty("RINDEX",    airPP, airRINDEX,     2);
    airMPT->AddProperty("ABSLENGTH", airPP, airABSLENGTH,  2);
    air->SetMaterialPropertiesTable(airMPT);


    // Vacuum
    vacuum_pure = new G4Material("Galactic", 1., 1.01*g/mole, universe_mean_density, kStateGas, 2.73*kelvin, 3.e-18*pascal);
//    auto vacuum_pure_MPT = new G4MaterialPropertiesTable();
//    vacuum_pure_MPT->AddProperty("RINDEX",    vaPP, airRINDEX,     2);
//    vacuum_pure->AddProperty("RINDEX",    airPP, airRINDEX,     2);

    // vacuum
    density     =  1e-3 * CLHEP::kGasThreshold;
    G4double temperature = CLHEP::STP_Temperature;
    G4double pressure    = CLHEP::STP_Pressure * density / (1.29e-3*g/cm3);
    vacuum = new G4Material("Vacuum", density, 1, kStateGas,temperature,pressure);
    vacuum->AddMaterial(air, 1.);
    G4MaterialPropertiesTable* VacuumMPT = new G4MaterialPropertiesTable();
    G4double VacuumPP[2] =
    {
        1.55*eV, 15.20*eV
    };
    G4double VacuumRINDEX[2] =
    {
        1.0029, 1.0029
    };

//    G4MaterialPropertyVector* mpv = new G4MaterialPropertyVector();
//    for (int ie=0; ie<2; ++ie) {
//        mpv->InsertValues(VacuumPP[ie], VacuumRINDEX[ie]);
//    }
//
//    VacuumMPT->AddProperty("RINDEX", mpv);
    VacuumMPT->AddProperty("RINDEX", VacuumPP, VacuumRINDEX, 2);
    vacuum->SetMaterialPropertiesTable(VacuumMPT);

    // hamamatsu pmt photocathode 3inch
    Photocathode_mat_Ham20inch = new G4Material("photocathode_Ham20inch",density,1);
    Photocathode_mat_Ham20inch->AddElement(K, 1);
    G4MaterialPropertiesTable* PhotocathodeMPT_Ham20inch = new G4MaterialPropertiesTable();
    PhotocathodeMPT_Ham20inch->AddProperty("RINDEX", fPP_PhC_NEW, fPhCRINDEX_NEW, 25);
    PhotocathodeMPT_Ham20inch->AddProperty("KINDEX", fPP_PhC_NEW, fPhCKINDEX_NEW, 25);
    PhotocathodeMPT_Ham20inch->AddProperty("REFLECTIVITY", fPP_PhC, fPhCREFLECTIVITY, 4);
    PhotocathodeMPT_Ham20inch->AddProperty("EFFICIENCY", fPP_PhCQE_Dynode20inch, fPhCEFFICIENCY_Dynode20inch, 43);
    PhotocathodeMPT_Ham20inch->AddProperty("THICKNESS", fPosZ, fTHICKNESS, 2);
    Photocathode_mat_Ham20inch->SetMaterialPropertiesTable(PhotocathodeMPT_Ham20inch);



    // hamamatsu pmt photocathode
    density = 5. *g/cm3; // true??
    Photocathode_mat_Ham3inch = new G4Material("photocathode_Ham3inch",density,1);
    Photocathode_mat_Ham3inch->AddElement(K, 1);
    G4MaterialPropertiesTable* PhotocathodeMPT_Ham3inch = new G4MaterialPropertiesTable();
    PhotocathodeMPT_Ham3inch->AddProperty("RINDEX", fPP_PhC, fPhCRINDEX, 4);
    PhotocathodeMPT_Ham3inch->AddProperty("KINDEX", fPP_PhC, fPhCKINDEX, 25);
    PhotocathodeMPT_Ham3inch->AddProperty("REFLECTIVITY", fPP_PhC, fPhCREFLECTIVITY, 4);
    PhotocathodeMPT_Ham3inch->AddProperty("EFFICIENCY", fPP_PhCQE_3inch, fPhCEFFICIENCY_3inch, 61);
    PhotocathodeMPT_Ham3inch->AddProperty("THICKNESS", fPosZ, fTHICKNESS, 2);
    Photocathode_mat_Ham3inch->SetMaterialPropertiesTable(PhotocathodeMPT_Ham3inch);

    // mcp-pmt photocathode
    density = 5. *g/cm3; // true??
    Photocathode_mat_MCP20inch = new G4Material("photocathode_MCP20inch",density,1);
    Photocathode_mat_MCP20inch->AddElement(K, 1);
    G4MaterialPropertiesTable* PhotocathodeMPT_MCP20inch = new G4MaterialPropertiesTable();
    PhotocathodeMPT_MCP20inch->AddProperty("RINDEX", fPP_PhC_NEW, fPhCRINDEX_NEW, 25);
    PhotocathodeMPT_MCP20inch->AddProperty("KINDEX", fPP_PhC_NEW, fPhCKINDEX_NEW, 25);
    PhotocathodeMPT_MCP20inch->AddProperty("REFLECTIVITY", fPP_PhC, fPhCREFLECTIVITY, 4);
    PhotocathodeMPT_MCP20inch->AddProperty("EFFICIENCY", fPP_PhCQE_AverageMCP20inch, fPhCEFFICIENCY_AverageMCP20inch, 43);
    PhotocathodeMPT_MCP20inch->AddProperty("THICKNESS", fPosZ, fTHICKNESS, 2);
    Photocathode_mat_MCP20inch->SetMaterialPropertiesTable(PhotocathodeMPT_MCP20inch);

    // R7600U photocathode
    density = 5. *g/cm3; // true??
    Photocathode_mat_R7600U= new G4Material("photocathode_R7600U",density,1);
    Photocathode_mat_R7600U->AddElement(K, 1);
    G4MaterialPropertiesTable* PhotocathodeMPT_R7600U = new G4MaterialPropertiesTable();
    PhotocathodeMPT_R7600U->AddProperty("RINDEX", fPP_PhC_NEW, fPhCRINDEX_NEW, 25);
    PhotocathodeMPT_R7600U->AddProperty("EFFICIENCY", fPP_energy_QE_R7600U, fPP_QE_R7600U, 35);
    PhotocathodeMPT_R7600U->AddProperty("THICKNESS", fPosZ, fTHICKNESS, 2);
    Photocathode_mat_R7600U->SetMaterialPropertiesTable(PhotocathodeMPT_MCP20inch);

    // Acrylic
    density = 1.18*g/cm3;
    Acrylic = new G4Material("Acrylic", density,3);
    Acrylic->AddElement(C, 59.984*perCent);
    Acrylic->AddElement(H, 8.055*perCent);
    Acrylic->AddElement(O, 31.961*perCent);
    auto* AcrylicPropertiesTable= new G4MaterialPropertiesTable();
    AcrylicPropertiesTable->AddProperty("RINDEX", AcrylicRefEnergy,AcrylicRefIndex, 18);
    AcrylicPropertiesTable->AddProperty("ABSLENGTH", AcrylicAbsEnergy, AcrylicAbsLength, 25);
    AcrylicPropertiesTable->AddProperty("RAYLEIGH", AcrylicRayEnergy,AcrylicRayLength, 11);
    Acrylic->SetMaterialPropertiesTable(AcrylicPropertiesTable);

    // Opaque Acrylic
    density = 1.18*g/cm3;
    OpaqueAcrylic = new G4Material("OpaqueAcrylic", density,3);
    OpaqueAcrylic->AddElement(C, 59.984*perCent);
    OpaqueAcrylic->AddElement(H, 8.055*perCent);
    OpaqueAcrylic->AddElement(O, 31.961*perCent);
    auto* OpaqueAcrylicPropertiesTable= new G4MaterialPropertiesTable();
    OpaqueAcrylicPropertiesTable->AddProperty("RINDEX", AcrylicRefEnergy,AcrylicRefIndex, 18);
    OpaqueAcrylicPropertiesTable->AddProperty("ABSLENGTH", AcrylicAbsEnergy, OpaqueAcrylicAbsLength, 25);
    OpaqueAcrylicPropertiesTable->AddProperty("RAYLEIGH", AcrylicRayEnergy,AcrylicRayLength, 11);
    OpaqueAcrylic->SetMaterialPropertiesTable(OpaqueAcrylicPropertiesTable);

    // PVC
    density = 1.42*g/cm3;
    PVC = new G4Material("PVC", density, 3);
    PVC->AddElement(C, 2);
    PVC->AddElement(H, 3);
    PVC->AddElement(Cl, 1);
    auto* PVCPropertiesTable= new G4MaterialPropertiesTable();
    PVCPropertiesTable->AddProperty("RINDEX", PVCRefEnergy, PVCRefIndex, 2);
    PVC->SetMaterialPropertiesTable(PVCPropertiesTable);

    // ESR mirror
    ESR = new G4Material("ESR", 1.0*g/cm3, 2);
    ESR->AddElement(H, 2.*1.00794/(12.0107+2.*1.00794));
    ESR->AddElement(C, 12.0107/(12.0107+2.*1.00794));

    auto ESRMPT = new G4MaterialPropertiesTable();
    ESRMPT->AddProperty("ABSLENGTH", ESREnergy, ESRAbsorptionLength, 31);
    ESRMPT->AddProperty("REFLECTIVITY", ESREnergy, ESRReflectivity, 31);
    ESRMPT->AddProperty("SPECULARLOBECONSTANT", ESREnergy, ESRSpecularlobe, 31);
    ESRMPT->AddProperty("SPECULARSPIKECONSTANT", ESREnergy, ESRSpecularspike, 31);
    ESRMPT->AddProperty("RINDEX", ESREnergy, ESRRefractionIndex, 31);

    ESR->SetMaterialPropertiesTable(ESRMPT);

    // Tyvek ( diffuse reflection )
    Tyvek = new G4Material("Tyvek", 0.94*g/cm3, 2);
    Tyvek->AddElement(C, 2);
    Tyvek->AddElement(H, 4);

    auto TyvekMPT = new G4MaterialPropertiesTable();
    TyvekMPT->AddProperty("REFLECTIVITY", TyvekEnergyForReflectivity, TyvekReflectivity,31);
    TyvekMPT->AddProperty("ABSLENGTH", TyvekEnergy, TyvekABSLength, 4);
//    TyvekMPT->AddProperty("RINDEX", TyvekEnergyForReflectivity, ESRRefractionIndex, 31);
    Tyvek->SetMaterialPropertiesTable(TyvekMPT);

    // Absorption Surface (temporarily define material as tyvek)
    AbsorptionSurfaceMat =  new G4Material("AbsorptionSurface", 0.94*g/cm3, 2);
    AbsorptionSurfaceMat->AddElement(C, 2);
    AbsorptionSurfaceMat->AddElement(H, 4);

    auto AbsorptionSurfaceMPT = new G4MaterialPropertiesTable();
    AbsorptionSurfaceMPT->AddProperty("REFLECTIVITY", AbsorptionSurfaceEnergy, AbsorptionSurfaceReflectivity,2);
    AbsorptionSurfaceMPT->AddProperty("ABSLENGTH", AbsorptionSurfaceEnergy, AbsorptionSurfaceAbS, 2);
//    TyvekMPT->AddProperty("RINDEX", TyvekEnergyForReflectivity, ESRRefractionIndex, 31);

    // quartz (SiO2, crystalline)
    density = 2.649 *g/cm3;
    Quartz = new G4Material("Quartz", density, 2);
    Quartz-> AddElement(Si, 1);
    Quartz-> AddElement(O,  2);
    auto* QuartzPropertiesTable= new G4MaterialPropertiesTable();
    QuartzPropertiesTable->AddProperty("RINDEX", QuartzRefEnergy, QuartzRefIndex, 101);
    PVC->SetMaterialPropertiesTable(QuartzPropertiesTable);


    // GdLS
    // COMMENT: element component no elS ?
    density = 0.855*g/cm3;
    GdLS = new G4Material("GdLiquidScintillator", density, 5);
    GdLS->AddElement(C, 85.35*perCent);
    GdLS->AddElement(H, 12.88*perCent);
    GdLS->AddElement(O, 1.64*perCent);
    GdLS->AddElement(N,  0.03*perCent);
    GdLS->AddElement(Gd, 0.10*perCent);


    // JUNO LS
    /////////////////////// Begin of LS Definition ///////////////////////////////
    LS  = new G4Material("LS", 0.859*g/cm3, 5);
    LS->AddElement(TS_C_of_Graphite,  0.87924);
    //LS->AddElement(H,  0.1201);
    LS->AddElement(TS_H_of_Water,  0.1201);
    LS->AddElement(O,  0.00034);
    //LS->AddElement(Gd, 0.0010315);
    LS->AddElement(N,  0.00027);
    LS->AddElement(S,  0.00005);

    auto LSMPT = new G4MaterialPropertiesTable();

    LSMPT->AddProperty("RINDEX",   GdLSRefIndexEnergy, GdLSRefIndex, 18);
    LSMPT->AddProperty("ABSLENGTH", GdLSABSEnergy_new, GdLSABSLength_new, 601);
    LSMPT->AddProperty("FASTCOMPONENT", GdLSComEnergy, GdLSFastComponent, 275);
    LSMPT->AddProperty("SLOWCOMPONENT", GdLSComEnergy, GdLSFastComponent, 275);
    LSMPT->AddProperty("REEMISSIONPROB", GdLSReemEnergy, GdLSReem, 28);

    // Shift RAYLEIGH Length
    double shifted_GdLSRayLength[11];
//    std::cout<< "Shifted Length:" << std::endl;
    for (int i=0;i<11;i++)
    {
        shifted_GdLSRayLength[i] = GdLSRayLength[i] *(1.+m_shiftLengthRAYLEIGH);
//        std::cout << shifted_GdLSRayLength[i]/m << "\t";
    }
//    std::cout<< std::endl;

    LSMPT->AddProperty("RAYLEIGH", GdLSRayEnergy, shifted_GdLSRayLength, 11);
    LSMPT->AddProperty("SCINTILLATIONYIELD", component, GdLSLY,2);
    LSMPT->AddProperty("RESOLUTIONSCALE", component, GdLSResolutionScale,2);

    LSMPT->AddConstProperty("SCINTILLATIONYIELD", GdLSLY[0]);
    LSMPT->AddConstProperty("RESOLUTIONSCALE",GdLSResolutionScale[0]);
    LSMPT->AddConstProperty("FASTTIMECONSTANT",GdLSFastTimeConstant[0]);
    LSMPT->AddConstProperty("SLOWTIMECONSTANT",GdLSSlowTimeConstant[0]);
    LSMPT->AddConstProperty("YIELDRATIO",GdLSYieldRatio[0]);

    LSMPT->AddProperty("GammaFASTTIMECONSTANT", component, GdLSFastTimeConstant,2);
    LSMPT->AddProperty("GammaSLOWTIMECONSTANT", component, GdLSSlowTimeConstant,2);
    LSMPT->AddProperty("GammaYIELDRATIO", component, GdLSYieldRatio,2);

    // add fast/slow time constant for alpha
    LSMPT->AddProperty("AlphaFASTTIMECONSTANT", component, GdLSAlphaFastTimeConstant,2);
    LSMPT->AddProperty("AlphaSLOWTIMECONSTANT", component, GdLSAlphaSlowTimeConstant,3);
    LSMPT->AddProperty("AlphaYIELDRATIO", component, GdLSAlphaYieldRatio,2);

    // add dE/dx dependent time constant (added by luoxj)
    LSMPT->AddProperty("N1CONSTANT", N1CONSTANT_Energy, N1CONSTANT_Value, 3);
    LSMPT->AddProperty("N2CONSTANT", N2CONSTANT_Energy, N2CONSTANT_Value, 3);
    LSMPT->AddProperty("N3CONSTANT", N3CONSTANT_Energy, N3CONSTANT_Value, 3);
    LSMPT->AddProperty("tau1CONSTANT", tau1CONSTANT_Energy, tau1CONSTANT_Value,3);
    LSMPT->AddProperty("tau2CONSTANT", tau2CONSTANT_Energy, tau2CONSTANT_Value,3);
    LSMPT->AddProperty("tau3CONSTANT", tau3CONSTANT_Energy, tau3CONSTANT_Value,3);
    LSMPT->AddProperty("OpticalCONSTANT", OpticalCONSTANT_Energy, OpticalCONSTANT_Value, 2);
    LSMPT->AddProperty("InstantCONSTANT", InstantEmission_Energy, InstantEmission_Value, 2);

    // add particle dependent time constants(v2)
    LSMPT->AddProperty("NeutronCONSTANT", NeutronTimeConstant, NeutronYieldRatio,3);
    LSMPT->AddProperty("GammaCONSTANT", GammaTimeConstant, GammaYieldRatio,3);
    LSMPT->AddProperty("AlphaCONSTANT", AlphaTimeConstant, AlphaYieldRatio,3);



    // add fast/slow time constant for neutron
    LSMPT->AddProperty("NeutronFASTTIMECONSTANT", component, GdLSNeutronFastTimeConstant,2);
    LSMPT->AddProperty("NeutronSLOWTIMECONSTANT", component, GdLSNeutronSlowTimeConstant,2);
    LSMPT->AddProperty("NeutronYIELDRATIO", component, GdLSNeutronYieldRatio,2);

    // add fast/slow time constant for reemission
    LSMPT->AddProperty("ReemissionFASTTIMECONSTANT", component, GdLSReemissionFastTimeConstant,2);
    LSMPT->AddProperty("ReemissionSLOWTIMECONSTANT", component, GdLSReemissionSlowTimeConstant,2);
    LSMPT->AddProperty("ReemissionYIELDRATIO", component, GdLSReemissionYieldRatio,2);

    // Following lines are for new Optical Model.
    // + PART I: Emission by PPO
    // + PART II: Absorption and Re-emission by PPO, bis-MSB and LAB
    //   Need:
    //     + XXXABSLENGTH
    //     + XXXREEMISSIONPROB
    //     + XXXCOMPONENT, maybe FAST/SLOW
    //     + XXXTIMECONSTANT, maybe FAST/SLOW
    //   XXX in [PPO, bisMSB, LAB]
    LSMPT->AddProperty("PPOABSLENGTH", GdLSABSEnergy_new, GdLSABSLength_new, 601);
    LSMPT->AddProperty("PPOREEMISSIONPROB", GdLSReemEnergy, GdLSReem, 28);
    LSMPT->AddProperty("PPOCOMPONENT", GdLSComEnergy, GdLSFastComponent, 275);
    LSMPT->AddProperty("PPOTIMECONSTANT", component, GdLSReemissionFastTimeConstant,2);

    LSMPT->AddProperty("bisMSBABSLENGTH", GdLSABSEnergy_new, GdLSABSLength_new, 601);
    LSMPT->AddProperty("bisMSBREEMISSIONPROB", GdLSReemEnergy, GdLSReem, 28);
    LSMPT->AddProperty("bisMSBCOMPONENT", GdLSComEnergy, GdLSFastComponent, 275);
    LSMPT->AddProperty("bisMSBTIMECONSTANT", component, GdLSReemissionFastTimeConstant,2);

    // LSMPT->DumpTable();
    LS->SetMaterialPropertiesTable(LSMPT);


    G4cout << "LAB is constructed from the code." << G4endl;
    LAB = new G4Material("LAB", 0.859*g/cm3, 5);
    //LAB->AddElement(C,  0.87924);
    LAB->AddElement(TS_C_of_Graphite,  0.87924);
    //LAB->AddElement(H,  0.1201);
    LAB->AddElement(TS_H_of_Water,  0.1201);
    LAB->AddElement(O,  0.00034);
    //LAB->AddElement(Gd, 0.0010315);
    LAB->AddElement(N,  0.00027);
    LAB->AddElement(S,  0.00005);

    G4MaterialPropertiesTable* LABMPT = new G4MaterialPropertiesTable();
    LABMPT->AddProperty("RINDEX",   GdLSRefIndexEnergy, GdLSRefIndex, 18);
    LABMPT->AddProperty("ABSLENGTH", GdLSABSEnergy_new, GdLSABSLength_new, 601);
    LABMPT->AddProperty("RAYLEIGH", GdLSRayEnergy, GdLSRayLength, 11);
    LAB->SetMaterialPropertiesTable(LABMPT);
    //////////////////////// End of LS Definition ////////////////////////////////////////////////////

    //////////////////////// Definition of Plastic Scintillator /////////////////////////////////////
    // plastic scintillator
    density = 1.032*g/cm3;
    PS = new G4Material("PS", density, 2);
    PS->AddElement(C, 0.91497);
    PS->AddElement(H, 0.08503);

    G4MaterialPropertiesTable* PSMPT = new G4MaterialPropertiesTable();
    PSMPT->AddProperty("RINDEX", fPP_PS_RINDEX, PS_RINDEX, 12);
    PSMPT->AddProperty("ABSLENGTH", fPP_PS_ABS, PS_ABS, 12);
    PSMPT->AddProperty("FASTCOMPONENT", fPP_PS_FAST, PS_FAST, 12);
    PSMPT->AddProperty("SLOWCOMPONENT", fPP_PS_SLOW, PS_SLOW, 2);
    PSMPT->AddConstProperty("SCINTILLATIONYIELD", m_LY_PS/MeV); // default 6000/MeV
    PSMPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
    PSMPT->AddConstProperty("FASTTIMECONSTANT", 2.4*ns);
    PSMPT->AddConstProperty("YIELDRATIO", 1.0);

    // add dE/dx dependent time constant (added by luoxj)
    PSMPT->AddProperty("N1CONSTANT", N1CONSTANT_Energy_PS, N1CONSTANT_Value_PS, 3);
    PSMPT->AddProperty("N2CONSTANT", N2CONSTANT_Energy_PS, N2CONSTANT_Value_PS, 3);
    PSMPT->AddProperty("N3CONSTANT", N3CONSTANT_Energy_PS, N3CONSTANT_Value_PS, 3);
    PSMPT->AddProperty("tau1CONSTANT", tau1CONSTANT_Energy_PS, tau1CONSTANT_Value_PS,3);
    PSMPT->AddProperty("tau2CONSTANT", tau2CONSTANT_Energy_PS, tau2CONSTANT_Value_PS,3);
    PSMPT->AddProperty("tau3CONSTANT", tau3CONSTANT_Energy_PS, tau3CONSTANT_Value_PS,3);
    PSMPT->AddProperty("OpticalCONSTANT", OpticalCONSTANT_Energy, OpticalCONSTANT_Value, 2);


    // add fast/slow time constant for reemission
//    PSMPT->AddProperty("ReemissionFASTTIMECONSTANT", component, GdLSReemissionFastTimeConstant,2);
//    PSMPT->AddProperty("ReemissionSLOWTIMECONSTANT", component, GdLSReemissionSlowTimeConstant,2);
//    PSMPT->AddProperty("ReemissionYIELDRATIO", component, GdLSReemissionYieldRatio,2);
//    PSMPT->AddProperty("REEMISSIONPROB", GdLSReemEnergy, GdLSReem, 28);

    PS->SetMaterialPropertiesTable(PSMPT);

    ////////////////// End of PS Definition ///////////////////////////////

    // Si Oil
    density = 0.838 * g / cm3;
    Oil = new G4Material("MineralOil", density, 2);
    Oil->AddElement(C, 0.8514);
    Oil->AddElement(H, 0.1486);
    G4double OilRayEnergy[11] = {1.55 * eV, 1.7714 * eV, 2.102 * eV, 2.255 * eV,
                                 2.531 * eV, 2.884 * eV, 3.024 * eV, 4.133 * eV, 6.20 * eV, 10.33 * eV, 15.5 * eV};
    G4double OilRayLength[11] = {500.0 * m, 300.0 * m, 170.0 * m, 100.0 * m, 62.0 * m,
                                 42.0 * m, 30.0 * m, 7.6 * m, 0.85 * m, 0.85 * m, 0.85 * m};
    G4double oilenergy[2] = {1.0 * eV, 15.5 * eV};
    G4double oilrefindex[2] = {1.406, 1.406};
    G4MaterialPropertiesTable *OilMPT = new G4MaterialPropertiesTable();
    OilMPT->AddProperty("RAYLEIGH", OilRayEnergy, OilRayLength, 11);
    OilMPT->AddProperty("RINDEX", &fPP_Oil_RIN[0], &fOilRINDEX[0],
                        18);
    OilMPT->AddProperty("RINDEX", oilenergy, oilrefindex, 2);
    OilMPT->AddProperty("ABSLENGTH", &fPP_Oil_ABS[0], &fOilABSORPTION[0],
                        543);
    Oil->SetMaterialPropertiesTable(OilMPT);

    // alminium
    G4double  A, Z;
    A= 26.98 *g/mole;
    density= 2.70 *g/cm3;
    Al = new G4Material("Al", Z=13., A, density);

    // lead
    A= 207.2 *g/mole;
    density= 11.35 *g/cm3;
    Pb = new G4Material("Lead", Z=82., A, density);
}

void NeutrinoDetectorConstruction::ConstructSDandField()
{
    G4String name_LS_SD = "GdLS_log";
    auto* Tracker_LS_SD = new MyTrackerSD(name_LS_SD, "TrackerHitCollection_LS");
    G4SDManager::GetSDMpointer()->AddNewDetector(Tracker_LS_SD);
    SetSensitiveDetector("GdLS_log", Tracker_LS_SD);

    G4String name_PS_SD = "PSVol_log";
    auto* Tracker_PS_SD = new MyTrackerSD(name_PS_SD, "TrackerHitCollection_PSVol");
    G4SDManager::GetSDMpointer()->AddNewDetector(Tracker_PS_SD);
    SetSensitiveDetector("PS_log", Tracker_PS_SD);
//
    G4String name_PMT_SD = "PMT_log";
    auto Tracker_PMT_SD = new MyTrackerSD(name_PMT_SD, "TrackerHitCollection_PMT");
    G4SDManager::GetSDMpointer()->AddNewDetector(Tracker_PMT_SD);
    SetSensitiveDetector("PMT_log", Tracker_PMT_SD);

    G4String name_PMT_PS_SD = "PMT_log_PS";
    auto Counter_PMT_PS_SD = new MyCounterSD(name_PMT_PS_SD, "PMT_log_PS_Counter");
    G4SDManager::GetSDMpointer()->AddNewDetector(Counter_PMT_PS_SD);
    SetSensitiveDetector("PMT_log_PS", Counter_PMT_PS_SD);

    G4String name_PMT_R7600U_SD = "PMT_log_R7600";
    auto Tracker_PMT_R7600U_SD = new MyTrackerSD(name_PMT_R7600U_SD, "TrackerHitCollection_PMT_R7600U");
    G4SDManager::GetSDMpointer()->AddNewDetector(Tracker_PMT_R7600U_SD);
    SetSensitiveDetector("PMT_log_R7600", Tracker_PMT_R7600U_SD);

    if (add_new_detector)
    {
        G4String name_Si_SD = "Si_log";
        auto Tracker_Si_SD = new MyTrackerSD(name_Si_SD, "TrackerHitCollection_Si");
        G4SDManager::GetSDMpointer()->AddNewDetector(Tracker_Si_SD);
        SetSensitiveDetector("log_Si_detector", Tracker_Si_SD);
    }

    if (m_add_distance2PE_calib)
    {
        for (const auto & j : v_name_PMT_calib) {
            G4String name_PMT_calib_SD = "PMT_" + j + "_calib_distance2PE_log";
            auto Tracker_PMT_calib = new MyTrackerSD(name_PMT_calib_SD, "TrackerHitCollection_"+name_PMT_calib_SD);
            G4SDManager::GetSDMpointer()->AddNewDetector(Tracker_PMT_calib);
            SetSensitiveDetector(name_PMT_calib_SD, Tracker_PMT_calib);
        }

    }

//
//    G4String name_PMT_near2_SD = "PMT_log_near2";
//    auto Tracker_PMT_near2_SD = new MyTrackerSD(name_PMT_near2_SD, "TrackerHitCollection_PMT_near2");
//    G4SDManager::GetSDMpointer()->AddNewDetector(Tracker_PMT_near2_SD);
//    SetSensitiveDetector("PMT_log_near2", Tracker_PMT_near2_SD);
}

