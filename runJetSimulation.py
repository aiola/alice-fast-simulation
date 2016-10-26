#!/usr/bin/env python

import ROOT
import argparse

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

parser = argparse.ArgumentParser(description='Run fast simulation.')
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
args = parser.parse_args()

if args.gen == "pythia":
    if args.proc == "dijet":
        proc = ROOT.kPyJets
        trainName = "FastSim_PyJets"
        forceDecay = False
        specialPart = ROOT.AliFastSimulationTask.kNoSpecialParticle
    elif args.proc == "charm":
        proc = ROOT.kPyCharm
        trainName = "FastSim_PyCharm"
        forceDecay = True
        specialPart = ROOT.AliFastSimulationTask.kccbar
    elif args.proc == "beauty":
        proc = ROOT.kPyBeauty
        trainName = "FastSim_PyBeauty"
        forceDecay = True
        specialPart = ROOT.AliFastSimulationTask.kbbbar
elif args.gen == "powheg":
    if args.proc == "dijet":
        proc = ROOT.kPyJetsPWHG
        trainName = "FastSim_PyJets"
        forceDecay = False
        specialPart = ROOT.AliFastSimulationTask.kNoSpecialParticle
    elif args.proc == "charm":
        proc = ROOT.kPyCharmPWHG
        trainName = "FastSim_PyCharm"
        forceDecay = True
        specialPart = ROOT.AliFastSimulationTask.kccbar
    elif args.proc == "beauty":
        proc = ROOT.kPyBeautyPWHG
        trainName = "FastSim_PyBeauty"
        forceDecay = True
        specialPart = ROOT.AliFastSimulationTask.kbbbar

ROOT.runJetSimulation(args.runtype, args.gridmode, args.numevents, proc, specialPart, forceDecay, trainName)
