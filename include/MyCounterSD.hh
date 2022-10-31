#pragma once

#include"G4VSensitiveDetector.hh"
#include"vector"
#include "TString.h"
#include "TTree.h"

class G4Step;
class G4HCofThisEvent;


class MyCounterSD:public G4VSensitiveDetector
{
public:
    MyCounterSD(const G4String& name,const G4String& hitsCollectionName);
    ~MyCounterSD() override;

    //methods from base class
    void Initialize(G4HCofThisEvent* hitsCollection) override;
    G4bool ProcessHits(G4Step * step, G4TouchableHistory * history) override;
    void EndOfEvent(G4HCofThisEvent* hitCollection) override;
    static void WriteTTree();

//    static int seed_SD;

private:
    std::vector<unsigned long int> v_n_hits;
    TString name_SDHitsCollection;
    static std::map<TString, TTree*> map_name2TTree;
};

