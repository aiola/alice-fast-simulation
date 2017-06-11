#!/usr/local/bin/python

import time
import datetime
import platform
import os
import shutil
import subprocess
import argparse
import random
import sys

def main(trainName, xmlFile, grid):
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

    print("Running merging of {0} on: {1}".format(trainName, " ".join(platform.uname())))

    print("Running merging...")
    output = "AnalysisResults_{0}.root".format(trainName)
    subprocess.call(["aliroot", "-b", "-l", "-q", "runJetSimulationMergingGrid.C(\"{0}\", \"{1}\")".format(output, xmlFile)])

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
    parser.add_argument('train_name', metavar='NAME')
    parser.add_argument('--xml', metavar='FILE.XML')
    parser.add_argument("--grid", action='store_const',
                        default=False, const=True,
                        help='Grid analysis.')
    args = parser.parse_args()

    main(args.train_name, args.xml, args.grid)
