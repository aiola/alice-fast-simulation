#!/usr/bin/env python

import argparse
import yaml
import shutil
import UserConfiguration
import GeneratePowhegInput
import os
import glob

def CopyFiles(TrainName, LocalPath):
    Dest = "./{}".format(TrainName)
    DestAdd = "{}/AdditionalFiles".format(Dest)
    os.makedirs(DestAdd)

    Origin = "{0}/{1}".format(LocalPath, TrainName)

    EssentialFilesToCopy = ["pwggrid-????.dat", "pwggridinfo-btl-xg?-????.dat", "pwgubound-????.dat"]

    for fpattern in EssentialFilesToCopy:
        for file in glob.glob("{}/{}".format(Origin, fpattern)): shutil.copy(file, Dest)

    AdditionalFilesToCopy = ["Powheg_Stage_?_Job_????.log", "powheg_Stage_*.input", "pwg-????-btlgrid.top", "pwg-????-stat.dat"
                             "pwg-st?-????-stat.dat", "pwg-xg?-????-btlgrid.top", "pwgboundviolations-????.dat",
                             "pwgcounters-st?-????.dat"]

    for fpattern in AdditionalFilesToCopy:
        for file in glob.glob("{}/{}".format(Origin, fpattern)): shutil.copy(file, DestAdd)


def main(UserConf, yamlFileName, unixTS):
    f = open(yamlFileName, 'r')
    config = yaml.load(f)
    f.close()

    Gen = config["gen"]
    Proc = config["proc"]
    LocalPath = UserConf["local_path"]

    print("Local working directory: {0}".format(LocalPath))

    TrainName = "FastSim_{0}_{1}_{2}".format(Gen, Proc, unixTS)
    CopyFiles(TrainName, LocalPath)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Local final merging for LEGO train results.')
    parser.add_argument('config', metavar='config.yaml',
                        default="default.yaml", help='YAML configuration file')
    parser.add_argument('--user-conf', metavar='USERCONF',
                        default="userConf.yaml")
    parser.add_argument('--ts', metavar='TS',
                        default=None)
    args = parser.parse_args()

    userConf = UserConfiguration.LoadUserConfiguration(args.user_conf)

    main(userConf, args.config, args.ts)
