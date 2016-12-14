How to run POWHEG+PYTHIA simulations on the grid

git clone https://gitlab.cern.ch/saiola/alice-yale-hfjet.git ./alice-yale-hfjet
cd fastSimulation
./submit_grid.py --aliphysics vAN-20161213-1 --gen powheg --proc charm --numevents 50000 --numjobs 500

The above command will submit 500 grid jobs, each simulating 50000 events (total: 25 M events).
The script automatically assign different seeds to each job, so that the simulated statistics
is uncorrelated.

The possible choices for the generator are:
- powheg
- pythia

In the first case POWHEG will be used to generate the parton level event and PYTHIA will be used for the
hadronization. In the second case PYTHIA will be used to generate the event from the beginning.
It is foreseen to implement more choices for the generator.

The possible choices for process are:
- charm
- beauty
- dijet

For POWHEG, it is possible to set the following parameters:
- qmass (mass of the quark)
- facscfact (factorization scale factor)
- renscfact (renormalization scale factor)
For example to change the mass of the quark:

./submit_grid.py --aliphysics vAN-20161213-1 --gen powheg --proc charm --numevents 50000 --numjobs 500 --qmass 1.7
