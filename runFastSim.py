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
import GeneratePowhegInput
import glob
import math


class PowhegResult:

    def __init__(self, events_generated, lhe_file, log_file):
        self.lhe_file = lhe_file
        self.log_file = log_file
        self.events_generated = events_generated


def GetNumberOfPowhegEvents(lhefile):
    if os.path.isfile(lhefile):
        proc = subprocess.Popen(["grep", "-c", "<event", lhefile], stdout=subprocess.PIPE)
        output = proc.stdout.read()
        nevents = int(output)
    else:
        nevents = 0
    return nevents


def RunPowhegParallel(powhegExe, powheg_stage, job_number):
    print("Running POWHEG simulation at stage {}!".format(powheg_stage))

    with open("powheg.input", 'r') as fin:
        powheg_input = fin.read().splitlines()
    for line in powheg_input:
        print(line)

    print("Running POWHEG...")
    LogFileName = "Powheg_Stage_{}_Job_{:04d}.log".format(powheg_stage, job_number)
    with open(LogFileName, "w") as myfile:
        print([powhegExe, str(job_number)])
        p = subprocess.Popen([powhegExe], stdout=myfile, stderr=myfile, stdin=subprocess.PIPE)
        p.communicate(input=str(job_number))

    if powheg_stage == 4:
        lhefile = "pwgevents-{:04d}.lhe".format(job_number)
        nevents = GetNumberOfPowhegEvents(lhefile)
        result = PowhegResult(nevents, lhefile, LogFileName)
    else:
        result = PowhegResult(0, "", LogFileName)

    return result


def RunPowhegSingle(powhegExe, yamlConfigFile):
    print("Running POWHEG simulation!")

    with open("powheg.input", "a") as myfile:
        rnd = random.randint(0, 1073741824)  # 2^30
        myfile.write("iseed {0}\n".format(rnd))

    with open("powheg.input", 'r') as fin:
        powheg_input = fin.read().splitlines()
    for line in powheg_input:
        print(line)

    print("Running POWHEG...")
    with open("powheg.log", "w") as myfile:
        subprocess.call([powhegExe], stdout=myfile, stderr=myfile)

    lhefile = "pwgevents.lhe"
    nevents = GetNumberOfPowhegEvents(lhefile)

    result = PowhegResult(nevents, lhefile, "powheg.log")

    return result


def main(events, powheg_stage, job_number, yamlConfigFile, batch_job, LHEfile, minpthard, maxpthard, debug_level):
    print("------------------ job starts ---------------------")
    dateNow = datetime.datetime.now()
    print(dateNow)

    abspath = os.path.abspath(__file__)
    dname = os.path.dirname(abspath)
    os.chdir(dname)

    try:
        rootPath = subprocess.check_output(["which", "root"]).rstrip()
        alirootPath = subprocess.check_output(["which", "aliroot"]).rstrip()
    except subprocess.CalledProcessError:
        print "Environment is not configured correctly!"
        exit()

    print "Root: " + rootPath
    print "AliRoot: " + alirootPath

    f = open(args.config, 'r')
    config = yaml.load(f)
    f.close()

    gen = config["gen"]
    proc = config["proc"]
    beamType = config["beam_type"]
    ebeam1 = config["ebeam1"]
    ebeam2 = config["ebeam2"]
    nPDFset = config["nPDFset"]
    nPDFerrSet = config["nPDFerrSet"]
    if "rejectISR" in config:
        rejectISR = config["rejectISR"]
    else:
        rejectISR = False

    if "powheg_buffer" in config:
        powheg_buffer = config["powheg_buffer"]
    else:
        powheg_buffer = 0.1

    if batch_job == "grid":
        fname = "{0}_{1}".format(gen, proc)
    elif batch_job == "lbnl3":
        fname = "{}_{}_{:04d}".format(gen, proc, job_number)
    else:
        unixTS = int(time.time())
        fname = "{0}_{1}_{2}".format(gen, proc, unixTS)

    print("Running {0} MC production on: {1}".format(proc, " ".join(platform.uname())))

    if "powheg" in gen:
        if powheg_stage <= 0:
            if os.path.isfile("pwgevents.lhe") or os.path.isfile("powheg.log"):
                print("Before running POWHEG again you must delete or move the following files: pwgevents.lhe, powheg.log")
                exit(1)

        if LHEfile:
            runPOWHEG = False
            nevents = GetNumberOfPowhegEvents(LHEfile)
            if nevents > 0:
                print("Using previously generated POWHEG events from file {}, where I found {} events!".format(LHEfile, nevents))
                powheg_result = PowhegResult(nevents, LHEfile, "")
            else:
                print("No events found in file {}!".format(LHEfile))
                exit(1)
        else:
            runPOWHEG = True
    else:
        runPOWHEG = False

    if runPOWHEG:
        if proc == "charm_jets" or proc == "beauty_jets":
            powheg_proc = "dijet"
        else:
            powheg_proc = proc

        if powheg_proc == "dijet":
            powhegExe = "pwhg_main_dijet"
        elif powheg_proc == "charm":
            powhegExe = "pwhg_main_hvq"
        elif powheg_proc == "beauty":
            powhegExe = "pwhg_main_hvq"
        else:
            print("Process '{}' not recognized!".format(powheg_proc))
            exit(1)

        if powheg_stage > 0 and powheg_stage <= 4:
            powheg_result = RunPowhegParallel(powhegExe, powheg_stage, job_number)
        else:
            powheg_result = RunPowhegSingle(powhegExe, yamlConfigFile)

        if not os.path.isfile(powheg_result.lhe_file) or powheg_result.events_generated <= 0:
            if powheg_stage > 0 and powheg_stage <= 3:
                print("POWHEG stage {} completed. Exiting.".format(powheg_stage))
                exit(0)
            else:
                print("POWHEG at stage {} did not produce any event!!!".format(powheg_stage))
                if os.path.isfile(powheg_result.log_file):
                    print("Check log file below.")
                    with open(powheg_result.log_file, "r") as myfile:
                        powheg_log = myfile.read().splitlines()
                    for line in powheg_log:
                        print(line)
                else:
                    print("No log file was found.")
                exit(1)
        else:
            print("POWHEG generated {} events, stored in {}".format(powheg_result.events_generated, powheg_result.lhe_file))

        if batch_job == "grid" or batch_job == "lbnl3":
            LHEfile = powheg_result.lhe_file
        else:
            LHEfile = "pwgevents_{0}.lhe".format(fname)
            os.rename("powheg.input", "{0}.input".format(fname))
            print("POWHEG configuration backed up in {0}.input".format(fname))
            os.rename(powheg_result.lhe_file, LHEfile)
            print("POWHEG events backed up in {0}".format(LHEfile))
            os.rename("powheg.log", "{0}.log".format(fname))
            print("POWHEG log backed up in {0}.log".format(fname))
            # cleaning the working directory and archiving POWHEG files
            subprocess.call(["./clean_powheg.sh", "{0}.tar".format(fname)])

    rnd = random.randint(0, 1073741824)  # 2^30
    print("Setting PYTHIA seed to {0}".format(rnd))

    if batch_job != "lbnl3":
        print("Compiling analysis code...")
        subprocess.call(["make"])

    print("Running PYTHIA simulation...")
    if batch_job == "lbnl3":
        work_dir = "output/{}".format(fname)
        os.makedirs(work_dir)
        shutil.copy("AnalysisCode.so", work_dir)
        if os.path.isfile("AnalysisCode.rootmap"): shutil.copy("AnalysisCode.rootmap", work_dir)
        shutil.copy("runJetSimulation.C", work_dir)
        shutil.copy("start_simulation.C", work_dir)
        for hdr_file in glob.glob(r'./*.h'): shutil.copy(hdr_file, work_dir)
        for pcm_file in glob.glob(r'./*.pcm'): shutil.copy(pcm_file, work_dir)
        if "powheg" in gen: LHEfile = "../../{}".format(LHEfile)
        os.chdir(work_dir)

    if "powheg" in gen:
        max_events = int(math.ceil(powheg_result.events_generated / (1.0 + powheg_buffer)))
        if events > max_events:
            print("Reducing the number of requested events to match the event found in the LHE file (with a 10% buffer to avoid PYTHIA6 crash): {}".format(max_events))
            events = max_events
    with open("sim_{0}.log".format(fname), "w") as myfile:
        subprocess.call(["aliroot", "-b", "-l", "-q", "start_simulation.C(\"{0}\", {1}, \"{2}\", \"{3}\", {4}, \"{5}\", \"{6}\", {7}, {8}, {9}, {10}, {11}, {12})".format(fname, events, proc, gen, rnd, LHEfile, beamType, ebeam1, ebeam2, int(rejectISR), minpthard, maxpthard, debug_level)], stdout=myfile, stderr=myfile)

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
    parser.add_argument('--batch-job', metavar='job',
                        default='local')
    parser.add_argument('--powheg-stage', metavar='STAGE',
                        default=0, type=int)
    parser.add_argument('--job-number', metavar='STAGE',
                        default=0, type=int)
    parser.add_argument('-d', metavar='debug_level',
                        default=0, type=int)
    args = parser.parse_args()

    main(args.numevents, args.powheg_stage, args.job_number, args.config, args.batch_job, args.lhe, args.minpthard, args.maxpthard, args.d)
