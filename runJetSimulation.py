#!/usr/bin/env python

import ROOT
import argparse

def main(pythiaEvents, procStr, gen, seed, lhe, name):
    ROOT.gSystem.SetFPEMask(ROOT.TSystem.kInvalid | ROOT.TSystem.kDivByZero | ROOT.TSystem.kOverflow | ROOT.TSystem.kUnderflow)

    ROOT.gInterpreter.AddIncludePath("$ALICE_ROOT/include")
    ROOT.gInterpreter.AddIncludePath("$ALICE_PHYSICS/include")
    ROOT.gInterpreter.AddIncludePath("$FASTJET/include")
    ROOT.gInterpreter.AddIncludePath("/opt/alicesw/RooUnfold/src")
        
    #load fastjet libraries 3.x
    ROOT.gSystem.Load("libCGAL")
        
    ROOT.gSystem.Load("libfastjet")
    ROOT.gSystem.Load("libsiscone")
    ROOT.gSystem.Load("libsiscone_spherical")
    ROOT.gSystem.Load("libfastjetplugins")
    ROOT.gSystem.Load("libfastjetcontribfragile")
    
    ROOT.gSystem.Load("libpythia6_4_28.so")
    ROOT.gROOT.ProcessLine(".L AliAnalysisTaskSEhfcjMCanalysis.cxx+g")
    ROOT.gROOT.ProcessLine(".L runJetSimulation.C+g")

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
            specialPart = ROOT.kNoSpecialParticle
        elif procStr == "charm":
            proc = ROOT.kPyCharm
            forceDecay = False
            specialPart = ROOT.kccbar
        elif procStr == "beauty":
            proc = ROOT.kPyBeauty
            forceDecay = False
            specialPart = ROOT.kbbbar
    elif gen == "powheg":
        if not lhe:
            print("Must provide an LHE file if POWHEG is selected as event generator!")
            exit(1)
        if procStr == "dijet":
            proc = ROOT.kPyJetsPWHG
            forceDecay = False
            specialPart = ROOT.kNoSpecialParticle
        elif procStr == "charm":
            proc = ROOT.kPyCharmPWHG
            forceDecay = False
            specialPart = ROOT.kNoSpecialParticle
        elif procStr == "beauty":
            proc = ROOT.kPyBeautyPWHG
            forceDecay = False
            specialPart = ROOT.kNoSpecialParticle

    ROOT.runJetSimulation(pythiaEvents, proc, specialPart, forceDecay, trainName, seed, lhe)

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
