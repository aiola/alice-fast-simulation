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

def main(pythiaEvents, proc, gen, LHEfile):
	print("------------------ job starts ---------------------")
	dateNow = datetime.datetime.now()
	print(dateNow)

	unixTS = int(time.time())
	fname = "{0}_{1}_{2}".format(gen, proc, unixTS)

	print("Running {0} MC production on: {1}".format(proc, " ".join(platform.uname())))
	
	if gen == "powheg":
		if os.path.isfile("pwgevents.lhe") or os.path.isfile("powheg.input") or os.path.isfile("powheg.log"):
			print("Before running POWHEG again you must delete or move the following files: pwgevents.lhe, powheg.input, powheg.log")
			exit(1)

		if LHEfile:
			print("Using previously generated POWHEG events from file {0}!".format(LHEfile))
			runPOWHEG = False
		else:
			runPOWHEG = True
	else:
		runPOWHEG = False

	if runPOWHEG:
		print("Running new POWHEG simulation!")
		if proc == "dijet": 
			powhegExe = "./POWHEG_bins/pwhg_main_dijet"
		elif proc == "charm":
			powhegExe = "./POWHEG_bins/pwhg_main_hvq"
		elif proc == "beauty":
			powhegExe = "./POWHEG_bins/pwhg_main_hvq"
		
		powhegEvents = int(pythiaEvents * 1.1)
		shutil.copy("{0}-powheg.input".format(proc),"powheg.input")
		rnd = random.randint(0, 65535)
		
		with open("powheg.input", "a") as myfile:
		    myfile.write("iseed {0}\n".format(rnd))
		    myfile.write("numevts {0}\n".format(powhegEvents))
		
		with open("powheg.input", 'r') as fin:
		    powheg_input = fin.read().splitlines()
		for line in powheg_input:
			print(line)

		print("Running POWHEG...")
		with open("powheg.log", "w") as myfile:
			subprocess.call([powhegExe], stdout=myfile, stderr=myfile)
	
		if not os.path.isfile("pwgevents.lhe"):
			print("Could not find POWHEG output pwgevents.lhe. Something went wrong, aborting...")
			if os.path.isfile("powheg.log"):
				print("Check log file below.")
				with open("powheg.log", "r") as myfile:
					powheg_log = myfile.read().splitlines()
				for line in powheg_log:
					print(line)
			else:
				print("No log file was found.")
			exit(1)

		LHEfile = "pwgevents_{0}.lhe".format(fname)

		os.rename("powheg.input","{0}.input".format(fname))
		print("POWHEG configuration backed up in {0}.input".format(fname))
		os.rename("pwgevents.lhe",LHEfile)
		print("POWHEG events backed up in {0}".format(LHEfile))
		os.rename("powheg.log","{0}.log".format(fname))
		print("POWHEG log backed up in {0}.log".format(fname))
		#cleaning the working directory and archiving POWHEG files
		subprocess.call(["./clean_powheg.sh", "{0}.tar".format(fname)])

	rnd = random.randint(0, 65535)
	print("Setting PYTHIA seed to {0}".format(rnd))
	
	print("Running PYTHIA simulation...")
	with open("sim.log", "w") as myfile:
		subprocess.call(["./runJetSimulation.py", "--numevents", str(pythiaEvents), "--proc", proc, "--gen", gen, "--seed", str(rnd), "--lhe", LHEfile, "--name", fname], stdout=myfile, stderr=myfile) 

	os.rename("sim.log","sim_{0}.log".format(fname))
	print("Simulation log backed up in {0}.log".format(fname))

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
    parser.add_argument('--numevents', metavar='NEVT',
                        default=50000, type=int)
    parser.add_argument('--gen', metavar='GEN',
                        default='pythia')
    parser.add_argument('--lhe', metavar='LHE',
						default='')
    parser.add_argument('--proc', metavar='PROC',
                        default='charm')
    args = parser.parse_args()

    main(args.numevents, args.proc, args.gen, args.lhe)
