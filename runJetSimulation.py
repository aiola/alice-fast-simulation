#!/usr/bin/env python

import ROOT
import argparse

def main(name, pythiaEvents, procStr, gen, seed, lhe, beamType, ebeam1, ebeam2, minPtHard, maxPtHard):
    # ROOT.gSystem.SetFPEMask(ROOT.TSystem.kInvalid | ROOT.TSystem.kDivByZero | ROOT.TSystem.kOverflow | ROOT.TSystem.kUnderflow)
    ROOT.gSystem.SetFPEMask(ROOT.TSystem.kNoneMask)

    ROOT.gInterpreter.AddIncludePath("$ALICE_ROOT/include")
    ROOT.gInterpreter.AddIncludePath("$ALICE_PHYSICS/include")
    ROOT.gInterpreter.AddIncludePath("$FASTJET/include")

    # load fastjet libraries 3.x
    ROOT.gSystem.Load("libCGAL")
    ROOT.gSystem.Load("libfastjet")
    ROOT.gSystem.Load("libsiscone")
    ROOT.gSystem.Load("libsiscone_spherical")
    ROOT.gSystem.Load("libfastjetplugins")
    ROOT.gSystem.Load("libfastjetcontribfragile")

    ROOT.gSystem.Load("libpythia6_4_28.so")
    ROOT.gROOT.ProcessLine(".L OnTheFlySimulationGenerator.cxx+g")

    if name:
        trainName = "FastSim_{0}".format(name)
    else:
        if ptHard < 0:
            trainName = "FastSim_{0}_{1}".format(gen, procStr)
        else:
            trainName = "FastSim_{0}_{1}_{2}".format(gen, procStr, ptHard)

    forceDecay = False
    if gen == "pythia":
        if lhe:
            print("An LHE file was provided ({0}) but PYTHIA was selected as generator. The LHE file will be ignored".format(lhe))
            lhe = ""
        proc = ROOT.kPyJets
        if procStr == "dijet":
            specialPart = ROOT.OnTheFlySimulationGenerator.kNoSpecialParticle
        elif procStr == "charm":
            specialPart = ROOT.OnTheFlySimulationGenerator.kccbar
        elif procStr == "beauty":
            specialPart = ROOT.OnTheFlySimulationGenerator.kbbbar
        else:
            print("Unknown process {}!".format(procStr))
            exit(1)
    elif gen == "powheg":
        if minPtHard >= 0 and maxPtHard >= 0:
            print("Pt hard bins are ignored for POWHEG")
            minPtHard = -1
            maxPtHard = -1
        if not lhe:
            print("Must provide an LHE file if POWHEG is selected as event generator!")
            exit(1)
        specialPart = ROOT.OnTheFlySimulationGenerator.kNoSpecialParticle
        if procStr == "dijet":
            proc = ROOT.kPyJetsPWHG
        elif procStr == "charm":
            proc = ROOT.kPyCharmPWHG
        elif procStr == "beauty":
            proc = ROOT.kPyBeautyPWHG
        else:
            print("Unknown process {}!".format(procStr))
            exit(1)

    sim = ROOT.OnTheFlySimulationGenerator(trainName)
    sim.SetNumberOfEvents(pythiaEvents)
    sim.SetProcess(proc)
    sim.SetSpecialParticle(specialPart)
    sim.SetForceHadronicDecay(forceDecay)
    sim.SetSeed(seed)
    sim.SetLHEFile(lhe)
    sim.SetEnergyBeam1(float(ebeam1))
    sim.SetEnergyBeam2(float(ebeam2))
    sim.SetPtHardRange(minPtHard, maxPtHard)
    if beamType == "pPb":
        sim.SetBeamType(ROOT.OnTheFlySimulationGenerator.kpPb)
    elif beamType == "pp":
        sim.SetBeamType(ROOT.OnTheFlySimulationGenerator.kpp)
    else:
        print("ERROR: Beam type {0} not recognized!! Not running...".format(beamType))
        exit(1)
    sim.Start()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Run jet simulation.')
    parser.add_argument('--numevents', metavar='NEVT',
                        default=50000, type=int)
    parser.add_argument('--gen', metavar='GEN',
                        default='pythia')
    parser.add_argument('--proc', metavar='PROC',
                        default='charm')
    parser.add_argument('--lhe', metavar='LHE',
                        default='')
    parser.add_argument('--seed', metavar='SEED',
                        default=0, type=int)
    parser.add_argument('--beam-type', metavar='BEAMTYPE',
                        default="pp")
    parser.add_argument('--ebeam1', metavar='EBEAM1',
                        default=3500)
    parser.add_argument('--ebeam2', metavar='EBEAM2',
                        default=3500)
    parser.add_argument('--name', metavar='NAME',
                        default='')
    parser.add_argument('--minpthard', metavar="MINPTHARD",
                        default=-1, type=float)
    parser.add_argument('--maxpthard', metavar='MAXPTHARD',
                        default=-1, type=float)
    args = parser.parse_args()

    main(args.name, args.numevents, args.proc, args.gen, args.seed, args.lhe, args.beam_type, args.ebeam1, args.ebeam2, args.minpthard, args.maxpthard)
