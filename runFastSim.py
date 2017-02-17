#!/usr/bin/env python

import time
import datetime
import platform
import os
import shutil
import subprocess
import argparse
import random
import sys

def main(pythiaEvents, gen, proc, qmass, facscfact, renscfact, lhans, beamType, ebeam1, ebeam2, nPDFset, nPDFerrSet, LHEfile, pthard, grid):
    print("------------------ job starts ---------------------")
    dateNow = datetime.datetime.now()
    print(dateNow)

    try:
        rootPath = subprocess.check_output(["which", "root"]).rstrip()
        alirootPath = subprocess.check_output(["which", "aliroot"]).rstrip()
        alienPath = subprocess.check_output(["which", "alien-token-info"]).rstrip()
    except subprocess.CalledProcessError:
        print "Environment is not configured correctly!"
        exit()

    print "Root: " + rootPath
    print "AliRoot: " + alirootPath
    print "Alien: " + alienPath

    if qmass < 0:
        if proc == "charm": qmass = 1.5
        elif proc == "beauty": qmass = 4.75

    if grid:
        fname = "{0}_{1}".format(gen, proc)
    else:
        unixTS = int(time.time())
        fname = "{0}_{1}_{2}".format(gen, proc, unixTS)

    print("Running {0} MC production on: {1}".format(proc, " ".join(platform.uname())))

    if gen == "powheg":
        if os.path.isfile("pwgevents.lhe") or os.path.isfile("powheg.input") or os.path.isfile("powheg.log"):
            print("Before running POWHEG again you must delete or move the following files: pwgevents.lhe, powheg.input, powheg.log")
            exit(1)

        if LHEfile:
            print("Using previously generated POWHEG events from file {0}!".format(LHEfile))
            runPOWHEG = False
        else:
            runPOWHEG = True
    else:
        runPOWHEG = False

    if runPOWHEG:
        print("Running new POWHEG simulation!")
        if proc == "dijet":
            powhegExe = "pwhg_main_dijet"
        elif proc == "charm":
            powhegExe = "pwhg_main_hvq"
        elif proc == "beauty":
            powhegExe = "pwhg_main_hvq"

        if not grid:
            powhegExe = "./POWHEG_bins/{0}".format(powhegExe)

        powhegEvents = int(pythiaEvents * 1.1)
        shutil.copy("{0}-powheg.input".format(proc), "powheg.input")
        rnd = random.randint(0, 1073741824)  # 2^30

        with open("powheg.input", "a") as myfile:
            myfile.write("iseed {0}\n".format(rnd))
            myfile.write("numevts {0}\n".format(powhegEvents))
            myfile.write("qmass {0}\n".format(qmass))
            myfile.write("facscfact {0}\n".format(facscfact))
            myfile.write("renscfact {0}\n".format(renscfact))
            myfile.write("lhans1 {0}\n".format(lhans))
            myfile.write("lhans2 {0}\n".format(lhans))
            myfile.write("ebeam1 {0}\n".format(ebeam1))
            myfile.write("ebeam2 {0}\n".format(ebeam2))
            if beamType == "pPb":
                myfile.write("nPDFset {0}        ! (0:EKS98, 1:EPS08, 2:EPS09LO, 3:EPS09NLO)\n".format(nPDFset))
                myfile.write("nPDFerrSet {0}     ! (1:central, 2:+1, 3:-1..., 30:+15, 31:-15)\n".format(nPDFerrSet))
                myfile.write("AA1 208            ! (Atomic number of hadron 1)\n")
                myfile.write("AA2 1              ! (Atomic number of hadron 2)\n")

        with open("powheg.input", 'r') as fin:
            powheg_input = fin.read().splitlines()
        for line in powheg_input:
            print(line)

        print("Running POWHEG...")
        with open("powheg.log", "w") as myfile:
            subprocess.call([powhegExe], stdout=myfile, stderr=myfile)

        if not os.path.isfile("pwgevents.lhe"):
            print("Could not find POWHEG output pwgevents.lhe. Something went wrong, aborting...")
            if os.path.isfile("powheg.log"):
                print("Check log file below.")
                with open("powheg.log", "r") as myfile:
                    powheg_log = myfile.read().splitlines()
                for line in powheg_log:
                    print(line)
            else:
                print("No log file was found.")
            exit(1)

        if grid:
            LHEfile = "pwgevents.lhe"
        else:
            LHEfile = "pwgevents_{0}.lhe".format(fname)
            os.rename("powheg.input", "{0}.input".format(fname))
            print("POWHEG configuration backed up in {0}.input".format(fname))
            os.rename("pwgevents.lhe", LHEfile)
            print("POWHEG events backed up in {0}".format(LHEfile))
            os.rename("powheg.log", "{0}.log".format(fname))
            print("POWHEG log backed up in {0}.log".format(fname))
            # cleaning the working directory and archiving POWHEG files
            subprocess.call(["./clean_powheg.sh", "{0}.tar".format(fname)])

    rnd = random.randint(0, 1073741824)  # 2^30
    print("Setting PYTHIA seed to {0}".format(rnd))

    print("Running PYTHIA simulation...")
    with open("sim.log", "w") as myfile:
        if grid:
            subprocess.call(["aliroot", "-b", "-l", "-q", "runJetSimulationGrid.C(\"{0}\", {1}, \"{2}\", \"{3}\", {4}, \"{5}\", \"{6}\", {7}, {8}, {9})".format(fname, pythiaEvents, proc, gen, rnd, LHEfile, beamType, ebeam1, ebeam2, pthard)], stdout=myfile, stderr=myfile)
        else:
            subprocess.call(["./runJetSimulation.py", "--numevents", str(pythiaEvents), "--proc", proc, "--gen", gen, "--seed", str(rnd), "--lhe", LHEfile, "--beam-type", beamType, "--ebeam1", str(ebeam1), "--ebeam2", str(ebeam2), "--name", fname, "--pthard", str(pthard)], stdout=myfile, stderr=myfile)

    if not grid:
        os.rename("sim.log", "sim_{0}.log".format(fname))
        print("Simulation log backed up in {0}.log".format(fname))

    print("Done")
    print("...see results in the log files")

    subprocess.call(["ls", "-l"])

    print("############### SCRATCH SIZE ####################")
    subprocess.call(["du", "-sh"])

    print("------------------ job ends ----------------------")
    dateNow = datetime.datetime.now()
    print(dateNow)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Run fast simulation.')
    parser.add_argument('--numevents', metavar='NEVT',
                        default=50000, type=int)
    parser.add_argument('--gen', metavar='GEN',
                        default='pythia')
    parser.add_argument('--lhe', metavar='LHE',
                        default='')
    parser.add_argument('--proc', metavar='PROC',
                        default='charm')
    parser.add_argument('--qmass', metavar='QMASS',
                        default=-1.0, type=float)
    parser.add_argument('--facscfact', metavar='FACSCFACT',
                        default=1.0, type=float)
    parser.add_argument('--renscfact', metavar='RENSCFACT',
                        default=1.0 , type=float)
    parser.add_argument('--lhans', metavar='LHANS',
                        default=11000, type=int)
    parser.add_argument('--beam-type', metavar='BEAMTYPE',
                        default="pp")
    parser.add_argument('--ebeam1', metavar='EBEAM1',
                        default=3500, type=int)
    parser.add_argument('--ebeam2', metavar='EBEAM2',
                        default=3500, type=int)
    parser.add_argument('--nPDFset', metavar='3',
                        default=3, type=int)
    parser.add_argument('--nPDFerrSet', metavar='1',
                        default=1, type=int)
    parser.add_argument('--pthard', metavar='1',
                        default=-1, type=int)
    parser.add_argument("--grid", action='store_const',
                        default=False, const=True,
                        help='Grid analysis.')
    args = parser.parse_args()

    main(args.numevents, args.gen, args.proc, args.qmass, args.facscfact, args.renscfact, args.lhans , args.beam_type, args.ebeam1, args.ebeam2, args.nPDFset, args.nPDFerrSet, args.lhe, args.pthard, args.grid)
