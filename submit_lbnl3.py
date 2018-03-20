#!/usr/bin/env python

# script to submit fast simulation jobs to the grid
# submit a processing job using POWHEG charm settings with 100 subjobs, each producing 50k events
# ./submit_grid.py --aliphysics vAN-20161101-1 --gen powheg --proc charm --numevents 50000 --numjobs 100
# submit a merging job with a maximum of 15 files processed in each subjob (you will need the timestamp number provided after submitting the processing job)
# ./submit_grid.py --aliphysics vAN-20161101-1 --gen powheg --proc charm --merge [TIMESTAMP] --max-files-per-job 15
# you should keep submitting merging jobs until everything is merged in an acceptable number of files (you can go on until everything is merged in a single file if you like)
# download your results from the last merging stage (you will need the timestamp number provided after submitting the processing job)
# ./submit_grid.py --aliphysics vAN-20161101-1 --gen powheg --proc charm --download [TIMESTAMP]
# you can specify a merging stage if you want to download an intermediate merging stage using the option "--stage [STAGE]"

import os
import subprocess
import argparse
import yaml
import time
import shutil
import re
import UserConfiguration
import datetime
import random


def subprocessCall(cmd):
    print(cmd)
    return subprocess.call(cmd)


def subprocessCheckCall(cmd):
    print(cmd)
    return subprocess.check_call(cmd)


def subprocessCheckOutput(cmd):
    print(cmd)
    return subprocess.check_output(cmd, universal_newlines=True)


def CopyFilesToTheWorkingDir(Files, LocalDest):
    output_dir = "{}/output".format(LocalDest)
    if not os.path.isdir(output_dir):
        print("Creating directory {}".format(output_dir))
        os.makedirs("{}/output".format(output_dir))
    for file in Files:
        shutil.copy(file, LocalDest)


def SubmitParallel(LocalDest, ExeFile, Events, Jobs, yamlFileName):
    for ijob in range(0, Jobs):
        output = subprocessCheckOutput(["qsub", "-F", "\"{yamlFileName} --numevents {Events} --job-number {ijob} --batch-job lbnl3\"".format(yamlFileName=yamlFileName, Events=Events, ijob=ijob), "{}/{}".format(LocalDest, ExeFile)])
        print(output)


def SubmitParallelPowheg(LocalDest, ExeFile, PowhegStage, Events, Jobs, yamlFileName):
    with open("pwgseeds.dat", "a") as myfile:
        for ijob in range(1, Jobs + 1):
            rnd = random.randint(0, 1073741824)  # 2^30
            myfile.write("{}\n".format(rnd))

    if PowhegStage == 1:
        njobs = 10
    elif PowhegStage == 2:
        njobs = 100
    elif PowhegStage == 3:
        njobs = 10
    elif PowhegStage == 4:
        njobs = Jobs
    for ijob in range(1, njobs + 1):
        output = subprocessCheckOutput(["qsub", "-F", "\"{yamlFileName} --numevents {Events} --job-number {ijob} --powheg-stage {PowhegStage} --batch-job lbnl3\"".format(yamlFileName=yamlFileName, Events=Events, ijob=ijob, PowhegStage=PowhegStage), "{}/{}".format(LocalDest, ExeFile)])
        print(output)


def GenerateComments():
    branch = subprocessCheckOutput(["git", "rev-parse", "--abbrev-ref", "HEAD"])
    hash = subprocessCheckOutput(["git", "rev-parse", "HEAD"])
    comments = "# This is the startup script \n\
# alice-yale-hfjet \n\
# Generated using branch {branch} ({hash}) \n\
".format(branch=branch.strip('\n'), hash=hash.strip('\n'))
    return comments


def SubmitProcessingJobs(TrainName, LocalPath, Events, Jobs, Gen, Proc, yamlFileName, copy_files, PowhegStage):
    print("Submitting processing jobs for train {0}".format(TrainName))

    ExeFile = "runFastSim.py"
    LocalDest = "{0}/{1}".format(LocalPath, TrainName)

    if copy_files:
        FilesToCopy = [yamlFileName, "OnTheFlySimulationGenerator.cxx", "OnTheFlySimulationGenerator.h",
                       "runJetSimulation.C", "start_simulation.C",
                       "beauty-powheg.input", "charm-powheg.input", "dijet-powheg.input", "powheg_pythia8_conf.cmnd",
                       "Makefile",
                       "AliGenEvtGen_dev.h", "AliGenEvtGen_dev.cxx",
                       "AliGenPythia_dev.h", "AliGenPythia_dev.cxx",
                       "AliPythia6_dev.h", "AliPythia6_dev.cxx",
                       "AliPythia8_dev.h", "AliPythia8_dev.cxx",
                       "AliPythiaBase_dev.h", "AliPythiaBase_dev.cxx"]

        FilesToCopy.extend([ExeFile])

        CopyFilesToTheWorkingDir(FilesToCopy, LocalDest)

    if "powheg" in Gen:
        SubmitParallelPowheg(LocalDest, ExeFile, PowhegStage, Events, Jobs, Gen, Proc, yamlFileName)
    else:
        SubmitParallel(LocalDest, ExeFile, Events, Jobs, Gen, Proc, yamlFileName)

    print "Done."


def main(UserConf, yamlFileName, powheg_stage, continue_powheg):
    f = open(yamlFileName, 'r')
    config = yaml.load(f)
    f.close()

    Events = config["numevents"]
    Jobs = config["numbjobs"]
    Gen = config["gen"]
    Proc = config["proc"]

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

    try:
        print "Token info disabled"
        # tokenInfo=subprocess.check_output(["alien-token-info"])
    except subprocess.CalledProcessError:
        print "Alien token not available. Creating a token for you..."
        try:
            # tokenInit=subprocess.check_output(["alien-token-init", UserConf["username"]], shell=True)
            print "Token init disabled"
        except subprocess.CalledProcessError:
            print "Error: could not create the token!"
            exit()

    LocalPath = UserConf["local_path"]

    print("Local working directory: {0}".format(LocalPath))

    if continue_powheg:
        unixTS = int(time.time())
        copy_files = False
        print("The timestamp for this job is {0}. You will need it to submit merging jobs and download you final results.".format(unixTS))
    else:
        unixTS = continue_powheg
        copy_files = True
        print("The timestamp for this job is {0}. You will need it to submit merging jobs and download you final results.".format(unixTS))
    TrainName = "FastSim_{0}_{1}_{2}".format(Gen, Proc, unixTS)
    SubmitProcessingJobs(TrainName, LocalPath, Events, Jobs, Gen, Proc, yamlFileName, copy_files, powheg_stage)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Local final merging for LEGO train results.')
    parser.add_argument('config', metavar='config.yaml',
                        default="default.yaml", help='YAML configuration file')
    parser.add_argument('--user-conf', metavar='USERCONF',
                        default="userConf.yaml")
    parser.add_argument('--continue-powheg', metavar='USERCONF',
                        default=None)
    parser.add_argument('--powheg-stage', metavar='USERCONF',
                        default=0, type=int)
    args = parser.parse_args()

    userConf = UserConfiguration.LoadUserConfiguration(args.user_conf)

    main(userConf, args.config, args.powheg_stage, args.continue_powheg)
