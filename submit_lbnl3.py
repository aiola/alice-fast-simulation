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
        RunJobFileName = "{}/RunJob_{:03d}.sh".format(LocalDest, ijob)
        with open(RunJobFileName, "w") as myfile:
            myfile.write("#!/bin/bash\n")
            myfile.write("source /home/salvatore/load_alice.sh\n")
            myfile.write("{LocalDest}/{ExeFile} {yamlFileName} --numevents {Events} --job-number {ijob} --batch-job lbnl3\n".format(LocalDest=LocalDest, ExeFile=ExeFile, yamlFileName=yamlFileName, Events=Events, ijob=ijob))
        output = subprocessCheckOutput(["qsub", RunJobFileName])
        print(output)


def SubmitParallelPowheg(LocalDest, ExeFile, PowhegStage, Events, Jobs, yamlFileName):
    with open("{}/pwgseeds.dat".format(LocalDest), "w") as myfile:
        for ijob in range(1, Jobs + 1):
            rnd = random.randint(0, 1073741824)  # 2^30
            myfile.write("{}\n".format(rnd))

    powheg_input_file = "{}/powheg.input".format(LocalDest)
    shutil.copy("{}/{}-powheg.input".format(LocalDest, powheg_proc), powheg_input_file)

    with open("powheg.input", "a") as myfile:
        myfile.write("numevts {0}\n".format(powhegEvents))
        myfile.write("manyseeds 1\n")
        myfile.write("parallelstage {}\n".format(powheg_stage))
        if powheg_proc == "beauty" or powheg_proc == "charm":
            myfile.write("qmass {0}\n".format(qmass))
            myfile.write("facscfact {0}\n".format(facscfact))
            myfile.write("renscfact {0}\n".format(renscfact))
            myfile.write("ncall1 4000\n")
            myfile.write("itmx1 5\n")
            myfile.write("ncall2 4000\n")
            myfile.write("itmx2 5\n")
        elif powheg_proc == "dijet":
            myfile.write("bornktmin {0}\n".format(bornktmin))
            myfile.write("ncall1 10000\n")
            myfile.write("itmx1 5\n")
            myfile.write("ncall2 20000\n")
            myfile.write("itmx2 5\n")

        if powheg_stage == 1: myfile.write("xgriditeration 1\n")
        myfile.write("lhans1 {0}\n".format(lhans))
        myfile.write("lhans2 {0}\n".format(lhans))
        myfile.write("ebeam1 {0}\n".format(ebeam1))
        myfile.write("ebeam2 {0}\n".format(ebeam2))

        if beamType == "pPb":
            myfile.write("nPDFset {0}        ! (0:EKS98, 1:EPS08, 2:EPS09LO, 3:EPS09NLO)\n".format(nPDFset))
            myfile.write("nPDFerrSet {0}     ! (1:central, 2:+1, 3:-1..., 30:+15, 31:-15)\n".format(nPDFerrSet))
            myfile.write("AA1 208            ! (Atomic number of hadron 1)\n")
            myfile.write("AA2 1              ! (Atomic number of hadron 2)\n")

    if PowhegStage == 1:
        njobs = 10
    elif PowhegStage == 2:
        njobs = 100
    elif PowhegStage == 3:
        njobs = 10
    elif PowhegStage == 4:
        njobs = Jobs
    for ijob in range(1, njobs + 1):
        #JobDir = "{}/Job_Stage_{}_{:03d}".format(LocalDest, PowhegStage, ijob)
        #os.makedirs(JobDir)
        #src_files = os.listdir(LocalDest)
        #for file_name in src_files:
        #    full_file_name = os.path.join(LocalDest, file_name)
        #    if (os.path.isfile(full_file_name)):
        #        shutil.copy(full_file_name, JobDir)
        #RunJobFileName = "{}/runjob.sh".format(JobDir)
        JobDir = LocalDest
        JobOutput = "{}/JobOutput_Stage_{}_{:03d}.log".format(JobDir, PowhegStage, ijob)
        RunJobFileName = "{}/RunJob_{}_{:03d}.sh".format(JobDir, PowhegStage, ijob)
        with open(RunJobFileName, "w") as myfile:
            myfile.write("#!/bin/bash\n")
            myfile.write("#PBS -o %s\n" %(JobOutput))
            myfile.write("#PBS -j oe\n")
            myfile.write("source /home/salvatore/load_alice.sh\n")
            myfile.write("{LocalDest}/{ExeFile} {yamlFileName} --numevents {Events} --job-number {ijob} --powheg-stage {PowhegStage} --batch-job lbnl3\n".format(LocalDest=LocalDest, ExeFile=ExeFile, yamlFileName=yamlFileName, Events=Events, ijob=ijob, PowhegStage=PowhegStage))
        os.chmod(RunJobFileName, 0755)
        output = subprocessCheckOutput(["qsub", RunJobFileName])
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
        SubmitParallelPowheg(LocalDest, ExeFile, PowhegStage, Events, Jobs, yamlFileName)
    else:
        SubmitParallel(LocalDest, ExeFile, Events, Jobs, yamlFileName)

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
        qsubPath = subprocess.check_output(["which", "qsub"]).rstrip()
    except subprocess.CalledProcessError:
        print "Environment is not configured correctly!"
        exit()

    print "Root: " + rootPath
    print "AliRoot: " + alirootPath
    print "qsub: " + qsubPath

    LocalPath = UserConf["local_path"]

    print("Local working directory: {0}".format(LocalPath))

    if not continue_powheg:
        unixTS = int(time.time())
        copy_files = True
        print("New job with timestamp {0}.".format(unixTS))
    else:
        unixTS = continue_powheg
        copy_files = False
        print("Continue job with timestamp {0}.".format(unixTS))
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
                        type=int)
    args = parser.parse_args()

    userConf = UserConfiguration.LoadUserConfiguration(args.user_conf)

    main(userConf, args.config, args.powheg_stage, args.continue_powheg)
