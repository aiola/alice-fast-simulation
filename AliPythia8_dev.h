#ifndef ALIPYTHIA8_DEV_H
#define ALIPYTHIA8_DEV_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include "AliPythiaBase_dev.h"
#include "AliTPythia8.h"

class AliPythia8_dev : public AliTPythia8, public AliPythiaBase_dev
{

 public:
    AliPythia8_dev();
    virtual ~AliPythia8_dev() {;}

    virtual Int_t Version() {return (8);}
    // convert to compressed code and print result (for debugging only)

    virtual Int_t CheckedLuComp(Int_t /*kf*/) {return -1;}

    // Pythia initialisation for selected processes
    virtual void  ProcInit (Process_t process, Float_t energy, Int_t strucfunc, Int_t tune);

    virtual void  SetSeed(UInt_t seed);
    virtual void  GenerateEvent();
    virtual Int_t GetNumberOfParticles() {return GetN();}
    virtual void  PrintStatistics();
    virtual void  EventListing();    
    virtual Int_t GetParticles(TClonesArray *particles) {return ImportParticles(particles, "All");}

    virtual void  SetDecayOff(const std::set<int>& pdg_codes);

    virtual void SetLHEFile(const char* fname) { fLHEFile = fname; }

    // Treat protons as inside nuclei
    virtual void  SetNuclei(Int_t /*a1*/, Int_t /*a2*/, Int_t /*pdg*/);

    // Print particle properties
    virtual void PrintParticles();

    // Common Physics Configuration
    virtual void SetPtHardRange(Float_t ptmin, Float_t ptmax);
    virtual void SetYHardRange(Float_t ymin, Float_t ymax);
    virtual void SetInitialAndFinalStateRadiation(Int_t flag1, Int_t flag2);

    // Common Getters
    virtual void    GetXandQ(Float_t& x1, Float_t& x2, Float_t& q);
    virtual Float_t GetXSection();
    virtual Float_t GetPtHard();
    virtual Int_t GetNMPI() { return fLastNMPI; }
    virtual Int_t GetNSuperpositions() { return fLastNSuperposition; }

    virtual Int_t ProcessCode();

 protected:
    Process_t             fProcess;           // Process type
    Int_t                 fItune;
    Float_t               fEcms;              // Centre of mass energy
    Int_t                 fStrucFunc;         // Structure function
    TString               fLHEFile;         //

 private:
    AliPythia8_dev(const AliPythia8_dev& pythia);  // not implemented
    AliPythia8_dev & operator=(const AliPythia8_dev & rhs);  // not implemented

    ClassDef(AliPythia8_dev, 1); //ALICE UI to PYTHIA8
};

#endif





