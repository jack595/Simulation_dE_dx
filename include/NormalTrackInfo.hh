//
// Created by luoxj@ihep.ac.cn on 2022/7/4.
//

#ifndef NormalTrackInfo_hh
#define NormalTrackInfo_hh


#include "globals.hh"
#include "G4VUserTrackInformation.hh"

#include "G4Track.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include <vector>
#include <map>

// Ref: http://geant4.slac.stanford.edu/Tips/

class NormalTrackInfo: public G4VUserTrackInformation {
public:
    NormalTrackInfo();
    NormalTrackInfo(const G4Track* aTrack);
    NormalTrackInfo(const NormalTrackInfo* aTrackInfo);
    virtual ~NormalTrackInfo();


    inline void *operator new(size_t);
    inline void operator delete(void *aTrackInfo);
    inline int operator ==(const NormalTrackInfo& right) const {
        return (this==&right);
    }
public:
    inline G4int GetOriginalTrackID() const {return originalTrackID;}

    inline void markTracedAncestors(G4int trkid) { tracedAncestors.push_back(trkid); }
    inline const std::vector<G4int>& getTracedAncestors() const { return tracedAncestors; }

    inline void setMichaelElectron() {phys_michael_electron = 1;}
    inline G4int isMichaelElectron() {return phys_michael_electron;}

    inline void setNeutronCapture() {phys_neutron_capture = 1;}
    inline G4int isNeutronCapture() {return phys_neutron_capture;}

    // In the G4, all the decay physics should be implemented in G4Decay.
    // If we need to store the different decay mode, then define the number schema.
    // By default, we use mode 0 for no decay, 1 for general decay.
    // It could be easily extented by defining different numbers.
    inline void setDecayMode(int mode=1) { phys_decay_mode = mode; }
    inline G4int getDecayMode()  { return phys_decay_mode; }

    inline void setFromCerenkov() {from_cerenkov = true;}
    inline G4bool isFromCerenkov(){return from_cerenkov;}

    inline void setReemission() {is_reemission = true;}
    inline G4bool isReemission(){return is_reemission;}


    inline void setOriginalOP() { m_op_is_original_op = true; }
    inline G4bool isOriginalOP() { return m_op_is_original_op; }
    inline void setOriginalOPStartTime(double t) {m_op_start_time = t;}
    inline double getOriginalOPStartTime() { return m_op_start_time; }

    inline void setBoundaryPos(const G4ThreeVector& pos) { pos_at_boundary = pos; }
    inline const G4ThreeVector& getBoundaryPos() const { return pos_at_boundary; }

    inline void setParentName(const G4String& parent_name) { m_parent_name = parent_name; }
    inline const G4String& getParentName() const { return m_parent_name; }

    inline void setXYZGoOutLS(const std::vector<double>& xyz){m_XYZ_GoOutLS=xyz;}
    inline std::vector<double> getXYZGoOutLS(){return m_XYZ_GoOutLS;}

private:
    G4int originalTrackID;
    std::vector<G4int> tracedAncestors;

    // special physics related
    G4int phys_michael_electron;
    G4int phys_neutron_capture;
    G4int phys_decay_mode;

    G4bool from_cerenkov;
    G4bool is_reemission;

    // the original OP info
    // Here, original means the parent of OP is not OP.
    G4bool m_op_is_original_op;
    G4double m_op_start_time;

    G4ThreeVector pos_at_boundary;

    // info of Parent Track
    G4String m_parent_name;

    std::vector<double> m_XYZ_GoOutLS;
};

extern G4ThreadLocal G4Allocator<NormalTrackInfo>* aNormalTrackInformationAllocator;


inline void* NormalTrackInfo::operator new(size_t)
{
    void* aTrackInfo;
    if (!aNormalTrackInformationAllocator) {
        aNormalTrackInformationAllocator = new G4Allocator<NormalTrackInfo>;
    }
    aTrackInfo = (void*)aNormalTrackInformationAllocator->MallocSingle();
    return aTrackInfo;
}

inline void NormalTrackInfo::operator delete(void *aTrackInfo)
{ aNormalTrackInformationAllocator->FreeSingle((NormalTrackInfo*)aTrackInfo);}

#endif


