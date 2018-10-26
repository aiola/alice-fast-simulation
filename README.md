# Fast Monte Carlo simulations for jet and heavy-flavor

The software contained in this package allows one to efficiently use some of the most common Monte Carlo event generators in the High Energy Physics (HEP) field to generate events on-the-fly and make them available to be analyzed by the user.

This package contains a set of C++ classes, python scripts and YAML configurations that allow to easily submit jobs on the ALICE grid. It has been developed and used for the first studies of D^0^-meson tagged jets in pp collisions. In fact, it was previously part of the repository containing the analysis code used to study D^0^-meson tagged jets, [alice-yale-hfjet](https://github.com/aiola/alice-yale-hfjet.git).

The code nicely integrates with the ALICE software, particularly [ROOT](https://github.com/root-project/root.git), [AliRoot](https://github.com/alisw/AliRoot.git) and [AliPhysics](https://github.com/alisw/AliPhysics.git) and it uses its main analysis framework. The analysis is performed through user tasks derived from [AliAnalysisTaskSE](https://github.com/alisw/AliRoot/blob/master/ANALYSIS/ANALYSISalice/AliAnalysisTaskSE.h), as done extensively in the ALICE software framework. For more information about the ALICE software see its official [documentation](https://alice-doc.github.io/alice-analysis-tutorial/).

The code contained in this repository was written by some ALICE collaborators but it is not part of the official ALICE software (althouh it integrates with it).

The code is currently available in a `GIT` repository:

~~~~bash
git clone https://github.com/aiola/alice-fast-simulation.git ./alice-fast-simulation
cd alice-fast-simulation
~~~~

## Submit simulations to the grid

The main script is contained in `submit_grid.py`. Most of the configuration is provided using YAML configuration files, e.g.:

~~~~bash
./submit_grid.py PYTHIA8_DIJET_7TeV.yaml
~~~~

The above command will submit 500 grid jobs, each simulating 50000 pp events at 7 TeV using PYTHIA 8 (total: 25 M events). The script automatically assign different seeds to each job, so that the simulated statistics is uncorrelated.

### YAML Configuration

- `name`: name of the configuration file
- `numevents`: number of events simulated per each grid job
- `numjobs`: number of jobs to be submitted
- `gen`: Monte Carlo event generator (see next section)
- `proc`: physics process to be simulated (different options depending on generator, see below)
- `lhans`: number of the PDF in the LHAPDF library, see https://lhapdf.hepforge.org/pdfsets.html
- `beam_type`: pp or pPb
- `ebeam1`: energy of beam 1
- `ebeam2`: energy of beam 2
- `add_d_mesons`: include D0-jet task
- `extended_event_info`: use exented event info option in the D0-jet task
- `grid_config.ttl`: Time-To-Live of grid jobs
- `grid_config.max_files_per_job`: number of files to be merged at once
- `grid_config.aliphysics`: AliPhysics version (e.g. vAN-20180620-1)
- `grid_config.load_packages_separately`: grid packages are not loaded automatically thorugh the JDL, but rather loaded in a separate shell; this is useful if there are package conflicts between the different event generators used in the simulation (e.g. some Herwig packages are incompatible with some AliPhysics packages)

### Event Generators

The event generator is set using the `gen` parameter in the YAML configuration.

It is expected to be in the form `xxx+yyy+zzz`, where:

- `xxx` is the partonic event generator: `powheg`, `pythia6`, `pythia8` or `herwig`
- `yyy` is the parton shower, hadronizator, underlying event generator: `pythia6`, `pythia8` or `herwig`
- `zzz` is the decayer: `pythia6`, `pythia8`, `herwig` or `evtgen`

If the same generator is used for two or more consecutive steps it is not necessary to repeat it. For example, if you simply set:

```yaml
gen: pythia6+evtgen
```

the script will interpret it as `xxx = yyy = pythia6` and `zzz = evtgen`.

Another example: `powheg+pythia6` will be interpreted as `xxx = powheg` and `yyy = zzz = pythia6`.

#### POWHEG

The `powheg_config` subsection in the YAML configuration is used to configure POWHEG:

- `bornktmin`: minimum kT of the hard scattering
- `bornsuppfactor`: Born suppression factor, see POWHEG documentation
- `factscfact`: factorization scale factor
- `renscfact`: renormalization scale factor
- `qmass`: mass of the heavy-quark

The possible choices for the `proc` parameter are:

- `charm`: calls the heavy-quark process with default `qmass=1.5`
- `beauty`: calls the heavy-quark process with default `qmass=4.75`
- `dijet`: calls the di-jet process
- `charm_lo`: calls the heavy-quark process with default `qmass=1.5` at Leading Order (Born-only) accuracy
- `beauty_lo`: calls the heavy-quark process with default `qmass=4.75` at Leading Order (Born-only) accuracy
- `dijet_lo`: calls the di-jet process at Leading Order (Born-only) accuracy

Notice that both the NLO and LO processes are appropriately matched to PYTHIA so that all subprocesses are technically included. The difference between the two is whether PYTHIA jumps in for corrections at NLO or NNLO.

#### Herwig

At the moment there is only one option Herwig-specific option, to be added in the `herwig_config` section: `tune`. The `tune` parameter should contain the name of an input Herwig file containing the tune settings. The file should be in the same main folder of the repository.

The possible choices for the `proc` parameter are:

- `charm_lo`: calls the heavy-quark process with default `qmass=1.5` at Leading Order (Born-only) accuracy
- `beauty_lo`: calls the heavy-quark process with default `qmass=4.75` at Leading Order (Born-only) accuracy
- `dijet_lo`: calls the di-jet process at Leading Order (Born-only) accuracy
- `mb`: calls the MB process

The `dijet_lo` and `mb` are very similar, the difference being that the former uses the hard QCD process and requires a minimum *k*~T~ of 5 GeV/*c*.

#### PYTHIA 6 and PYTHIA 8

The possible choices for the `proc` parameter are:

- `charm`: calls the MB process, triggering on charm particles (i.e. events without charm are rejected)
- `beauty`: calls the MB process, triggering on beauty particles (i.e. events without beauty are rejected)
- `mb`: calls the MB process
- `dijet`: calls the di-jet process (minimum *k*~T~ of 5 GeV/*c*)
- `charm_lo`: uses only the Leading Order charm production process
- `beauty_lo`: uses only the Leading Order beauty production process

Notice that the meaning and interpretation of the processes is different in PYTHIA compared to POWHEG / Herwig. In particular, while the LO implementation of charm and beauty in POWHEG / Herwig changes the accuracy of the perturbative expansion, but technically includes processes at all orders (via the parton shower), when using PYTHIA as partonic event generator with `charm_lo` and `beauty_lo`, all charm/beauty production at any order after LO are suppressed (no gluon splitting, flavor excitation...).

### Adding more user tasks

By default, the analysis will include an inclusive jet spectra task and a D^0^-jet task. If you want to add/remove tasks, you need to modify `OnTheFlySimulationGenerator::PrepareAnalysisManager` in `OnTheFlySimulationGenerator.cxx`. This is not (yet) automated. It would be nice to be able to add tasks directly from the YAML instead of modifying the C++ code (something that could be developed for the future).

### POWHEG multi-stage

If you want to run POWHEG di-jet it is recommended to first calculate the cross section matrix elements and upload them to the grid. This calculation takes 5-10 hours (you can run it in parallel in a decent multi-core PC and have it done in a couple of hours or less). The procedure is semi-automatic. A number of matrix elements are already present in the git repository. For example:

```bash
./submit_grid.py POWHEG_PYTHIA6_DIJET_BORNKT1_BORNSUPP60_7TeV.yaml --powheg-stage 4 --old-powheg-init powheg_dijet_bornkt1_bornsupp60_1527172778
```

In order to generate the matrix elements you have to run POWHEG in a local machine with certain parameters. I wrote a script to do that in a semi-automatic way which works on a machine located at CERN and owned by LBNL. You'd have to adapt it to make it work on a different machine / batch system.

This is how you run it:

```bash
./submit_lbnl3.py POWHEG_PYTHIA6_DIJET_BORNKT1_BORNSUPP60_7TeV.yaml
```

Once it is done (the first stage will take few seconds), you run it again with:

```bash
./submit_lbnl3.py POWHEG_PYTHIA6_DIJET_BORNKT1_BORNSUPP60_7TeV.yaml --powheg-stage 1 --xgrid-iter 2 --continue-powheg TIMESTAMP
```

where TIMESTAMP will be given to you in the first stage.

Then again:

```bash
./submit_lbnl3.py POWHEG_PYTHIA6_DIJET_BORNKT1_BORNSUPP60_7TeV.yaml --powheg-stage 2 --continue-powheg TIMESTAMP
```

which will take several hours in total (speed-up by parallelizing).

And finally:

```bash
./submit_lbnl3.py POWHEG_PYTHIA6_DIJET_BORNKT1_BORNSUPP60_7TeV.yaml --powheg-stage 3 --continue-powheg TIMESTAMP
```

Notice that you have to wait until the previous stage is completed before going to the next stage. At the end of stage 3 the output is ready to be used on the grid to generate events. You can use the following script to automatically copy the output files and create a folder in the repository with all the needed files:

```bash
./prepare_powheg_stage4.py POWHEG_PYTHIA6_DIJET_BORNKT1_BORNSUPP60_7TeV.yaml --ts TIMESTAMP
```

## Submit merging jobs to the grid
The following command will submit merging jobs to the grid:

~~~~bash
./submit_grid.py PYTHIA8_DIJET_7TeV.yaml --merge TIMESTAMP
~~~~

The merging is done automatically in stages. Once a merging stage is fully completed you can submit again
a merging job with the exact same command - the script will automatically figures out the merging stage number.

## Downloading results
Results can be downloaded with the following command:

~~~~bash
./submit_grid.py PYTHIA8_DIJET_7TeV.yaml --download TIMESTAMP
~~~~

The script will download by default the last available merging stage. You can also download a different merging stage with the argument `--stage N` (use -1 to download unmerged results). The results will be downloaded in the working directory set in the `userConf.yaml` file.