#pragma once

#include"G4VSensitiveDetector.hh"
#include"MytrackerHit.hh"
#include"vector"

class G4Step;
class G4HCofThisEvent;


class MyTrackerSD:public G4VSensitiveDetector
{
public:
    MyTrackerSD(const G4String& name,const G4String& hitsCollectionName);
    virtual ~MyTrackerSD();

    //methods from base class
    virtual void Initialize(G4HCofThisEvent* hitsCollection);
    virtual G4bool ProcessHits(G4Step * step, G4TouchableHistory * history);
    virtual void EndOfEvent(G4HCofThisEvent* hitCollection);

private:
    MyTrackerHitsCollection* fHitsCollection;
};

