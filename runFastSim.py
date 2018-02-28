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
import yaml


def main(pythiaEvents, gen, proc, qmass, facscfact, renscfact, lhans, beamType, ebeam1, ebeam2, nPDFset, nPDFerrSet, rejectISR, LHEfile, minpthard, maxpthard, grid, debug_level):
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

    if "powheg" in gen:
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
        else:
            print("Process '{}' not recognized!".format(proc))

        if not grid:
            powhegExe = "./POWHEG_bins/{0}".format(powhegExe)

        powhegEvents = int(pythiaEvents * 1.1)
        shutil.copy("{0}-powheg.input".format(proc), "powheg.input")
        rnd = random.randint(0, 1073741824)  # 2^30

        with open("powheg.input", "a") as myfile:
            myfile.write("iseed {0}\n".format(rnd))
            myfile.write("numevts {0}\n".format(powhegEvents))
            if proc == "beauty" or proc == "charm":
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

    print("Compiling analysis code...")
    subprocess.call(["make"])

    print("Running PYTHIA simulation...")
    with open("sim_{0}.log".format(fname), "w") as myfile:
        subprocess.call(["aliroot", "-b", "-l", "-q", "start_simulation.C(\"{0}\", {1}, \"{2}\", \"{3}\", {4}, \"{5}\", \"{6}\", {7}, {8}, {9}, {10}, {11}, {12})".format(fname, pythiaEvents, proc, gen, rnd, LHEfile, beamType, ebeam1, ebeam2, int(rejectISR), minpthard, maxpthard, debug_level)], stdout=myfile, stderr=myfile)

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
    parser.add_argument('config', metavar='config.yaml',
                        default="default.yaml", help='YAML configuration file')
    parser.add_argument('--numevents', metavar='NEVT',
                        default=50000, type=int)
    parser.add_argument('--lhe', metavar='LHE',
                        default='')
    parser.add_argument('--minpthard', metavar="MINPTHARD",
                        default=-1, type=float)
    parser.add_argument('--maxpthard', metavar='MAXPTHARD',
                        default=-1, type=float)
    parser.add_argument("--grid", action='store_const',
                        default=False, const=True,
                        help='Grid analysis.')
    parser.add_argument('-d', metavar='debug_level',
                        default=0, type=int)
    args = parser.parse_args()

    f = open(args.config, 'r')
    config = yaml.load(f)
    f.close()

    Gen = config["gen"]
    Proc = config["proc"]
    if "qmass" in config: QMass = config["qmass"]
    else: QMass = None
    if "facscfact" in config: FacScFact = config["facscfact"]
    else: FacScFact = None
    if "renscfact" in config: RenScFact = config["renscfact"]
    else: RenScFact = None
    LHANS = config["lhans"]
    BeamType = config["beam_type"]
    EBeam1 = config["ebeam1"]
    EBeam2 = config["ebeam2"]
    nPDFset = config["nPDFset"]
    nPDFerrSet = config["nPDFerrSet"]
    if "rejectISR" in config:
        rejectISR = config["rejectISR"]
    else:
        rejectISR = False

    main(args.numevents, Gen, Proc, QMass, FacScFact, RenScFact, LHANS, BeamType, EBeam1, EBeam2, nPDFset, nPDFerrSet, rejectISR, args.lhe, args.minpthard, args.maxpthard, args.grid, args.d)
