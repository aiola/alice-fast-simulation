#ifndef ALIPYTHIABASE_DEV_H
#define ALIPYTHIABASE_DEV_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include <TObject.h>

#include <AliRndm.h>
#include <AliStructFuncType.h>
#include <PythiaProcesses.h>

class AliFastGlauber;
class AliQuenchingWeights;
class AliStack;
class TClonesArray;

class AliPythiaBase_dev : public AliRndm
{

 public:
    AliPythiaBase_dev() {;}
    virtual ~AliPythiaBase_dev() {;}
    virtual Int_t Version() = 0;

    // Convert to compressed code and print result (for debugging only)
    virtual Int_t CheckedLuComp(Int_t /*kf*/) = 0;

    // Pythia initialisation for selected processes
    virtual void  ProcInit (Process_t /*process*/, Float_t /*energy*/, StrucFunc_t /*strucfunc*/, Int_t /* tune */) = 0;

    virtual void  SetSeed(UInt_t /*seed*/);
    virtual void  GenerateEvent() = 0;
    virtual Int_t GetNumberOfParticles() = 0;
    virtual void  EditEventList(Int_t /*i*/);
    virtual Int_t GetParticles(TClonesArray */*particles*/) = 0;
    virtual void  PrintStatistics() = 0;
    virtual void  EventListing() = 0;

    // Treat protons as inside nuclei
    virtual void  SetNuclei(Int_t /*a1*/, Int_t /*a2*/, Int_t /*pdf*/) = 0;

    // Print particle properties
    virtual void PrintParticles() = 0;

    virtual void SetLHEFile(const char* /*fname*/) = 0;

    // Common Physics Configuration
    virtual void SetWeightPower(Double_t /*p*/);
    virtual void SetPtHardRange(Float_t /*ptmin*/, Float_t /*ptmax*/) = 0;
    virtual void SetYHardRange(Float_t /*ymin*/, Float_t /*ymax*/) = 0;
    virtual void SetInitialAndFinalStateRadiation(Int_t /*flag1*/, Int_t /*flag2*/) = 0;

    // Common Getters
    virtual Float_t GetEventWeight() { return 1; }
    virtual Float_t GetXSection() = 0;
    virtual Int_t   ProcessCode() = 0;
    virtual Float_t GetPtHard() = 0;
    virtual Int_t   GetNMPI() {return -1;}
    virtual Int_t   GetNSuperpositions() { return 1; }

    ClassDef(AliPythiaBase_dev, 0) //ALICE UI to PYTHIA
};

#endif





