#!/usr/bin/env python

import time
import datetime
import platform
import os
import shutil
import subprocess
import argparse
import random
import glob
import math
import yaml
from time import sleep
import lhapdf_utils

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

class HerwigResult:

    def __init__(self, events_generated, hep_file, log_file):
        self.hep_file = hep_file
        self.log_file = log_file
        self.events_generated = events_generated

def GetNumberOfHerwigEvents(hepfile):
    if os.path.isfile(hepfile):
        proc = subprocess.Popen(["grep", "-c", "E ", hepfile], stdout=subprocess.PIPE)
        output = proc.stdout.read()
        nevents = int(output)
    else:
        nevents = 0
    return nevents

def AddEmptyEvent(lhefile):
    backup_filename = lhefile + ".bak"
    os.rename(lhefile, backup_filename)
    with open(backup_filename, 'r') as fin:
        lhe = fin.read().splitlines()
    last_event_stop_index = -1
    last_event_start_index = -1
    for i, line in reversed(list(enumerate(lhe))):
        if last_event_stop_index < 0 and line == "</event>":
            last_event_stop_index = i
            continue
        if last_event_stop_index > 0 and line == "<event>":
            last_event_start_index = i
            break
    if last_event_stop_index < 0:
        print("Could not find </event> in the LHE file '{}'!".format(lhefile))
        os.rename(backup_filename, lhefile)
        return

    # Check whether the empty event already exists
    event_header = lhe[last_event_start_index + 1].strip().split()
    if int(event_header[0]) == 0:
        print("Empty event already present in the LHE file '{}'!".format(lhefile))
        print(lhe[last_event_start_index + 1])
        os.rename(backup_filename, lhefile)

        return
    lhe.insert(last_event_stop_index + 1, "<event>")
    lhe.insert(last_event_stop_index + 2, "      0  10001  1.0  1.0 -1.00000E+00  1.0")
    lhe.insert(last_event_stop_index + 3, "</event>")
    with open(lhefile, 'w') as fout:
        for line in lhe:
            fout.write(line + "\n")

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

def RunPowhegSingle(powhegExe):
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

def Powheg(LHEfile, proc, powheg_stage, job_number):
    if LHEfile:
        nevents = GetNumberOfPowhegEvents(LHEfile)
        if nevents > 0:
            print("Using previously generated POWHEG events from file {}, where I found {} events!".format(LHEfile, nevents))
            powheg_result = PowhegResult(nevents, LHEfile, "")
        else:
            print("No events found in file {}!".format(LHEfile))
            exit(1)
    else:
        if proc == "dijet" or proc == "dijet_lo":
            powhegExe = "pwhg_main_dijet"
        elif proc == "charm" or proc == "charm_lo":
            powhegExe = "pwhg_main_hvq"
        elif proc == "beauty" or proc == "beauty_lo":
            powhegExe = "pwhg_main_hvq"
        else:
            print("Process '{}' not recognized!".format(proc))
            exit(1)

        try:
            powhegPath = subprocess.check_output(["which", powhegExe]).rstrip()
        except subprocess.CalledProcessError:
            print("Powheg executable '{}' not found!".format(powhegExe))
            exit(1)

        print("Powheg found in '{}'".format(powhegPath))

        if powheg_stage > 0 and powheg_stage <= 4:
            powheg_result = RunPowhegParallel(powhegExe, powheg_stage, job_number)
        else:
            powheg_result = RunPowhegSingle(powhegExe)

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

    AddEmptyEvent(powheg_result.lhe_file)
    
    return powheg_result

def RunHerwig(nevents, pdfid, forceload):
    print("Running HERWIG simulation!")

    rnd = random.randint(0, 1073741824)  # 2^30

    with open("herwig.in", 'r') as fin:
        herwig_input = fin.read().splitlines()
    for line in herwig_input:
        print(line)

    if not "HERWIG_ROOT" in os.environ and "HERWIGPATH" in os.environ:
        os.environ["HERWIG_ROOT"] = os.environ["HERWIGPATH"]

    pdfname = lhapdf_utils.GetPDFName(pdfid, False)
    if TestHerwig():
        print("Running HERWIG...")
        with open("herwig_stdout.log", "w") as myfile:
            # Verify that PDF is installed
            pdfsetlist = subprocess.check_output(["lhapdf", "list", "--installed"]).splitlines()
            if pdfname in pdfsetlist:
                print("PDF '{}' already installed.".format(pdfname))
            else:
                os.environ["LHAPDF_DATA_PATH"] = "./"
                subprocess.call(["lhapdf", "--pdfdir=./", "install", pdfname], stdout=myfile, stderr=myfile)

            subprocess.call(["Herwig", "read", "herwig.in"], stdout=myfile, stderr=myfile)
            if os.path.isfile("herwig.run"):
                subprocess.call(["Herwig", "run", "herwig.run", "-s", str(rnd), "-N", str(nevents)], stdout=myfile, stderr=myfile)
            else:
                print("Something went wrong in the HERWIG run configuration.")
    elif forceload:
        print("Herwig not found. Trying to force-load the environment...")
        with open("herwig_stdout.log", "w") as myfile:
            shell = subprocess.Popen(["bash"], stdin=subprocess.PIPE, stdout=myfile, stderr=myfile)
            shell.stdin.write("alienv enter VO_ALICE@Herwig::v7.1.2-alice1-1\n")
            shell.stdin.write("which Herwig\n")

            #shell.stdin.write("env\n")

            # Verify that PDF is installed
            shell.stdin.write("lhapdf list --installed\n")
            sleep(5)
            myfile_read_mode = open("herwig_stdout.log", "r")
            output = myfile_read_mode.read()
            myfile_read_mode.close()
            if pdfname in output:
                print("PDF '{}' already installed.".format(pdfname))
            else:
                #shell.stdin.write("cp pdfsets.index ./\n")
                shell.stdin.write("export LHAPDF_DATA_PATH=$LHAPDF_DATA_PATH:./\n")
                shell.stdin.write("export LHAPDF_PDFSETS_ROOT=./\n")
                shell.stdin.write("lhapdf --pdfdir=./ install {}\n".format(pdfname))
                shell.stdin.write("pwd\n")
                shell.stdin.write("ls\n")

            shell.stdin.write("Herwig read --repo=$HERWIG_ROOT/share/Herwig/HerwigDefaults.rpo herwig.in\n")
            shell.stdin.write("ls\n")
            shell.stdin.write("Herwig run herwig.run -s {} -N {}\n".format(rnd, nevents))
            shell.communicate()
    else:
        print("HERWIG not found. Aborting...")

    hepfile = "events.hepmc"
    if os.path.isfile(hepfile):
        nevents_generated = GetNumberOfHerwigEvents(hepfile)
    else:
        print("Something went wrong in the HERWIG simulation.")
        nevents_generated = 0

    result = HerwigResult(nevents_generated, hepfile, "herwig_stdout.log")

    return result

def TestHerwig():
    try:
        herwigPath = subprocess.check_output(["which", "Herwig"]).rstrip()
    except subprocess.CalledProcessError:
        print("Herwig executable '{}' not found!".format("Herwig"))
        return False
    print("Herwig found in '{}'".format(herwigPath))
    return True

def Herwig(HEPfile, nevents, pdfid, herwig_force_load):
    if HEPfile:
        nevents_generated = GetNumberOfHerwigEvents(HEPfile)
        if nevents_generated > 0:
            print("Using previously generated HERWIG events from file {}, where I found {} events!".format(HEPfile, nevents_generated))
            herwig_result = HerwigResult(nevents_generated, HEPfile, "")
        else:
            print("No events found in file {}!".format(HEPfile))
            exit(1)
    else:
        herwig_result = RunHerwig(nevents, pdfid, herwig_force_load)
    
    return herwig_result

def main(events, powheg_stage, job_number, yamlConfigFile, batch_job, input_events, minpthard, maxpthard, debug_level, herwig_force_load):
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
    if "add_d_mesons" in config:
        always_d_mesons = config["add_d_mesons"]
    else:
        always_d_mesons = False

    if "powheg_buffer" in config:
        powheg_buffer = config["powheg_buffer"]
    else:
        powheg_buffer = 0.0

    if "extended_event_info" in config:
        extended_event_info = config["extended_event_info"]
    else:
        extended_event_info = False

    if batch_job == "grid":
        fname = "{0}_{1}".format(gen, proc)
    elif batch_job == "lbnl3":
        fname = "{}_{}_{:04d}".format(gen, proc, job_number)
    else:
        unixTS = int(time.time())
        fname = "{0}_{1}_{2}".format(gen, proc, unixTS)

    print("Running {0} MC production on: {1}".format(proc, " ".join(platform.uname())))

    LHEfile = ""
    HEPfile = ""

    if "powheg" in gen:
        powheg_result = Powheg(input_events, proc, powheg_stage, job_number)
        LHEfile = powheg_result.lhe_file
        if powheg_buffer > 0:
            max_events = int(math.floor(powheg_result.events_generated / (1.0 + powheg_buffer) + 0.5))
        else:
            max_events = powheg_result.events_generated
        if max_events == 0:
            print("Error no events generated by POWHEG!")
            exit(1)
        if events > max_events:
            print("Reducing the number of requested events to match the event found in the LHE file (with a {}% buffer to avoid PYTHIA6 crash): {}".format(powheg_buffer * 100, max_events))
            events = max_events
    
    if "herwig" in gen:
        herwig_result = Herwig(input_events, events, config["lhans"], herwig_force_load)
        HEPfile = herwig_result.hep_file
        max_events = herwig_result.events_generated
        if max_events == 0:
            print("Error no events generated by HERWIG!")
            exit(1)
        if events > max_events:
            print("Reducing the number of requested events to match the event found in the HEP file: {}".format(max_events))
            events = max_events

    rnd = random.randint(0, 1073741824)  # 2^30
    print("Setting seed to {0}".format(rnd))

    print("Compiling analysis code...")
    subprocess.call(["make"])

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
        if "herwig" in gen: HEPfile = "../../{}".format(HEPfile)
        os.chdir(work_dir)

    print("Running simulation...")
    with open("sim_{0}.log".format(fname), "w") as myfile:
        subprocess.call(["aliroot", "-b", "-l", "-q", "start_simulation.C(\"{0}\", {1}, \"{2}\", \"{3}\", {4}, \"{5}\", \"{6}\", \"{7}\", {8}, {9}, {10}, {11}, {12}, {13}, {14})".format(fname, events, proc, gen, rnd, LHEfile, HEPfile, beamType, ebeam1, ebeam2, int(always_d_mesons), int(extended_event_info), minpthard, maxpthard, debug_level)], stdout=myfile, stderr=myfile)

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
    parser.add_argument('--input-events', metavar='file.lhe',
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
    parser.add_argument('--herwig-force-load', action='store_const',
                        default=False, const=True,
                        help='Force load Herwig if not found')
    args = parser.parse_args()

    main(args.numevents, args.powheg_stage, args.job_number, args.config, args.batch_job, args.input_events, args.minpthard, args.maxpthard, args.d, args.herwig_force_load)
