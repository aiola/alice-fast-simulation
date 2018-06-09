#ifndef ALIGENEXTFILE_DEV_H
#define ALIGENEXTFILE_DEV_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// Event generator that can read events from a files.
// The reading is performed by a realisation of AliGenReader specific to the file format.
// Author: andreas.morsch@cern.ch

#include <AliGenMC.h>

class AliGenReader;

class TTree;

class AliGenExtFile_dev : public AliGenMC {
public:
    AliGenExtFile_dev();
    AliGenExtFile_dev(Int_t npart);
    virtual ~AliGenExtFile_dev();
    // Initialise
    virtual void Init();
    // generate event
    virtual void Generate();
    void SetReader(AliGenReader* reader) { fReader = reader; }
    void SetStartEvent(Int_t startEvent) { fStartEvent = startEvent; }
    void SetFileName(const char* name) { fFileName = name; }
    AliGenReader* Reader() const { return fReader; }

protected:
    void CdEventFile();
    AliGenReader* fReader; //! Reader to read the file
    Int_t fStartEvent; //! Start event number
    void     InhibitAllTasks();
    TString fFileName;

private:
    AliGenExtFile_dev(const AliGenExtFile_dev& ext);
    AliGenExtFile_dev& operator=(const AliGenExtFile_dev& rhs);

    ClassDef(AliGenExtFile_dev, 1) //Generate particles from external file
};
#endif
