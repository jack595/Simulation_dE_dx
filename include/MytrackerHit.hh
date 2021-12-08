#pragma once
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include"G4ThreadLocalSingleton.hh"
#include "tls.hh"

class MytrackerHit:public G4VHit
{
public:
    MytrackerHit();
    MytrackerHit(const MytrackerHit&);
    virtual ~MytrackerHit();

    // operators
    const MytrackerHit& operator=(const MytrackerHit&);
    G4bool operator==(const MytrackerHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    virtual void Draw();
    virtual void Print();

    // Set methods
    void SetTrackID  (G4int track)      { fTrackID = track; };
    void SetChamberNb(G4int chamb)      { fChamberNb = chamb; };
    void SetEdep     (G4double de)      { fEdep = de; };
    void SetPos      (G4ThreeVector xyz){ fPos = xyz; };
    void SetParticleName(G4String particleName)
                 { fParticleName=particleName; };
    void SetPDGID(G4int pdgID) { fpdgID=pdgID;};

    // Get methods
    G4int GetTrackID() const     { return fTrackID; };
    G4int GetChamberNb() const   { return fChamberNb; };
    G4double GetEdep() const     { return fEdep; };
    G4ThreeVector GetPos() const { return fPos; };
    G4int  GetPdgID() {return  fpdgID;};

//    bool WhetherHit

    public:

      G4int         fTrackID;
      G4int         fChamberNb;
      G4double      fEdep;
      G4double      fEquench;
      G4ThreeVector fPos;
      G4double      fStepLength;
      G4String      fParticleName;
      G4int         fpdgID;
      G4double      time;

};

typedef  G4THitsCollection<MytrackerHit> MyTrackerHitsCollection; //this is a vector

extern G4ThreadLocal G4Allocator<MytrackerHit>* MyTrackerHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* MytrackerHit::operator new(size_t)
{
  if(!MyTrackerHitAllocator)
      MyTrackerHitAllocator = new G4Allocator<MytrackerHit>;
  return (void *) MyTrackerHitAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void MytrackerHit::operator delete(void *hit)
{
  MyTrackerHitAllocator->FreeSingle((MytrackerHit*) hit);
}

