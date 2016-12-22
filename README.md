# How to run POWHEG+PYTHIA simulations on the grid

The code is available in a `GIT` repository:

~~~~
git clone https://gitlab.cern.ch/saiola/alice-yale-hfjet.git ./alice-yale-hfjet
cd alice-yale-hfjet/fastSimulation
~~~~

## Submit simulations to the grid

~~~~
./submit_grid.py --aliphysics vAN-20161213-1 --gen powheg --proc charm --numevents 50000 --numjobs 500 --ttl 7200
~~~~

The above command will submit 500 grid jobs, each simulating 50000 events (total: 25 M events), with a TTL (Time-To-Live) of 7200 seconds.
The script automatically assign different seeds to each job, so that the simulated statistics is uncorrelated.

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

The following additional parameters are available for both POWHEG and PYTHIA:

- `--beam-type [pp | pPb]`: set the beam type either `pp` or `pPb`; the beam type determines whether the the method `OnTheFlySimulationGenerator::AddDJet_pp()` or `OnTheFlySimulationGenerator::AddDJet_pPb()` will be used (default is `pp`);
- `--ebeam1 X` and `--ebeam2 Y`: set the energy of the two beams; note that this functionality is not well tested for asymmetric collisions (it seems that POWHEG gets stuck for asymmetric collisions), so for the moment it is recommended to use symmetric only.

For POWHEG, it is possible to set the following parameters:

- qmass (mass of the quark)

- facscfact (factorization scale factor)

- renscfact (renormalization scale factor)

- lhans (LHA PDF number, see https://lhapdf.hepforge.org/pdfsets.html)

For example to change the mass of the quark:

~~~~
./submit_grid.py --aliphysics vAN-20161213-1 --gen powheg --proc charm --numevents 50000 --numjobs 500 --qmass 1.7
~~~~

## Submit merging jobs to the grid
The following command will submit merging jobs to the grid.

~~~~
./submit_grid.py --aliphysics vAN-20161213-1 --gen powheg --proc charm --merge [TIMESTAMP] --max-files-per-job 10
~~~~

The (optional) argument `max-files-per-job` sets the maximum number of files merged in each job. The default is 10.
The merging is done automatically in stages. Once a merging stage is fully completed you can submit again
a merging job with the exact same command - the script will automatically figure out the merging stage number.
It is recommended to merge not more than about 5 M events in a single file (merged file sizes will reach about 500 MB). This means that if you submitted 500 jobs with 50k events each, two merging stages are enough (everything will be merged in 5 files).

## Download results
Results can be downloaded with the following command:

~~~~
./submit_grid.py --aliphysics vAN-20161213-1 --gen powheg --proc charm --download [TIMESTAMP] --max-files-per-job 10
~~~~

The script will download by default the last available merging stage. You can also download a different merging stage with the argument `--stage [N]` (use -1 to download unmerged results). The results will be downloaded in the working directory set in the `userConf.yaml` file.
