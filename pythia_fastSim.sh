#!/bin/bash

echo "------------------ job starts ---------------------"
date +%s

PYTHIAPROC=$1
NEVT=$2

echo "Running $1 MC production on:"
uname -a

export CONFIG_SEED=$RANDOM
echo "Setting PYTHIA seed to $CONFIG_SEED"

echo "Running simulation..."
./runJetSimulation.py --runtype local --gridmode offline --numevents ${NEVT} --proc ${PYTHIAPROC} >& sim.log 

echo "Done"
echo "...see results in the log file"

ls -l

echo "############### SCRATCH SIZE ####################"
du -sh

echo "------------------ job ends ----------------------"
date +%s
