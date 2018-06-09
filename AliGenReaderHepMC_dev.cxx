#include <TDatabasePDG.h>
#include <TParticle.h>
#include <TVirtualMC.h>

#include <AliGenHepMCEventHeader.h>
#include <AliLog.h>
#include <AliRun.h>
#include <AliStack.h>

#include "HepMC/GenEvent.h"
#include "HepMC/IO_BaseClass.h"
#include "HepMC/IO_GenEvent.h"

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
        THepMCParser::ParseGenEvent2TCloneArray(fGenEvent, fParticleArray, "GEV", "MM", false);
        fParticleIterator->Reset();
        THepMCParser::HeavyIonHeader_t heavyIonHeader;
        THepMCParser::PdfHeader_t pdfHeader;
        THepMCParser::ParseGenEvent2HeaderStructs(fGenEvent, heavyIonHeader, pdfHeader, true, true);
        fGenEventHeader = new AliGenHepMCEventHeader(
            heavyIonHeader.Ncoll_hard,
            heavyIonHeader.Npart_proj,
            heavyIonHeader.Npart_targ,
            heavyIonHeader.Ncoll,
            heavyIonHeader.spectator_neutrons,
            heavyIonHeader.spectator_protons,
            heavyIonHeader.N_Nwounded_collisions,
            heavyIonHeader.Nwounded_N_collisions,
            heavyIonHeader.Nwounded_Nwounded_collisions,
            heavyIonHeader.impact_parameter,
            heavyIonHeader.event_plane_angle,
            heavyIonHeader.eccentricity,
            heavyIonHeader.sigma_inel_NN,
            pdfHeader.id1,
            pdfHeader.id2,
            pdfHeader.pdf_id1,
            pdfHeader.pdf_id2,
            pdfHeader.x1,
            pdfHeader.x2,
            pdfHeader.scalePDF,
            pdfHeader.pdf1,
            pdfHeader.pdf2);
        // propagate the event weight from HepMC to the event header
        HepMC::WeightContainer weights = fGenEvent->weights();
        if (!weights.empty()) fGenEventHeader->SetEventWeight(weights.front());
        AliDebug(1, Form("Parsed event %d with %d particles, weight = %e", fGenEvent->event_number(), fGenEvent->particles_size(), fGenEventHeader->EventWeight()));

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
