#!/usr/bin/env python

import ROOT
import argparse

def main(runtype, gridmode, pythiaEvents, procStr, gen, seed):
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
    ROOT.gROOT.ProcessLine(".L AliFastSimulationTask.cxx+g")
    ROOT.gROOT.ProcessLine(".L runJetSimulation.C+g")

    if gen == "pythia":
        if procStr == "dijet":
            proc = ROOT.kPyJets
            trainName = "FastSim_PyJets"
            forceDecay = False
            specialPart = ROOT.AliFastSimulationTask.kNoSpecialParticle
        elif procStr == "charm":
            proc = ROOT.kPyCharm
            trainName = "FastSim_PyCharm"
            forceDecay = False
            specialPart = ROOT.AliFastSimulationTask.kccbar
        elif procStr == "beauty":
            proc = ROOT.kPyBeauty
            trainName = "FastSim_PyBeauty"
            forceDecay = False
            specialPart = ROOT.AliFastSimulationTask.kbbbar
    elif gen == "powheg":
        if procStr == "dijet":
            proc = ROOT.kPyJetsPWHG
            trainName = "FastSim_PyJetsPWHG"
            forceDecay = False
            specialPart = ROOT.AliFastSimulationTask.kNoSpecialParticle
        elif procStr == "charm":
            proc = ROOT.kPyCharmPWHG
            trainName = "FastSim_PyCharmPWHG"
            forceDecay = False
            specialPart = ROOT.AliFastSimulationTask.kNoSpecialParticle
        elif procStr == "beauty":
            proc = ROOT.kPyBeautyPWHG
            trainName = "FastSim_PyBeautyPWHG"
            forceDecay = False
            specialPart = ROOT.AliFastSimulationTask.kNoSpecialParticle

    ROOT.runJetSimulation(runtype, gridmode, pythiaEvents, proc, specialPart, forceDecay, trainName, seed)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Run jet simulation.')
    parser.add_argument('--runtype', metavar='RUNTYPE',
                        default='local')
    parser.add_argument('--gridmode', metavar='GRIDMODE',
                        default='offline')
    parser.add_argument('--numevents', metavar='NEVT',
                        default=50000, type=int)
    parser.add_argument('--gen', metavar='GEN',
                        default='pythia')
    parser.add_argument('--proc', metavar='PROC',
                        default='charm')
    parser.add_argument('--seed', metavar='SEED',
                        default=0, type=int)
    args = parser.parse_args()

    main(args.runtype, args.gridmode, args.numevents, args.proc, args.gen, args.seed)
