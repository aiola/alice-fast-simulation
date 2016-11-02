#!/usr/bin/env python

import os
import subprocess
import argparse
import yaml
import time
import shutil
import re

def AlienDelete(fileName):
    if fileName.find("alien://") == -1:
        fname = fileName
    else:
        fname = fileName[8:]

    subprocessCall(["alien_rm", fname])

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
    return  subprocess.call(cmd)
    
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
        print "Creating directory "+LocalDest
        os.makedirs(LocalDest)
    for file in Files:
        if not Offline:
            AlienCopy(file, "alien://{0}/{1}".format(AlienDest,file), 3, GridUpdate)
        shutil.copy(file, LocalDest)

def GenerateProcessingJDL(Exe, AlienDest, AliPhysicsVersion, ValidationScript, FilesToCopy, Events, Jobs, Gen, Proc):
    jdlContent = "# This is the startup script \n\
Executable = \"{dest}/{executable}\"; \n\
# Time after which the job is killed (120 min.) \n\
TTL = \"7200\"; \n\
OutputDir = \"{dest}/output/#alien_counter_03i#\"; \n\
Output = {{ \n\
\"log_archive.zip:stderr,stdout,*.log@disk=1\", \n\
\"root_archive.zip:AnalysisResults*.root@disk=2\" \n\
}}; \n\
Arguments = \"--gen {Gen} --proc {Proc} --numevents {Events} --grid\"; \n\
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
".format(executable=Exe, dest=AlienDest, aliphysics=AliPhysicsVersion, validationScript=ValidationScript, Jobs=Jobs, Events=Events, Gen=Gen, Proc=Proc)

    if len(FilesToCopy) > 0:
        jdlContent += "InputFile = {"
        start=True
        for file in FilesToCopy:
            if start:
                jdlContent += "\n"
            else:
                jdlContent += ", \n"
            start=False
            jdlContent += "\"LF:{dest}/{f}\"".format(dest=AlienDest, f=file)
        jdlContent += "}; \n"

    return jdlContent

def GenerateXMLCollection(Path, XmlName):
    return subprocessCheckOutput(["alien_find", "-x", XmlName, Path, "*/AnalysisResults*.root"])

def GenerateMergingJDL(Exe, Xml, AlienDest, TrainName, AliPhysicsVersion, ValidationScript, FilesToCopy, MaxFilesPerJob, SplitMethod):
    jdlContent = "# This is the startup script \n\
Executable = \"{dest}/{executable}\"; \n\
# Time after which the job is killed (120 min.) \n\
TTL = \"7200\"; \n\
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
".format(executable=Exe, xml=Xml, dest=AlienDest, trainName=TrainName, aliphysics=AliPhysicsVersion, validationScript=ValidationScript, maxFiles=MaxFilesPerJob)
    if SplitMethod:
        jdlContent += "Split=\"{split}\"; \n".format(split=SplitMethod)
    if len(FilesToCopy) > 0:
        jdlContent += "InputFile = {"
        start=True
        for file in FilesToCopy:
            if start:
                jdlContent += "\n"
            else:
                jdlContent += ", \n"
            start=False
            jdlContent += "\"LF:{dest}/{f}\"".format(dest=AlienDest, f=file)
        jdlContent += "}; \n"

    return jdlContent

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

def SubmitMergingJobs(TrainName, LocalPath, AlienPath, AliPhysicsVersion, Offline, GridUpdate, MaxFilesPerJob, Gen, Proc):
    MergingStage = DetermineMergingStage(AlienPath, TrainName)

    if MergingStage < 0:
        print("Could not find any results from train {0}! Aborting...".format(TrainName))
        exit(1)
    elif MergingStage == 0:
        print("Merging stage determined to be 0 (i.e. first merging stage)")
        PreviousStagePath = "{0}/{1}/output".format(AlienPath, TrainName)
        SplitMethod = "se"
    else:
        print("Merging stage determined to be {0}".format(MergingStage))
        PreviousStagePath = "{0}/{1}/stage_{2}/output".format(AlienPath, TrainName, MergingStage-1)
        SplitMethod = "parentdirectory"

    AlienDest = "{0}/{1}/stage_{2}".format(AlienPath, TrainName, MergingStage)
    LocalDest = "{0}/{1}/stage_{2}".format(LocalPath, TrainName, MergingStage)

    ValidationScript="FastSim_validation.sh"
    ExeFile = "runFastSimMerging.py"
    JdlFile = "FastSim_Merging_{0}_{1}.jdl".format(Gen, Proc)
    XmlFile = "FastSim_Merging_{0}_{1}_stage_{2}.xml".format(Gen, Proc, MergingStage)

    FilesToCopy = ["runJetSimulationMergingGrid.C"]
    JdlContent = GenerateMergingJDL(ExeFile, XmlFile, AlienDest, TrainName, AliPhysicsVersion, ValidationScript, FilesToCopy, MaxFilesPerJob, SplitMethod)

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

def SubmitProcessingJobs(TrainName, LocalPath, AlienPath, AliPhysicsVersion, Offline, GridUpdate, Events, Jobs, Gen, Proc, OldPowhegInit):
    AlienDest = "{0}/{1}".format(AlienPath, TrainName)
    LocalDest = "{0}/{1}".format(LocalPath, TrainName)

    ValidationScript="FastSim_validation.sh"
    ExeFile = "runFastSim.py"
    JdlFile = "FastSim_{0}_{1}.jdl".format(Gen, Proc)

    FilesToCopy = ["runJetSimulation.C", "runJetSimulationGrid.C", "AliAnalysisTaskSEhfcjMCanalysis.cxx", "AliAnalysisTaskSEhfcjMCanalysis.h",
                   "beauty-powheg.input", "charm-powheg.input", "dijet-powheg.input"]
    if OldPowhegInit:
        FilesToCopy.extend(["pwggrid.dat", "pwggrid.dat", "pwgubound.dat"])
    JdlContent = GenerateProcessingJDL(ExeFile, AlienDest, AliPhysicsVersion, ValidationScript, FilesToCopy, Events, Jobs, Gen, Proc)

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

def main(AliPhysicsVersion, Offline, GridUpdate, Events, Jobs, Gen, Proc, OldPowhegInit, Merge, Download, MaxFilesPerJob):
    try:
        rootPath=subprocess.check_output(["which", "root"]).rstrip()
        alirootPath=subprocess.check_output(["which", "aliroot"]).rstrip()
        alienPath=subprocess.check_output(["which", "alien-token-info"]).rstrip()
    except subprocess.CalledProcessError:
        print "Environment is not configured correctly!"
        exit()

    print "Root: "+rootPath
    print "AliRoot: "+alirootPath
    print "Alien: "+alienPath

    try:
        print "Token info disabled"
        #tokenInfo=subprocess.check_output(["alien-token-info"])
    except subprocess.CalledProcessError:
        print "Alien token not available. Creating a token for you..."
        try:
            #tokenInit=subprocess.check_output(["alien-token-init", "saiola"], shell=True)
            print "Token init disabled"
        except subprocess.CalledProcessError:
            print "Error: could not create the token!"
            exit()

    if "JETRESULTS" in os.environ:
        LocalPath=os.environ["JETRESULTS"]
    else:
        LocalPath="."
    AlienPath = "/alice/cern.ch/user/s/saiola"
    
    if Merge:
        TrainName="FastSim_{0}_{1}_{2}".format(Gen, Proc, Merge)
        SubmitMergingJobs(TrainName, LocalPath, AlienPath, AliPhysicsVersion, Offline, GridUpdate, MaxFilesPerJob, Gen, Proc)
    elif Download:
        TrainName="FastSim_{0}_{1}_{2}".format(Gen, Proc, Download)
        DownloadResults(TrainName, LocalPath, AlienPath, Gen, Proc)
    else:
        unixTS = int(time.time())
        TrainName="FastSim_{0}_{1}_{2}".format(Gen, Proc, unixTS)
        SubmitProcessingJobs(TrainName, LocalPath, AlienPath, AliPhysicsVersion, Offline, GridUpdate, Events, Jobs, Gen, Proc, OldPowhegInit)

if __name__ == '__main__':
    # FinalMergeLocal.py executed as script

    parser = argparse.ArgumentParser(description='Local final merging for LEGO train results.')
    parser.add_argument('--aliphysics', metavar='vXXX',
                        help='AliPhysics version')
    parser.add_argument('--offline', action='store_const',
                        default=False, const=True,
                        help='Test mode')
    parser.add_argument("--update", action='store_const',
                        default=False, const = True,
                        help='Update all scripts and macros on the grid.')
    parser.add_argument('--numevents', metavar='NEVT',
                        default=50000, type=int)
    parser.add_argument('--numjobs', metavar='NJOBS',
                        default=10, type=int)
    parser.add_argument('--gen', metavar='GEN',
                        default='pythia')
    parser.add_argument('--proc', metavar='PROC',
                        default='charm')
    parser.add_argument("--old-powheg-init", action='store_const',
                        default=False, const = True,
                        help='Use old POWHEG init files.')
    parser.add_argument('--merge', metavar='TIMESTAMP',
                        default='')
    parser.add_argument('--max-files-per-job', metavar='N',
                        default=15, type=int)
    parser.add_argument('--download', metavar='TIMESTAMP',
                        default='')
    args = parser.parse_args()

    main(args.aliphysics, args.offline, args.update, args.numevents, args.numjobs, args.gen, args.proc, args.old_powheg_init, args.merge, args.download, args.max_files_per_job)
