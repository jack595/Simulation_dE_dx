#include"../include/MytrackerHit.hh"
#include<iomanip>
#include"G4VVisManager.hh"
#include"G4Circle.hh"
#include"G4VisAttributes.hh"
#include "G4UnitsTable.hh"

G4ThreadLocal G4Allocator<MytrackerHit>* MyTrackerHitAllocator=0;

MytrackerHit::MytrackerHit()
    :G4VHit(),fTrackID(-1),fChamberNb(-1),fEdep(0.),fPos(G4ThreeVector()){
    m_XYZ_GoOutLS = std::vector<double> {0.,0., 0.};
}

MytrackerHit::MytrackerHit(const MytrackerHit & right)
    :G4VHit()
{
    fTrackID =right.fTrackID;
    fChamberNb =right.fChamberNb;
    fEdep=right.fEdep;
    fPos=right.fPos;
}

MytrackerHit::~MytrackerHit()
{

}	//if omit this definition will occur the situation:undefined reference to `vtable for MytrackerHit'



const MytrackerHit &MytrackerHit::operator=(const MytrackerHit &right)
{
    fTrackID   = right.fTrackID;
    fChamberNb = right.fChamberNb;
    fEdep      = right.fEdep;
    fPos       = right.fPos;

    return *this;
}

G4bool MytrackerHit::operator==(const MytrackerHit & right) const
{
    return ( this == &right ) ? true : false;
}

void MytrackerHit::Draw()
{
    G4VVisManager* pVVisManager =G4VVisManager::GetConcreteInstance();
    if(pVVisManager)
    {
        G4Circle circle(fPos);
        circle.SetScreenSize(4.);
        circle.SetFillStyle(G4Circle::filled);
        G4Color colour(1.,0.,0.);
        G4VisAttributes attribs(colour);
        circle.SetVisAttributes(attribs);
        pVVisManager->Draw(circle);
    }

}

void MytrackerHit::Print()
{
    G4cout<<" Particle: "<<fParticleName
       << "  trackID: " << fTrackID << " chamberNb: " << fChamberNb
       << "Edep: "
       << std::setw(7) << G4BestUnit(fEdep,"Energy")
       << " Position: "
       << std::setw(7) << G4BestUnit( fPos,"Length")
       << " Equench: "
       << std::setw(7) << G4BestUnit( fEquench,"Energy")
       << G4endl;

}




