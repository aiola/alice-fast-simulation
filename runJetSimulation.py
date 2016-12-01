#!/usr/bin/env python

import ROOT
import argparse

def main(pythiaEvents, procStr, gen, seed, lhe, name):
    #ROOT.gSystem.SetFPEMask(ROOT.TSystem.kInvalid | ROOT.TSystem.kDivByZero | ROOT.TSystem.kOverflow | ROOT.TSystem.kUnderflow)
    ROOT.gSystem.SetFPEMask(ROOT.TSystem.kNoneMask)

    ROOT.gInterpreter.AddIncludePath("$ALICE_ROOT/include")
    ROOT.gInterpreter.AddIncludePath("$ALICE_PHYSICS/include")
    ROOT.gInterpreter.AddIncludePath("$FASTJET/include")

    #load fastjet libraries 3.x
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
        trainName = "FastSim_{0}_{1}".format(gen, procStr)

    if gen == "pythia":
        if lhe:
            print("An LHE file was provided ({0}) but PYTHIA was selected as generator. The LHE file will be ignored".format(lhe))
            lhe = ""
        if procStr == "dijet":
            proc = ROOT.kPyJets
            forceDecay = False
            specialPart = ROOT.OnTheFlySimulationGenerator.kNoSpecialParticle
        elif procStr == "charm":
            proc = ROOT.kPyCharmppMNRwmi
            forceDecay = False
            specialPart = ROOT.OnTheFlySimulationGenerator.kccbar
        elif procStr == "beauty":
            proc = ROOT.kPyBeautyppMNRwmi
            forceDecay = False
            specialPart = ROOT.OnTheFlySimulationGenerator.kbbbar
    elif gen == "powheg":
        if not lhe:
            print("Must provide an LHE file if POWHEG is selected as event generator!")
            exit(1)
        if procStr == "dijet":
            proc = ROOT.kPyJetsPWHG
            forceDecay = False
            specialPart = ROOT.OnTheFlySimulationGenerator.kNoSpecialParticle
        elif procStr == "charm":
            proc = ROOT.kPyCharmPWHG
            forceDecay = False
            specialPart = ROOT.OnTheFlySimulationGenerator.kNoSpecialParticle
        elif procStr == "beauty":
            proc = ROOT.kPyBeautyPWHG
            forceDecay = False
            specialPart = ROOT.OnTheFlySimulationGenerator.kNoSpecialParticle

    sim = ROOT.OnTheFlySimulationGenerator(trainName)
    sim.SetNumberOfEvents(pythiaEvents)
    sim.SetProcess(proc)
    sim.SetSpecialParticle(specialPart)
    sim.SetForceHadronicDecay(forceDecay)
    sim.SetSeed(seed)
    sim.SetLHEFile(lhe)
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
    parser.add_argument('--name', metavar='NAME',
                        default='')
    args = parser.parse_args()

    main(args.numevents, args.proc, args.gen, args.seed, args.lhe, args.name)
