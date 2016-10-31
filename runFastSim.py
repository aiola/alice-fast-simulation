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

def main(runtype, gridmode, pythiaEvents, proc, gen):
	print("------------------ job starts ---------------------")
	dateNow = datetime.datetime.now()
	print(dateNow)
	
	print("Running {0} MC production on: {1}".format(proc, " ".join(platform.uname())))
	
	if gen == "powheg":
		runPOWHEG = True
		if proc == "dijet": 
			powhegExe = "./POWHEG_bins/pwhg_main_dijet"
		elif proc == "charm":
			powhegExe = "./POWHEG_bins/pwhg_main_hvq"
		elif proc == "beauty":
			powhegExe = "./POWHEG_bins/pwhg_main_hvq"
		
		powhegEvents = int(pythiaEvents * 1.1)
		shutil.copy("{0}-powheg.input".format(proc),"powheg_new.input")
		rnd = random.randint(0, 65535)
		
		with open("powheg_new.input", "a") as myfile:
		    myfile.write("iseed {0}\n".format(rnd))
		    myfile.write("numevts {0}\n".format(powhegEvents))
		
		with open("powheg_new.input", 'r') as fin:
		    powheg_new_input = fin.read().splitlines()
	else:
		runPOWHEG = False
	
	if runPOWHEG and os.path.isfile("powheg.input") and os.path.isfile("pwgevents.lhe"):
		# POWHEG configuration file and event file already exist
		# Check if the configuration was the same
		with open("powheg.input", 'r') as fin:
			powheg_new = fin.read().splitlines()
		confDiff = set(powheg_new).symmetric_difference(powheg_new_input)
		runPOWHEG = False
		for diff in confDiff:
			if "iseed" in diff:
				continue
			else:
				runPOWHEG = True
				break
		if runPOWHEG:
			unixTS = int(time.time())
			os.rename("powheg.input","powheg_{0}.input".format(unixTS))
			print("Old POWHEG configuration backed up in {0}".format("powheg_{0}.input".format(unixTS)))
			os.rename("pwgevents.lhe","pwgevents_{0}.lhe".format(unixTS))
			print("Old POWHEG events backed up in {0}".format("pwgevents_{0}.lhe".format(unixTS)))
			if os.path.isfile("powheg.log"):
				os.rename("powheg.log","powheg_{0}.log".format(unixTS))
				print("Old POWHEG log backed up in {0}".format("powheg_{0}.log".format(unixTS)))
			print("Running new POWHEG simulation!")
		else:
			os.remove("powheg_new.input")
			print("Will use the same POWHEG events from previous simulation!")
	
	if runPOWHEG:
		print("Cleaning up working directory...")
		subprocess.call(["./clean_powheg.sh"])
		os.rename("powheg_new.input","powheg.input")
		print("Running POWHEG...")
	
		for line in powheg_new_input:
			print(line)
	
		powhegOutput = subprocess.check_output([powhegExe])
		with open("powheg.log", "w") as myfile:
			myfile.write(powhegOutput)
	
		if not os.path.isfile("pwgevents.lhe"):
			print("Could not find POWHEG output pwgevents.lhe. Something went wrong, aborting...")
			print("Check log file below.")
			print(powhegOutput)
			exit(1)
	
	rnd = random.randint(0, 65535)
	print("Setting PYTHIA seed to {0}".format(rnd))
	
	print("Running PYTHIA simulation...")
	simOutput = subprocess.check_output(["./runJetSimulation.py", "--numevents", str(pythiaEvents), "--proc", proc, "--gen", gen, "--seed", str(rnd)]) 
	with open("sim.log", "w") as myfile:
		myfile.write(simOutput)

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
    parser.add_argument('--runtype', metavar='RUNTYPE',
                        default='local')
    parser.add_argument('--gridmode', metavar='GRIDMODE',
                        default='offline')
    parser.add_argument('--numevents', metavar='NEVT',
                        default=50000, type=int)
    parser.add_argument('--gen', metavar='GEN',
                        default='pythia')
    parser.add_argument('--proc', metavar='PROC',
                        default='charm')
    args = parser.parse_args()

    main(args.runtype, args.gridmode, args.numevents, args.proc, args.gen)
