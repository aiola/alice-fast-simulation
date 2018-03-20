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


def AlienDelete(fileName):
    if fileName.find("alien://") == -1:
        fname = fileName
    else:
        fname = fileName[8:]

    subprocessCall(["alien_rm", fname])


def AlienDeleteDir(fileName):
    if fileName.find("alien://") == -1:
        fname = fileName
    else:
        fname = fileName[8:]

    subprocessCall(["alien_rmdir", fname])


def AlienFileExists(fileName):
    if fileName.find("alien://") == -1:
        fname = fileName
    else:
        fname = fileName[8:]

    fileExists = True
    try:
        subprocessCheckCall(["alien_ls", fname])
    except subprocess.CalledProcessError:
        fileExists = False

    return fileExists


def AlienCopy(source, destination, attempts=3, overwrite=False):
    i = 0
    fileExists = False

    if AlienFileExists(destination):
        if overwrite:
            AlienDelete(destination)
        else:
            return True

    if destination.find("alien://") == -1:
        dest = "alien://{0}".format(destination)
    else:
        dest = destination

    while True:
        subprocessCall(["alien_cp", source, dest])
        i += 1
        fileExists = AlienFileExists(destination)
        if fileExists:
            break
        if i >= attempts:
            print("After {0} attempts I could not copy {1} to {2}".format(i, source, dest))
            break

    return fileExists


def subprocessCall(cmd):
    print(cmd)
    return subprocess.call(cmd)


def subprocessCheckCall(cmd):
    print(cmd)
    return subprocess.check_call(cmd)


def subprocessCheckOutput(cmd):
    print(cmd)
    return subprocess.check_output(cmd, universal_newlines=True)


def CopyFilesToTheGrid(Files, AlienDest, LocalDest, Offline, GridUpdate):
    if not Offline:
        subprocessCall(["alien_mkdir", "-p", AlienDest])
        subprocessCall(["alien_mkdir", "-p", "{0}/output".format(AlienDest)])

    if not os.path.isdir(LocalDest):
        print "Creating directory " + LocalDest
        os.makedirs(LocalDest)
    for file in Files:
        if not Offline:
            AlienCopy(file, "alien://{0}/{1}".format(AlienDest, file), 3, GridUpdate)
        shutil.copy(file, LocalDest)


def GenerateComments():
    branch = subprocessCheckOutput(["git", "rev-parse", "--abbrev-ref", "HEAD"])
    hash = subprocessCheckOutput(["git", "rev-parse", "HEAD"])
    comments = "# This is the startup script \n\
# alice-yale-hfjet \n\
# Generated using branch {branch} ({hash}) \n\
".format(branch=branch.strip('\n'), hash=hash.strip('\n'))
    return comments


def GenerateProcessingJDL(Exe, AlienDest, AliPhysicsVersion, ValidationScript, FilesToCopy, TTL, Events, Jobs, yamlFileName, MinPtHard, MaxPtHard):
    comments = GenerateComments()
    jdlContent = "{comments} \n\
Executable = \"{dest}/{executable}\"; \n\
# Time after which the job is killed (120 min.) \n\
TTL = \"{TTL}\"; \n\
OutputDir = \"{dest}/output/#alien_counter_03i#\"; \n\
Output = {{ \n\
\"log_archive.zip:stderr,stdout,*.log@disk=1\", \n\
\"root_archive.zip:AnalysisResults*.root@disk=2\" \n\
}}; \n\
Arguments = \"{yamlFileName} --numevents {Events} --minpthard {MinPtHard} --maxpthard {MaxPtHard} --batch-job grid\"; \n\
Packages = {{ \n\
\"VO_ALICE@AliPhysics::{aliphysics}\", \n\
\"VO_ALICE@APISCONFIG::V1.1x\", \n\
\"VO_ALICE@POWHEG::r3178-alice1-1\", \n\
\"VO_ALICE@Python-modules::1.0-12\" \n\
}}; \n\
# JDL variables \n\
JDLVariables = \n\
{{ \n\
\"Packages\", \n\
\"OutputDir\" \n\
}}; \n\
Split=\"production:1-{Jobs}\"; \n\
ValidationCommand = \"{dest}/{validationScript}\"; \n\
# List of input files to be uploaded to workers \n\
".format(yamlFileName=yamlFileName, MinPtHard=MinPtHard, MaxPtHard=MaxPtHard, comments=comments, executable=Exe, dest=AlienDest, aliphysics=AliPhysicsVersion, validationScript=ValidationScript, Jobs=Jobs, Events=Events, TTL=TTL)

    if len(FilesToCopy) > 0:
        jdlContent += "InputFile = {"
        start = True
        for file in FilesToCopy:
            if start:
                jdlContent += "\n"
            else:
                jdlContent += ", \n"
            start = False
            jdlContent += "\"LF:{dest}/{f}\"".format(dest=AlienDest, f=file)
        jdlContent += "}; \n"

    return jdlContent


def GenerateXMLCollection(Path, XmlName):
    return subprocessCheckOutput(["alien_find", "-x", XmlName, Path, "*/AnalysisResults*.root"])


def GenerateMergingJDL(Exe, Xml, AlienDest, TrainName, AliPhysicsVersion, ValidationScript, FilesToCopy, TTL, MaxFilesPerJob, SplitMethod):
    comments = GenerateComments()
    jdlContent = "{comments} \n\
Executable = \"{dest}/{executable}\"; \n\
# Time after which the job is killed (120 min.) \n\
TTL = \"{TTL}\"; \n\
OutputDir = \"{dest}/output/#alien_counter_03i#\"; \n\
Output = {{ \n\
\"log_archive.zip:stderr,stdout,*.log@disk=1\", \n\
\"root_archive.zip:AnalysisResults*.root@disk=2\" \n\
}}; \n\
Arguments = \"{trainName} --xml wn.xml --batch-job grid\"; \n\
Packages = {{ \n\
\"VO_ALICE@AliPhysics::{aliphysics}\", \n\
\"VO_ALICE@APISCONFIG::V1.1x\", \n\
\"VO_ALICE@Python-modules::1.0-12\" \n\
}}; \n\
# JDL variables \n\
JDLVariables = \n\
{{ \n\
\"Packages\", \n\
\"OutputDir\" \n\
}}; \n\
InputDataCollection={{\"LF:{dest}/{xml},nodownload\"}}; \n\
InputDataListFormat = \"xml-single\"; \n\
InputDataList = \"wn.xml\"; \n\
SplitMaxInputFileNumber=\"{maxFiles}\"; \n\
ValidationCommand = \"{dest}/{validationScript}\"; \n\
# List of input files to be uploaded to workers \n\
".format(comments=comments, executable=Exe, xml=Xml, dest=AlienDest, trainName=TrainName, aliphysics=AliPhysicsVersion, validationScript=ValidationScript, maxFiles=MaxFilesPerJob, TTL=TTL)
    if SplitMethod:
        jdlContent += "Split=\"{split}\"; \n".format(split=SplitMethod)
    if len(FilesToCopy) > 0:
        jdlContent += "InputFile = {"
        start = True
        for file in FilesToCopy:
            if start:
                jdlContent += "\n"
            else:
                jdlContent += ", \n"
            start = False
            jdlContent += "\"LF:{dest}/{f}\"".format(dest=AlienDest, f=file)
        jdlContent += "}; \n"

    return jdlContent


def DetermineMergingStage(AlienPath, TrainName):
    AlienOutput = "{0}/{1}".format(AlienPath, TrainName)
    if not AlienFileExists(AlienOutput):
        return -1
    AlienOuputContent_orig = subprocessCheckOutput(["alien_ls", AlienOutput]).splitlines()
    AlienOuputContent = []
    for p in AlienOuputContent_orig:
        i = p.rfind("/")
        if i >= 0: p = p[i + 1:]
        AlienOuputContent.append(p)
    if not "output" in AlienOuputContent:
        print(AlienOuputContent)
        return -1
    regex = re.compile("stage_.")
    MergingStages = [string for string in AlienOuputContent if re.match(regex, string)]
    MergingStage = len(MergingStages)
    return MergingStage


def SubmitMergingJobs(TrainName, LocalPath, AlienPath, AliPhysicsVersion, Offline, GridUpdate, TTL, MaxFilesPerJob, Gen, Proc, PtHardList, MergingStage):
    if PtHardList and len(PtHardList) > 1:
        minPtHardBin = 0
        maxPtHardBin = len(PtHardList) - 1
    else:
        minPtHardBin = -1
        maxPtHardBin = 0

    for ptHardBin in range(minPtHardBin, maxPtHardBin):
        if ptHardBin < 0:
            TrainPtHardName = TrainName
        else:
            TrainPtHardName = "{0}/{1}".format(TrainName, ptHardBin)

        if MergingStage < 0:
            MergingStage = DetermineMergingStage(AlienPath, TrainPtHardName)

        if MergingStage < 0:
            print("Could not find any results from train {0}! Aborting...".format(TrainName))
            exit(1)
        elif MergingStage == 0:
            print("Merging stage determined to be 0 (i.e. first merging stage)")
            PreviousStagePath = "{0}/{1}/output".format(AlienPath, TrainPtHardName)
            SplitMethod = "parentdirectory"
        else:
            print("Merging stage determined to be {0}".format(MergingStage))
            PreviousStagePath = "{0}/{1}/stage_{2}/output".format(AlienPath, TrainPtHardName, MergingStage - 1)
            SplitMethod = "parentdirectory"

        AlienDest = "{0}/{1}/stage_{2}".format(AlienPath, TrainPtHardName, MergingStage)
        LocalDest = "{0}/{1}/stage_{2}".format(LocalPath, TrainPtHardName, MergingStage)

        if AlienFileExists(AlienDest): AlienDeleteDir(AlienDest)

        ValidationScript = "FastSim_validation.sh"
        ExeFile = "runFastSimMerging.py"
        JdlFile = "FastSim_Merging_{0}_{1}.jdl".format(Gen, Proc)
        XmlFile = "FastSim_Merging_{0}_{1}_stage_{2}.xml".format(Gen, Proc, MergingStage)

        FilesToCopy = ["runJetSimulationMergingGrid.C"]
        JdlContent = GenerateMergingJDL(ExeFile, XmlFile, AlienDest, TrainName, AliPhysicsVersion, ValidationScript, FilesToCopy, TTL, MaxFilesPerJob, SplitMethod)

        f = open(JdlFile, 'w')
        f.write(JdlContent)
        f.close()

        XmlContent = GenerateXMLCollection(PreviousStagePath, XmlFile)
        f = open(XmlFile, 'w')
        f.write(XmlContent)
        f.close()

        FilesToCopy.extend([JdlFile, XmlFile, ExeFile, ValidationScript])

        CopyFilesToTheGrid(FilesToCopy, AlienDest, LocalDest, Offline, GridUpdate)
        if not Offline:
            subprocessCall(["alien_submit", "alien://{0}/{1}".format(AlienDest, JdlFile)])
        os.remove(JdlFile)
        os.remove(XmlFile)
    print "Done."

    subprocessCall(["ls", LocalDest])


def SubmitProcessingJobs(TrainName, LocalPath, AlienPath, AliPhysicsVersion, Offline, GridUpdate, TTL, Events, Jobs, Gen, Proc, yamlFileName, PtHardList, OldPowhegInit):
    print("Submitting processing jobs for train {0}".format(TrainName))

    ValidationScript = "FastSim_validation.sh"
    ExeFile = "runFastSim.py"
    JdlFile = "FastSim_{0}_{1}.jdl".format(Gen, Proc)

    # "AliAnalysisTaskSEhfcjMCanalysis.cxx", "AliAnalysisTaskSEhfcjMCanalysis.h"
    FilesToCopy = [yamlFileName, "OnTheFlySimulationGenerator.cxx", "OnTheFlySimulationGenerator.h",
                   "runJetSimulation.C", "start_simulation.C",
                   "beauty-powheg.input", "charm-powheg.input", "dijet-powheg.input", "powheg_pythia8_conf.cmnd",
                   "Makefile", "GeneratePowhegInput.py",
                   "AliGenEvtGen_dev.h", "AliGenEvtGen_dev.cxx",
                   "AliGenPythia_dev.h", "AliGenPythia_dev.cxx",
                   "AliPythia6_dev.h", "AliPythia6_dev.cxx",
                   "AliPythia8_dev.h", "AliPythia8_dev.cxx",
                   "AliPythiaBase_dev.h", "AliPythiaBase_dev.cxx"]
    if OldPowhegInit:
        FilesToCopy.extend(["pwggrid.dat", "pwggrid.dat", "pwgubound.dat"])

    if PtHardList and len(PtHardList) > 1:
        minPtHardBin = 0
        maxPtHardBin = len(PtHardList) - 1
    else:
        minPtHardBin = -1
        maxPtHardBin = 0

    for ptHardBin in range(minPtHardBin, maxPtHardBin):
        if ptHardBin < 0:
            AlienDest = "{0}/{1}".format(AlienPath, TrainName)
            LocalDest = "{0}/{1}".format(LocalPath, TrainName)
            minPtHard = -1
            maxPtHard = -1
            JobsPtHard = Jobs
        else:
            minPtHard = PtHardList[ptHardBin]
            maxPtHard = PtHardList[ptHardBin + 1]
            AlienDest = "{0}/{1}/{2}".format(AlienPath, TrainName, ptHardBin)
            LocalDest = "{0}/{1}/{2}".format(LocalPath, TrainName, ptHardBin)
            JobsPtHard = Jobs[ptHardBin]
        JdlContent = GenerateProcessingJDL(ExeFile, AlienDest, AliPhysicsVersion, ValidationScript, FilesToCopy, TTL, Events, JobsPtHard, yamlFileName, minPtHard, maxPtHard)

        f = open(JdlFile, 'w')
        f.write(JdlContent)
        f.close()

        FilesToCopy.extend([JdlFile, ExeFile, ValidationScript])

        CopyFilesToTheGrid(FilesToCopy, AlienDest, LocalDest, Offline, GridUpdate)
        if not Offline:
            subprocessCall(["alien_submit", "alien://{0}/{1}".format(AlienDest, JdlFile)])
        os.remove(JdlFile)
    print "Done."

    subprocessCall(["ls", LocalDest])


def DownloadResults(TrainName, LocalPath, AlienPath, Gen, Proc, PtHardList, MergingStage):
    if PtHardList and len(PtHardList) > 1:
        minPtHardBin = 0
        maxPtHardBin = len(PtHardList) - 1
    else:
        minPtHardBin = -1
        maxPtHardBin = 0

    for ptHardBin in range(minPtHardBin, maxPtHardBin):
        if ptHardBin < 0:
            TrainPtHardName = TrainName
        else:
            minPtHard = PtHardList[ptHardBin]
            maxPtHard = PtHardList[ptHardBin + 1]
            TrainPtHardName = "{0}/{1}".format(TrainName, ptHardBin)
        print("Downloading results from train {0}".format(TrainPtHardName))
        if MergingStage < 0:
            MergingStage = DetermineMergingStage(AlienPath, TrainPtHardName)

        if MergingStage < 0:
            print("Could not find any results from train {0}! Aborting...".format(TrainPtHardName))
            exit(0)
        elif MergingStage == 0:
            print("Merging stage determined to be 0 (i.e. no grid merging has been performed)")
            AlienOutputPath = "{0}/{1}/output".format(AlienPath, TrainPtHardName)
            LocalDest = "{0}/{1}/output".format(LocalPath, TrainPtHardName)
        else:
            print("Merging stage determined to be {0}".format(MergingStage))
            AlienOutputPath = "{0}/{1}/stage_{2}/output".format(AlienPath, TrainPtHardName, MergingStage - 1)
            LocalDest = "{0}/{1}/stage_{2}/output".format(LocalPath, TrainPtHardName, MergingStage - 1)

        if not os.path.isdir(LocalDest):
            os.makedirs(LocalDest)
        AlienOuputContent = subprocessCheckOutput(["alien_ls", AlienOutputPath]).splitlines()
        for SubDir in AlienOuputContent:
            i = SubDir.rfind("/")
            if i >= 0: SubDir = SubDir[i + 1:]
            SubDirDest = "{0}/{1}".format(LocalDest, SubDir)
            SubDirOrig = "{0}/{1}".format(AlienOutputPath, SubDir)
            if not os.path.isdir(SubDirDest):
                os.makedirs(SubDirDest)
            FilesToDownload = subprocessCheckOutput(["alien_ls", "{0}/AnalysisResults*.root".format(SubDirOrig)]).splitlines()
            for FileName in FilesToDownload:
                i = FileName.rfind("/")
                if i >= 0: FileName = FileName[i + 1:]
                FileDest = "{0}/{1}".format(SubDirDest, FileName)
                if os.path.isfile(FileDest):
                    print("File {0} already exists, skipping...".format(FileDest))
                    continue
                FileOrig = "{0}/{1}".format(SubDirOrig, FileName)
                FileDestTemp = "{0}/temp_{1}".format(SubDirDest, FileName)
                if os.path.isfile(FileDestTemp):
                    os.remove(FileDestTemp)
                print("Downloading from {0} to {1}".format(FileOrig, FileDestTemp))
                subprocessCall(["alien_cp", "alien://{0}".format(FileOrig), FileDestTemp])
                if os.path.getsize(FileDestTemp) > 0:
                    print("Renaming {0} to {1}".format(FileDestTemp, FileDest))
                    os.rename(FileDestTemp, FileDest)
                else:
                    print("ERROR ***** Downloading of {0} failed!".format(FileOrig))
                    os.remove(FileDestTemp)


def GetLastTrainName(AlienPath, Gen, Proc):
    TrainName = "FastSim_{0}_{1}".format(Gen, Proc)
    AlienPathContent = subprocessCheckOutput(["alien_ls", AlienPath]).splitlines()
    regex = re.compile("{0}.*".format(TrainName))
    Timestamps = [int(subdir[len(TrainName) + 1:]) for subdir in AlienPathContent if re.match(regex, subdir)]
    if len(Timestamps) == 0:
        print("Could not find any train in the alien path {0} provided!".format(AlienPath))
        print("\n".join(AlienPathContent))
        print("{0}.*".format(TrainName))
        return None
    TrainName += "_{0}".format(max(Timestamps))
    return TrainName


def GetAliPhysicsVersion(ver):
    if ver == "_last_":
        now = datetime.datetime.now()
        if now.hour < 18: now -= datetime.timedelta(days=1)
        ver = now.strftime("vAN-%Y%m%d-1")
    return ver


def main(UserConf, yamlFileName, Offline, GridUpdate, OldPowhegInit, Merge, Download, MergingStage):
    f = open(yamlFileName, 'r')
    config = yaml.load(f)
    f.close()

    AliPhysicsVersion = GetAliPhysicsVersion(config["aliphysics"])
    Events = config["numevents"]
    Jobs = config["numbjobs"]
    Gen = config["gen"]
    Proc = config["proc"]
    PtHardList = config["pthard"]
    TTL = config["ttl"]
    MaxFilesPerJob = config["max_files_per_job"]

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
    AlienPath = "/alice/cern.ch/user/{0}/{1}".format(UserConf["username"][0], UserConf["username"])

    print("Local working directory: {0}".format(LocalPath))
    print("Alien working directory: {0}".format(AlienPath))

    if Merge:
        if Merge == "last":
            TrainName = GetLastTrainName(AlienPath, Gen, Proc)
            if not TrainName:
                exit(1)
        else:
            TrainName = "FastSim_{0}_{1}_{2}".format(Gen, Proc, Merge)
        SubmitMergingJobs(TrainName, LocalPath, AlienPath, AliPhysicsVersion, Offline, GridUpdate, TTL, MaxFilesPerJob, Gen, Proc, PtHardList, MergingStage)
    elif Download:
        if Download == "last":
            TrainName = GetLastTrainName(AlienPath, Gen, Proc)
            if not TrainName:
                exit(1)
        else:
            TrainName = "FastSim_{0}_{1}_{2}".format(Gen, Proc, Download)
        DownloadResults(TrainName, LocalPath, AlienPath, Gen, Proc, PtHardList, MergingStage)
    else:
        unixTS = int(time.time())
        print("The timestamp for this job is {0}. You will need it to submit merging jobs and download you final results.".format(unixTS))
        TrainName = "FastSim_{0}_{1}_{2}".format(Gen, Proc, unixTS)
        SubmitProcessingJobs(TrainName, LocalPath, AlienPath, AliPhysicsVersion, Offline, GridUpdate, TTL, Events, Jobs, Gen, Proc, yamlFileName, PtHardList, OldPowhegInit)


if __name__ == '__main__':
    # FinalMergeLocal.py executed as script


    parser = argparse.ArgumentParser(description='Local final merging for LEGO train results.')
    parser.add_argument('config', metavar='config.yaml',
                        default="default.yaml", help='YAML configuration file')
    parser.add_argument('--user-conf', metavar='USERCONF',
                        default="userConf.yaml")
    parser.add_argument("--update", action='store_const',
                        default=False, const=True,
                        help='Update all scripts and macros on the grid.')
    parser.add_argument('--offline', action='store_const',
                        default=False, const=True,
                        help='Test mode')
    parser.add_argument('--merge', metavar='TIMESTAMP',
                        default='')
    parser.add_argument('--download', metavar='TIMESTAMP',
                        default='')
    parser.add_argument('--stage', metavar='stage',
                        default=-1, type=int)
    parser.add_argument("--old-powheg-init", action='store_const',
                        default=False, const=True,
                        help='Use old POWHEG init files.')
    args = parser.parse_args()

    userConf = UserConfiguration.LoadUserConfiguration(args.user_conf)

    main(userConf, args.config, args.offline, args.update, args.old_powheg_init, args.merge, args.download, args.stage)
