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

# goodSites = ["ALICE::LUNARC::SLURM", "ALICE::ORNL::LCG", "ALICE::SNIC::SLURM", "ALICE::Torino::Torino-CREAM",
#             "ALICE::Bari::CREAM", "ALICE::GRIF_IRFU::LCG", "ALICE::NIHAM::PBS64", "ALICE::RRC_KI_T1::LCG",
#             "ALICE::Catania::Catania_VF", "ALICE::RRC_KI_T1::LCG"]
# badSites = ["ALICE::UiB::ARC"]

goodSites = []
badSites = []

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

def GenerateProcessingJDL(Exe, AlienDest, AliPhysicsVersion, ValidationScript, FilesToCopy, TTL, Events, Jobs, Gen, Proc, QMass, FacScFact, RenScFact, LHANS, BeamType, EBeam1, EBeam2):
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
Arguments = \"--gen {Gen} --proc {Proc} --qmass {QMass} --facscfact {FacScFact} --renscfact {RenScFact} --lhans {LHANS} --beam-type {BeamType} --ebeam1 {EBeam1} --ebeam2 {EBeam2} --numevents {Events} --grid\"; \n\
Packages = {{ \n\
\"VO_ALICE@AliPhysics::{aliphysics}\", \n\
\"VO_ALICE@APISCONFIG::V1.1x\", \n\
\"VO_ALICE@POWHEG::r3178-5\", \n\
\"VO_ALICE@Python-modules::1.0-4\" \n\
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
".format(comments=comments, executable=Exe, dest=AlienDest, aliphysics=AliPhysicsVersion, validationScript=ValidationScript, Jobs=Jobs, Events=Events, Gen=Gen, Proc=Proc, QMass=QMass, FacScFact=FacScFact, RenScFact=RenScFact, LHANS=LHANS, BeamType=BeamType, EBeam1=EBeam1, EBeam2=EBeam2, TTL=TTL)

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

    requirements = GenerateSiteRequirements()
    jdlContent += requirements
    return jdlContent

def GenerateXMLCollection(Path, XmlName):
    return subprocessCheckOutput(["alien_find", "-x", XmlName, Path, "*/AnalysisResults*.root"])

def GenerateMergingJDL(Exe, Xml, AlienDest, TrainName, AliPhysicsVersion, ValidationScript, FilesToCopy, TTL, MaxFilesPerJob, SplitMethod):
    jdlContent = "{comments} \n\
Executable = \"{dest}/{executable}\"; \n\
# Time after which the job is killed (120 min.) \n\
TTL = \"{TTL}\"; \n\
OutputDir = \"{dest}/output/#alien_counter_03i#\"; \n\
Output = {{ \n\
\"log_archive.zip:stderr,stdout,*.log@disk=1\", \n\
\"root_archive.zip:AnalysisResults*.root@disk=2\" \n\
}}; \n\
Arguments = \"{trainName} --xml wn.xml --grid\"; \n\
Packages = {{ \n\
\"VO_ALICE@AliPhysics::{aliphysics}\", \n\
\"VO_ALICE@APISCONFIG::V1.1x\", \n\
\"VO_ALICE@Python-modules::1.0-4\" \n\
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

    requirements = GenerateSiteRequirements()
    jdlContent += requirements
    return jdlContent

def GenerateSiteRequirements():
    PosRequirements = ""
    NegRequirements = ""
    if len(goodSites) > 0:
        for site in goodSites:
            PosRequirements += "(other.CE == \"{0}\") ||".format(site)
        PosRequirements = PosRequirements[:-3]
    if len(badSites) > 0:
        for site in badSites:
            NegRequirements += "(other.CE != \"{0}\") &&".format(site)
        NegRequirements = NegRequirements[:-3]
    if PosRequirements and not NegRequirements:
        requirements = "Requirements = ({0});\n".format(PosRequirements)
    elif NegRequirements and not PosRequirements:
        requirements = "Requirements = ({0});\n".format(NegRequirements)
    elif NegRequirements and PosRequirements:
        requirements = "Requirements = ({0} && {1});\n".format(PosRequirements, NegRequirements)
    else:
        requirements = ""
    return requirements

def DetermineMergingStage(AlienPath, TrainName):
    AlienOutput = "{0}/{1}".format(AlienPath, TrainName)
    if not AlienFileExists(AlienOutput):
        return -1
    AlienOuputContent = subprocessCheckOutput(["alien_ls", AlienOutput]).splitlines()
    if not "output" in AlienOuputContent:
        return -1
    regex = re.compile("stage_.")
    MergingStages = [string for string in AlienOuputContent if re.match(regex, string)]
    MergingStage = len(MergingStages)
    return MergingStage

def SubmitMergingJobs(TrainName, LocalPath, AlienPath, AliPhysicsVersion, Offline, GridUpdate, TTL, MaxFilesPerJob, Gen, Proc, MergingStage):
    if MergingStage < 0:
        MergingStage = DetermineMergingStage(AlienPath, TrainName)

    if MergingStage < 0:
        print("Could not find any results from train {0}! Aborting...".format(TrainName))
        exit(1)
    elif MergingStage == 0:
        print("Merging stage determined to be 0 (i.e. first merging stage)")
        PreviousStagePath = "{0}/{1}/output".format(AlienPath, TrainName)
        SplitMethod = "parentdirectory"
    else:
        print("Merging stage determined to be {0}".format(MergingStage))
        PreviousStagePath = "{0}/{1}/stage_{2}/output".format(AlienPath, TrainName, MergingStage - 1)
        SplitMethod = "parentdirectory"

    AlienDest = "{0}/{1}/stage_{2}".format(AlienPath, TrainName, MergingStage)
    if AlienFileExists(AlienDest): AlienDeleteDir(AlienDest)
    LocalDest = "{0}/{1}/stage_{2}".format(LocalPath, TrainName, MergingStage)

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

def SubmitProcessingJobs(TrainName, LocalPath, AlienPath, AliPhysicsVersion, Offline, GridUpdate, TTL, Events, Jobs, Gen, Proc, QMass, FacScFact, RenScFact, LHANS, BeamType, EBeam1, EBeam2, OldPowhegInit):
    print("Submitting merging jobs for train {0}".format(TrainName))
    AlienDest = "{0}/{1}".format(AlienPath, TrainName)
    LocalDest = "{0}/{1}".format(LocalPath, TrainName)

    ValidationScript = "FastSim_validation.sh"
    ExeFile = "runFastSim.py"
    JdlFile = "FastSim_{0}_{1}.jdl".format(Gen, Proc)

    # "AliAnalysisTaskSEhfcjMCanalysis.cxx", "AliAnalysisTaskSEhfcjMCanalysis.h"
    FilesToCopy = ["OnTheFlySimulationGenerator.cxx", "OnTheFlySimulationGenerator.h", "runJetSimulationGrid.C",
                   "beauty-powheg.input", "charm-powheg.input", "dijet-powheg.input"]
    if OldPowhegInit:
        FilesToCopy.extend(["pwggrid.dat", "pwggrid.dat", "pwgubound.dat"])
    JdlContent = GenerateProcessingJDL(ExeFile, AlienDest, AliPhysicsVersion, ValidationScript, FilesToCopy, TTL, Events, Jobs, Gen, Proc, QMass, FacScFact, RenScFact, LHANS, BeamType, EBeam1, EBeam2)

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

def DownloadResults(TrainName, LocalPath, AlienPath, Gen, Proc, MergingStage):
    print("Downloading results from train {0}".format(TrainName))
    if MergingStage < 0:
        MergingStage = DetermineMergingStage(AlienPath, TrainName)

    if MergingStage < 0:
        print("Could not find any results from train {0}! Aborting...".format(TrainName))
        exit(0)
    elif MergingStage == 0:
        print("Merging stage determined to be 0 (i.e. no grid merging has been performed)")
        AlienOutputPath = "{0}/{1}/output".format(AlienPath, TrainName)
        LocalDest = "{0}/{1}/output".format(LocalPath, TrainName)
    else:
        print("Merging stage determined to be {0}".format(MergingStage))
        AlienOutputPath = "{0}/{1}/stage_{2}/output".format(AlienPath, TrainName, MergingStage - 1)
        LocalDest = "{0}/{1}/stage_{2}/output".format(LocalPath, TrainName, MergingStage - 1)

    if not os.path.isdir(LocalDest):
        os.makedirs(LocalDest)
    AlienOuputContent = subprocessCheckOutput(["alien_ls", AlienOutputPath]).splitlines()
    for SubDir in AlienOuputContent:
        SubDirDest = "{0}/{1}".format(LocalDest, SubDir)
        SubDirOrig = "{0}/{1}".format(AlienOutputPath, SubDir)
        if not os.path.isdir(SubDirDest):
            os.makedirs(SubDirDest)
        FilesToDownload = subprocessCheckOutput(["alien_ls", "{0}/AnalysisResults*.root".format(SubDirOrig)]).splitlines()
        for FileName in FilesToDownload:
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

def main(UserConf, AliPhysicsVersion, Offline, GridUpdate, Events, Jobs, Gen, Proc, QMass, FacScFact, RenScFact, LHANS, BeamType, EBeam1, EBeam2, OldPowhegInit, TTL, Merge, Download, MergingStage, MaxFilesPerJob):
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
        SubmitMergingJobs(TrainName, LocalPath, AlienPath, AliPhysicsVersion, Offline, GridUpdate, TTL, MaxFilesPerJob, Gen, Proc, MergingStage)
    elif Download:
        if Download == "last":
            TrainName = GetLastTrainName(AlienPath, Gen, Proc)
            if not TrainName:
                exit(1)
        else:
            TrainName = "FastSim_{0}_{1}_{2}".format(Gen, Proc, Download)
        DownloadResults(TrainName, LocalPath, AlienPath, Gen, Proc, MergingStage)
    else:
        unixTS = int(time.time())
        print("The timestamp for this job is {0}. You will need it to submit merging jobs and download you final results.".format(unixTS))
        TrainName = "FastSim_{0}_{1}_{2}".format(Gen, Proc, unixTS)
        SubmitProcessingJobs(TrainName, LocalPath, AlienPath, AliPhysicsVersion, Offline, GridUpdate, TTL, Events, Jobs, Gen, Proc, QMass, FacScFact, RenScFact, LHANS, BeamType, EBeam1, EBeam2, OldPowhegInit)

if __name__ == '__main__':
    # FinalMergeLocal.py executed as script

    parser = argparse.ArgumentParser(description='Local final merging for LEGO train results.')
    parser.add_argument('--aliphysics', metavar='vXXX',
                        help='AliPhysics version')
    parser.add_argument('--offline', action='store_const',
                        default=False, const=True,
                        help='Test mode')
    parser.add_argument("--update", action='store_const',
                        default=False, const=True,
                        help='Update all scripts and macros on the grid.')
    parser.add_argument('--numevents', metavar='NEVT',
                        default=50000, type=int)
    parser.add_argument('--numjobs', metavar='NJOBS',
                        default=10, type=int)
    parser.add_argument('--gen', metavar='GEN',
                        default='pythia')
    parser.add_argument('--proc', metavar='PROC',
                        default='charm')
    parser.add_argument('--qmass', metavar='QMASS',
                        default=-1)
    parser.add_argument('--facscfact', metavar='FACSCFACT',
                        default=1)
    parser.add_argument('--renscfact', metavar='RENSCFACT',
                        default=1)
    parser.add_argument('--lhans', metavar='RENSCFACT',
                        default=11000)
    parser.add_argument('--beam-type', metavar='BEAMTYPE',
                        default="pp")
    parser.add_argument('--ebeam1', metavar='ENERGY1',
                        default=3500)
    parser.add_argument('--ebeam2', metavar='ENERGY2',
                        default=3500)
    parser.add_argument("--old-powheg-init", action='store_const',
                        default=False, const=True,
                        help='Use old POWHEG init files.')
    parser.add_argument('--ttl', metavar='TTL',
                        default=7200)
    parser.add_argument('--merge', metavar='TIMESTAMP',
                        default='')
    parser.add_argument('--max-files-per-job', metavar='N',
                        default=10, type=int)
    parser.add_argument('--download', metavar='TIMESTAMP',
                        default='')
    parser.add_argument('--stage', metavar='TIMESTAMP',
                        default=-1, type=int)
    parser.add_argument('--user-conf', metavar='USERCONF',
                        default="userConf.yaml")
    args = parser.parse_args()

    userConf = UserConfiguration.LoadUserConfiguration(args.user_conf)

    main(userConf, args.aliphysics, args.offline, args.update, args.numevents, args.numjobs, args.gen, args.proc, args.qmass, args.facscfact, args.renscfact, args.lhans, args.beam_type, args.ebeam1, args.ebeam2, args.old_powheg_init, args.ttl, args.merge, args.download, args.stage, args.max_files_per_job)
