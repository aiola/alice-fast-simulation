#!/bin/bash

echo "------------------ job starts ---------------------"
date +%s

export PWHGPROC=$1
export PYNEVT=$2

echo "Running $1 MC production on:"
uname -a

if [ "$PWHGPROC" = "dijet" ] 
then
	PWHGEXE="pwhg_main_dijet"
elif [ "$PWHGPROC" = "charm" ]
then
	PWHGEXE="pwhg_main_hvq"
elif [ "$PWHGPROC" = "beauty" ]
then
	PWHGEXE="pwhg_main_hvq"
fi

if [ ! -e "pwgevents.lhe" ]
then
	echo "Running POWHEG..."

	cp "${PWHGPROC}-powheg.input" "powheg.input"
	
	NE="$(echo "scale=0; $NEVT+0.1*$NEVT" | bc)"
	PWHGNEVT="$(echo "($NE+0.5)/1" | bc)"

	echo "iseed $RANDOM" >> powheg.input
	echo "numevts $PWHGNEVT" >> powheg.input

	cat powheg.input
	./POWHEG_bins/${PWHGEXE} &> pwhg.log
fi

export CONFIG_SEED=$RANDOM
echo "Setting PYTHIA seed to $CONFIG_SEED"

echo "Running simulation..."
./runJetSimulation.py --runtype local --gridmode offline --numevents ${PYNEVT} --proc ${PWHGPROC} >& sim.log 

echo "Done"
echo "...see results in the log file"

ls -l

echo "############### SCRATCH SIZE ####################"
du -sh

echo "------------------ job ends ----------------------"
date +%s
