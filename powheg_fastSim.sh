#!/bin/bash

echo "------------------ job starts ---------------------"
date +%s

export PWHGPROC=$1
NEVT=$2

echo "Running $1 MC production on:"
uname -a

echo "Running POWHEG..."

cp "${PWHGPROC}-powheg.input" "powheg.input"

echo "iseed $RANDOM" >> powheg.input
echo "numevts $NEVT" >> powheg.input

cat powheg.input

if [ "$PWHGPROC" = "dijet" ] 
then
	PWHGEXE="pwhg_main_dijet"
	PYPROC="kPyJetsPWHG"
	TRAINNAME="FastSim_PyJetsPWHG"
elif [ "$PWHGPROC" = "charm" ]
then
	PWHGEXE="pwhg_main_hvq"
	PYPROC="kPyCharmPWHG"
	TRAINNAME="FastSim_PyCharmPWHG"
elif [ "$PWHGPROC" = "beauty" ]
then
	PWHGEXE="pwhg_main_hvq"
	PYPROC="kPyBeautyPWHG"
	TRAINNAME="FastSim_PyBeautyPWHG"
fi

if [ -e "pwgevents.lhe" ]
then
	rm pwgevents.lhe
fi

./POWHEG_bins/${PWHGEXE} &> pwhg.log

export CONFIG_SEED=$RANDOM
echo "Setting PYTHIA seed to $CONFIG_SEED"

NE="$(echo "scale=0; $NEVT-0.1*$NEVT" | bc)"
export PYNEVT="$(echo "($NE+0.5)/1" | bc)"

echo "Running simulation..."
aliroot -b >& sim.log << EOF
.x runJetSimulation.C("local","offline",${PYNEVT},${PYPROC},"${TRAINNAME}")
.q
EOF

echo "Done"
echo "...see results in the log file"

ls -l

echo "############### SCRATCH SIZE ####################"
du -sh

echo "------------------ job ends ----------------------"
date +%s
