//
// Created by luoxj@ihep.ac.cn on 2021/12/8.
//

#ifndef SIM_DE_DX_EXP_JUNOTOOL_HH
#define SIM_DE_DX_EXP_JUNOTOOL_HH
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4Gamma.hh"
#include "G4LossTableManager.hh"
#include "G4Electron.hh"
#include "G4SystemOfUnits.hh"

double EdepToQquench(G4Step* step)
{
    // Ref:
    //   - Scintillation, Birks Law

    G4double m_BirksConstant1 = 6.5e-3*g/cm2/MeV;
    G4double m_BirksConstant2 = 1.5e-6*(g/cm2/MeV)*(g/cm2/MeV);

    double QuenchedTotalEnergyDeposit = 0.;
    double dE = step->GetTotalEnergyDeposit();
    double dx = step->GetStepLength();

    G4Track *aTrack = step->GetTrack();
    G4ParticleDefinition* aParticle = aTrack->GetDefinition();

    // Find quenched energy deposit.
    if(dE > 0) {
        if(aParticle == G4Gamma::Gamma()) // It is a gamma
        {
            G4LossTableManager* manager = G4LossTableManager::Instance();
            dx = manager->GetRange(G4Electron::Electron(), dE, aTrack->GetMaterialCutsCouple());
            //info()<<"dE_dx = "<<dE/dx/(MeV/mm)<<"MeV/mm"<<endreq;
        }
        G4Material* aMaterial = step->GetPreStepPoint()->GetMaterial();
        G4MaterialPropertiesTable* aMaterialPropertiesTable =
                aMaterial->GetMaterialPropertiesTable();
        if (aMaterialPropertiesTable) {

            // There are some properties. Is there a scintillator property?
            const G4MaterialPropertyVector* Fast_Intensity =
                    aMaterialPropertiesTable->GetProperty("FASTCOMPONENT");
            const G4MaterialPropertyVector* Slow_Intensity =
                    aMaterialPropertiesTable->GetProperty("SLOWCOMPONENT");
            if (Fast_Intensity || Slow_Intensity ) {
                // It's a scintillator.
                double delta = dE/dx/aMaterial->GetDensity();
                //double birk1 = 0.0125*g/cm2/MeV;
                double birk1 = m_BirksConstant1;
                if(aTrack->GetDefinition()->GetPDGCharge()>1.1)//for particle charge greater than 1.
                    birk1 = 0.57*birk1;
                //double birk2 = (0.0031*g/MeV/cm2)*(0.0031*g/MeV/cm2);
                double birk2 = m_BirksConstant2;
                QuenchedTotalEnergyDeposit = dE /(1+birk1*delta+birk2*delta*delta);
            }
        }
    }
    std::cout<< "Quench Energy:\t"<< QuenchedTotalEnergyDeposit<< std::endl;
    return QuenchedTotalEnergyDeposit;

}

#endif //SIM_DE_DX_EXP_JUNOTOOL_HH
