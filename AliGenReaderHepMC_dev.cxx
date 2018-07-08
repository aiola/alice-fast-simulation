#include <TDatabasePDG.h>
#include <TParticle.h>
#include <TVirtualMC.h>

#include <AliGenPythiaEventHeader.h>
#include <AliLog.h>
#include <AliRun.h>
#include <AliStack.h>

#include "HepMC/GenEvent.h"
#include "HepMC/IO_BaseClass.h"
#include "HepMC/IO_GenEvent.h"

#include "THepMCParser_dev.h"
#include "AliGenReaderHepMC_dev.h"

ClassImp(AliGenReaderHepMC_dev)

    AliGenReaderHepMC_dev::AliGenReaderHepMC_dev()
    : fEventsHandle(0)
    , fGenEvent(0)
    , fParticleArray(0)
    , fParticleIterator(0)
    , fGenEventHeader(0)
{
    ;
}

AliGenReaderHepMC_dev::AliGenReaderHepMC_dev(const AliGenReaderHepMC_dev& reader)
    : AliGenReader(reader)
    , fEventsHandle(0)
    , fGenEvent(0)
    , fParticleArray(0)
    , fParticleIterator(0)
    , fGenEventHeader(0)
{
    reader.Copy(*this);
}

AliGenReaderHepMC_dev& AliGenReaderHepMC_dev::operator=(const AliGenReaderHepMC_dev& rhs)
{
    // Assignment operator
    rhs.Copy(*this);
    return *this;
}

AliGenReaderHepMC_dev::~AliGenReaderHepMC_dev()
{
    delete fEventsHandle;
    delete fGenEvent;
    delete fParticleArray;
    delete fParticleIterator;
} // not deleting fGenEventHeader as it is returned out

void AliGenReaderHepMC_dev::Copy(TObject&) const
{
    //
    // Copy
    //
    Fatal("Copy", "Not implemented!\n");
}

void AliGenReaderHepMC_dev::Init()
{
    // check if file exists
    if (access(fFileName, R_OK) != 0)
        AliError(Form("Couldn't open input file: %s", fFileName));

    // Initialisation
    fEventsHandle = new HepMC::IO_GenEvent(fFileName, std::ios::in);
    fParticleArray = new TClonesArray("TParticle");
    fParticleIterator = new TIter(fParticleArray);
}

Int_t AliGenReaderHepMC_dev::NextEvent()
{
    // Clean memory
    if (fGenEvent) delete fGenEvent;
    // Read the next event
    if ((fGenEvent = fEventsHandle->read_next_event())) {
        std::string err_mess = THepMCParser_dev::ParseGenEvent2TCloneArray(fGenEvent, fParticleArray, "GEV", "MM", false);
        if (!err_mess.empty()) {
            AliErrorStream() << "Unable to parse event: " << err_mess << std::endl <<
            "Skipping to the next event." << std::endl;
            return NextEvent();
        }
        fParticleIterator->Reset();
        THepMCParser_dev::HeavyIonHeader_t heavyIonHeader;
        THepMCParser_dev::PdfHeader_t pdfHeader;
        THepMCParser_dev::ParseGenEvent2HeaderStructs(fGenEvent, heavyIonHeader, pdfHeader, true, true);

        // Here I am going to do a hack. I need an event header that can hold information such as cross section and pt hard
        AliGenPythiaEventHeader* pythia_header = new AliGenPythiaEventHeader("HepMC");
        fGenEventHeader = pythia_header;
        // propagate the event weight from HepMC to the event header
        HepMC::WeightContainer weights = fGenEvent->weights();
        if (!weights.empty()) pythia_header->SetEventWeight(weights.front());
        HepMC::GenCrossSection* cross_section = fGenEvent->cross_section();
        if (cross_section) {
            pythia_header->SetXsection(cross_section->cross_section() / 1.0e9); // pb -> mb
            AliDebugStream(2) << "Cross section is " << cross_section->cross_section() << " pb" << std::endl;
        }
        else {
            AliDebugStream(2) << "Cross section object not valid." << std::endl;
        }
        pythia_header->SetPtHard(fGenEvent->event_scale()); // careful here: using the "event scale" instead of the pt hard
        pythia_header->SetTrials(1);
        AliDebugStream(1) << "Parsed event " << fGenEvent->event_number() << " with " << fGenEvent->particles_size() << " particles, weight = " << fGenEventHeader->EventWeight() << std::endl;

        // convert time from [mm/c] to [s]
        const Double_t conv = 0.0001 / 2.99792458e8;

        fParticleIterator->Reset();
        Int_t npart = fParticleArray->GetEntries();
        for (Int_t i = 0; i < npart; i++) {
            TParticle* particle = (TParticle*)fParticleIterator->Next();
            particle->SetProductionVertex(particle->Vx(), particle->Vy(), particle->Vz(), particle->T() * conv);
        }
        fParticleIterator->Reset();

        return npart;
    }
    AliWarningStream() << "No more events in the file." << std::endl;
    return 0;
}

TParticle* AliGenReaderHepMC_dev::NextParticle()
{
    // Read next particle

    TParticle* particle = (TParticle*)fParticleIterator->Next();
    if (particle && particle->GetStatusCode() == 1) {
        particle->SetBit(kTransportBit);
    }
    return particle;
}

void AliGenReaderHepMC_dev::RewindEvent()
{
    fParticleIterator->Reset();
}
