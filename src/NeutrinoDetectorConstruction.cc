#include "../include/NeutrinoDetectorConstruction.hh"

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


NeutrinoDetectorConstruction::NeutrinoDetectorConstruction() :
    G4VUserDetectorConstruction(),
    solidWorld(NULL), logicWorld(NULL), physWorld(NULL),
    fCheckOverlaps(false)
{
}

NeutrinoDetectorConstruction::~NeutrinoDetectorConstruction()
{
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
    G4double expHallLength_x = 3 * m, expHallLength_y = 3 * m, expHallLength_z = 3 * m;
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
    G4double ChamberLength_x = 100. * cm, ChamberLength_y = 100. * cm, ChamberLength_z = 100. * cm;
    auto *Chamber = new G4Box("Chamber", 0.5 * ChamberLength_x, 0.5 * ChamberLength_y, 0.5 * ChamberLength_z);
    G4LogicalVolume *Chamber_log = new G4LogicalVolume(Chamber, vacuum_pure, "Chamber_log", 0, 0, 0);
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
    G4double PMT_3inch_r = 38.1 * mm;
    G4double PMT_3inch_thickness = 1 * cm;
    G4double Al_thickness = 0.1 * mm;
    G4double Acrylic_thickness = 0.0001 * mm;
    G4double GdLS_Length = 5 * mm;
    G4double GdLS_r = 3 * mm;
    G4double Acrylic_r = GdLS_r + Acrylic_thickness;
    G4double Acrylic_Length = GdLS_Length + 2 * Acrylic_thickness;
    G4double SiOil_r = PMT_3inch_r;
    G4double SiOil_thickness = 0.1 * mm;

    G4LogicalVolume* mother_log_of_LS = NULL;
    if (use_tank)
    {
        //-------------------- Acrylic tank  ----------------------------------------
        G4Tubs *Tank = new G4Tubs("Tank",
                                  0. * mm,
                                  Acrylic_r,
                                  0.5 * Acrylic_Length,
                                  0. * deg,
                                  360. * deg);
        G4LogicalVolume *Tank_log = new G4LogicalVolume(Tank,
                                                        Acrylic,
                                                        "Tank_log",
                                                        0,
                                                        0,
                                                        0);
        G4VPhysicalVolume *Tank_phys = new G4PVPlacement(0,
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
    G4Tubs *GdLS_solid = new G4Tubs("GdLS_solid",
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

//-------------------- SiOil ------------------------

    G4Tubs *SiOil = new G4Tubs("SiOil",
                               0. * mm,
                               SiOil_r,
                               0.5 * SiOil_thickness,
                               0. * deg,
                               360. * deg);
    G4LogicalVolume *SiOil_log = new G4LogicalVolume(SiOil,
                                                     Oil,
                                                     "SiOil_log",
                                                     0,
                                                     0,
                                                     0);
    G4Tubs *PMT = new G4Tubs("PMT",
                             0. * mm,
                             PMT_3inch_r,
                             0.5 * PMT_3inch_thickness,
                             0. * deg,
                             360. * deg);
    G4LogicalVolume *PMT_log = new G4LogicalVolume(PMT,
                                                   pyrex,
                                                   "PMT_log",
                                                   0,
                                                   0,
                                                   0);

    for (int i = 0; i < 2; i++) {
        G4VPhysicalVolume *SiOil_phys = new G4PVPlacement(0,
                                                          G4ThreeVector(0, 0, (1 - 2 * i) * 0.5 * (Acrylic_Length +
                                                                                                   SiOil_thickness)),    // at (x,y,z)
                                                          SiOil_log,   // its logical volume
                                                          "SiOil_phys",    // its name
                                                          Chamber_log,      // its mother  volume
                                                          false,           // no boolean operations
                                                          0);
        G4VPhysicalVolume *PMT_phys = new G4PVPlacement(0,
                                                        G4ThreeVector(0, 0, (1 - 2 * i) * 0.5 *
                                                                            (Acrylic_Length + 2 * SiOil_thickness +
                                                                             PMT_3inch_thickness)),    // at (x,y,z)
                                                        PMT_log,   // its logical volume
                                                        "PMT_phys",    // its name
                                                        Chamber_log,      // its mother  volume
                                                        false,           // no boolean operations
                                                        i);
    }

    // --------- Detector periphery-----------
    G4double n_Detector_periphery = 13.;
    G4double tube_dPhi = 2.* M_PI * rad;
    G4double r_distance_periphery = 5. * cm;
    G4double Detector_periphery_r = 5. * cm;
    G4double divided_tube_dPhi = tube_dPhi/n_Detector_periphery;
    G4VSolid* div_tube = new G4Tubs("div_tube",
                                    Acrylic_r+r_distance_periphery,
                                    Acrylic_r+r_distance_periphery+Detector_periphery_r,
                                    0.5*Acrylic_Length,
                                    -divided_tube_dPhi/2.,
                                    divided_tube_dPhi/2.0);
    G4Material* Ge = new G4Material("Germanium",32.,72.61*g/mole,5.323*g/cm3);
    G4LogicalVolume* div_tube_log = new G4LogicalVolume(div_tube,
                                                        Ge,
                                                        "div_tubeL",
                                                        0,0,0);

    for (int i=0;i<n_Detector_periphery;i++)
    {

        G4double delta_Phi = M_PI/n_Detector_periphery * rad;
        G4RotationMatrix* detector_rotate = new G4RotationMatrix();
        detector_rotate->rotateZ(delta_Phi*i);
//            G4double R_Detector_periphery = Acrylic_r+r_distance_periphery+0.5*Detector_periphery_r;
        G4PVPlacement* dir_tube_phys = new G4PVPlacement(detector_rotate,
                                                         G4ThreeVector( TMath::Cos(delta_Phi*i), TMath::Sin(delta_Phi*i),0),    // at (x,y,z)
                                                         div_tube_log,   // its logical volume
                                                         "div_tub_phys",    // its name
                                                         Chamber_log,      // its mother  volume
                                                         false,           // no boolean operations
                                                         i);

    }



    G4VisAttributes *PMT_visatt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));//red
    PMT_visatt->SetForceWireframe(true);
    PMT_visatt->SetForceAuxEdgeVisible(true);
    PMT_log->SetVisAttributes(PMT_visatt);
//------------------------------ SkinSurface -----------------------------------
//        new G4LogicalSkinSurface("RefOpSurface", Alfilm_log, MirrorSurface);
    G4OpticalSurface* PMTRefOpSurface1 = new G4OpticalSurface("PMTRefOpSurface",
                                                              glisur,
            //   unified,  // diffus reflect
            //   groundfrontpainted,   //diffus reflect
            //   ground,   //diffus reflect
                                                              polished,
                                                              dielectric_metal);
    new G4LogicalSkinSurface("PMTOpSurface", PMT_log, PMTRefOpSurface1);



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

    // vacuum
    density     =  1e-3 * CLHEP::kGasThreshold;
    G4double temperature = CLHEP::STP_Temperature;
    G4double pressure    = CLHEP::STP_Pressure * density / (1.29e-3*g/cm3);
    vacuum = new G4Material("Vacuum", density, 1, kStateGas,temperature,pressure);
    vacuum->AddMaterial(air, 1.);
    G4MaterialPropertiesTable* VacuumMPT = new G4MaterialPropertiesTable();
    G4double VacuumPP[2] =
    {
        1.55*eV, 6.20*eV
    };
    G4double VacuumRINDEX[2] =
    {
        1.000, 1.000
    };
    VacuumMPT->AddProperty("RINDEX", VacuumPP, VacuumRINDEX, 2);
    vacuum->SetMaterialPropertiesTable(VacuumMPT);

    // hamamatsu pmt photocathode
    density = 5. *g/cm3; // true??
    Photocathode_mat_Ham20inch = new G4Material("photocathode_Ham20inch",density,1);
    Photocathode_mat_Ham20inch->AddElement(K, 1);
    G4MaterialPropertiesTable* PhotocathodeMPT_Ham20inch = new G4MaterialPropertiesTable();
    PhotocathodeMPT_Ham20inch->AddProperty("RINDEX", fPP_PhC_NEW, fPhCRINDEX_NEW, 25);
    PhotocathodeMPT_Ham20inch->AddProperty("KINDEX", fPP_PhC_NEW, fPhCKINDEX_NEW, 25);
    PhotocathodeMPT_Ham20inch->AddProperty("REFLECTIVITY", fPP_PhC, fPhCREFLECTIVITY, 4);
    PhotocathodeMPT_Ham20inch->AddProperty("EFFICIENCY", fPP_PhCQE_Dynode20inch, fPhCEFFICIENCY_Dynode20inch, 43);
    PhotocathodeMPT_Ham20inch->AddProperty("THICKNESS", fPosZ, fTHICKNESS, 2);
    Photocathode_mat_Ham20inch->SetMaterialPropertiesTable(PhotocathodeMPT_Ham20inch);

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

    // Acrylic
    density = 1.18*g/cm3;
    Acrylic = new G4Material("Acrylic", density,3);
    Acrylic->AddElement(C, 59.984*perCent);
    Acrylic->AddElement(H, 8.055*perCent);
    Acrylic->AddElement(O, 31.961*perCent);

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
    LSMPT->AddProperty("RAYLEIGH", GdLSRayEnergy, GdLSRayLength, 11);
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
    LSMPT->AddProperty("AlphaSLOWTIMECONSTANT", component, GdLSAlphaSlowTimeConstant,2);
    LSMPT->AddProperty("AlphaYIELDRATIO", component, GdLSAlphaYieldRatio,2);

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
}

void NeutrinoDetectorConstruction::ConstructSDandField()
{
    G4String name_LS_SD = "GdLS_log";
    auto Tracker_LS_SD = new MyTrackerSD(name_LS_SD, "TrackerHitCollection_LS");
    G4SDManager::GetSDMpointer()->AddNewDetector(Tracker_LS_SD);
    SetSensitiveDetector("GdLS_log", Tracker_LS_SD);

}

