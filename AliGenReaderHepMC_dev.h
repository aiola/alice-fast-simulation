#ifndef ALIGENREADERHEPMC_DEV_H
#define ALIGENREADERHEPMC_DEV_H
// Realisations of the AliGenReader interface to be used with AliGenExFile.
// NextEvent() loops over events
// and NextParticle() loops over particles.
// This implementation reads HepMC output formats
// Author: brian.peter.thorsbro@cern.ch, brian@thorsbro.dk
// Based on AliGenReaderSL by andreas.morsch@cern.ch

#include <TClonesArray.h>

#include <AliGenEventHeader.h>
#include <AliGenReader.h>

#if !(defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__))
#include <THepMCParser.h>
namespace HepMC {
class IO_BaseClass;
class GenEvent;
}

#endif

class TParticle;

class AliGenReaderHepMC_dev : public AliGenReader {
public:
    AliGenReaderHepMC_dev();
    AliGenReaderHepMC_dev(const AliGenReaderHepMC_dev& reader);
    virtual ~AliGenReaderHepMC_dev();
    AliGenEventHeader* GetGenEventHeader() const { return fGenEventHeader; };
    virtual void Init();
    virtual Int_t NextEvent();
    virtual TParticle* NextParticle();
    virtual void RewindEvent();
    AliGenReaderHepMC_dev& operator=(const AliGenReaderHepMC_dev& rhs);

protected:
#if !(defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__))
    HepMC::IO_BaseClass* fEventsHandle; //! pointer to the HepMC file handler
    HepMC::GenEvent* fGenEvent; //! pointer to a generated event
#endif
    TClonesArray* fParticleArray; //! pointer to array containing particles of current event
    TIter* fParticleIterator; //! iterator coupled to the array
    AliGenEventHeader* fGenEventHeader; //! AliGenEventHeader

private:
    void Copy(TObject&) const;

    ClassDef(AliGenReaderHepMC_dev, 1) //Generate particles from external file
};
#endif
