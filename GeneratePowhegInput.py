#!/usr/bin/env python

import shutil
import argparse
import math
import yaml

def GetParallelInputFileName(powheg_stage, x_grid_iter=1):
    if powheg_stage == 1:
        fname = "powheg_Stage_{}_XGrid_{}.input".format(powheg_stage, x_grid_iter)
    else:
        fname = "powheg_Stage_{}.input".format(powheg_stage)
    return fname

def GenerateParallelPowhegInput(outputdir, powheg_stage, x_grid_iter, events, jobs, powheg_proc, bornonly, qmass, facscfact, renscfact, lhans, beamType, ebeam1, ebeam2, bornktmin, bornsuppfact, storemintupb, powheg_buffer, nPDFset, nPDFerrSet):
    fname = "{}/{}".format(outputdir, GetParallelInputFileName(powheg_stage, x_grid_iter))
    shutil.copy("{}-powheg.input".format(powheg_proc), fname)

    with open(fname, "a") as myfile:
        myfile.write("numevts {0}\n".format(int(math.ceil(events * (1.0 + powheg_buffer)))))
        myfile.write("manyseeds 1\n")
        myfile.write("maxseeds {}\n".format(jobs))
        myfile.write("parallelstage {}\n".format(powheg_stage))
        if powheg_proc == "beauty":
            myfile.write("qmass {0}\n".format(qmass))
            myfile.write("ncall1 2000\n")
            myfile.write("itmx1 5\n")
            myfile.write("ncall2 5000\n")
            myfile.write("itmx2 5\n")
        elif powheg_proc == "charm":
            myfile.write("qmass {0}\n".format(qmass))
            myfile.write("ncall1 10000\n")
            myfile.write("itmx1 5\n")
            myfile.write("ncall2 5000\n")
            myfile.write("itmx2 5\n")
        elif powheg_proc == "dijet":
            myfile.write("ncall1 5000\n")
            myfile.write("itmx1 5\n")
            myfile.write("ncall2 1000\n")
            myfile.write("itmx2 5\n")

        myfile.write("facscfact {0}\n".format(facscfact))
        myfile.write("renscfact {0}\n".format(renscfact))
        myfile.write("bornktmin {0}\n".format(bornktmin))
        myfile.write("bornsuppfact {0}\n".format(bornsuppfact))

        if bornonly:
            myfile.write("bornonly 1\n")

        myfile.write("storemintupb {0}\n".format(storemintupb))
        if powheg_stage == 1: myfile.write("xgriditeration {}\n".format(x_grid_iter))
        myfile.write("lhans1 {0}\n".format(lhans))
        myfile.write("lhans2 {0}\n".format(lhans))
        myfile.write("ebeam1 {0}\n".format(ebeam1))
        myfile.write("ebeam2 {0}\n".format(ebeam2))

        if beamType == "pPb":
            myfile.write("nPDFset {0}        ! (0:EKS98, 1:EPS08, 2:EPS09LO, 3:EPS09NLO)\n".format(nPDFset))
            myfile.write("nPDFerrSet {0}     ! (1:central, 2:+1, 3:-1..., 30:+15, 31:-15)\n".format(nPDFerrSet))
            myfile.write("AA1 208            ! (Atomic number of hadron 1)\n")
            myfile.write("AA2 1              ! (Atomic number of hadron 2)\n")

def GenerateSinglePowhegInput(outputdir, events, powheg_proc, bornonly, qmass, facscfact, renscfact, lhans, beamType, ebeam1, ebeam2, bornktmin, bornsuppfact, storemintupb, powheg_buffer, nPDFset, nPDFerrSet):
    fname = "{}/powheg.input".format(outputdir)
    shutil.copy("{}-powheg.input".format(powheg_proc), fname)

    with open(fname, "a") as myfile:
        myfile.write("numevts {0}\n".format(int(math.ceil(events * (1.0 + powheg_buffer)))))
        if powheg_proc == "beauty":
            myfile.write("qmass {0}\n".format(qmass))
            myfile.write("ncall1 10000\n")
            myfile.write("itmx1 5\n")
            myfile.write("ncall2 100000\n")
            myfile.write("itmx2 5\n")
        elif powheg_proc == "charm":
            myfile.write("qmass {0}\n".format(qmass))
            myfile.write("ncall1 50000\n")
            myfile.write("itmx1 5\n")
            myfile.write("ncall2 100000\n")
            myfile.write("itmx2 5\n")
        elif powheg_proc == "dijet":
            myfile.write("ncall1 20000\n")
            myfile.write("itmx1 5\n")
            myfile.write("ncall2 20000\n")
            myfile.write("itmx2 5\n")

        myfile.write("facscfact {0}\n".format(facscfact))
        myfile.write("renscfact {0}\n".format(renscfact))
        myfile.write("bornktmin {0}\n".format(bornktmin))
        myfile.write("bornsuppfact {0}\n".format(bornsuppfact))

        if bornonly:
            myfile.write("bornonly 1\n")

        myfile.write("storemintupb {0}\n".format(storemintupb))
        myfile.write("lhans1 {0}\n".format(lhans))
        myfile.write("lhans2 {0}\n".format(lhans))
        myfile.write("ebeam1 {0}\n".format(ebeam1))
        myfile.write("ebeam2 {0}\n".format(ebeam2))
        if beamType == "pPb":
            myfile.write("nPDFset {0}        ! (0:EKS98, 1:EPS08, 2:EPS09LO, 3:EPS09NLO)\n".format(nPDFset))
            myfile.write("nPDFerrSet {0}     ! (1:central, 2:+1, 3:-1..., 30:+15, 31:-15)\n".format(nPDFerrSet))
            myfile.write("AA1 208            ! (Atomic number of hadron 1)\n")
            myfile.write("AA2 1              ! (Atomic number of hadron 2)\n")


def main(yamlConfigFile, outputdir, events, powheg_stage, x_grid_iter=1):
    f = open(yamlConfigFile, 'r')
    config = yaml.load(f)
    f.close()

    proc = config["proc"]
    lhans = config["lhans"]
    beamType = config["beam_type"]
    jobs = config["numbjobs"]
    ebeam1 = config["ebeam1"]
    ebeam2 = config["ebeam2"]
    if beamType != "pp":
        nPDFset = config["nPDFset"]
        nPDFerrSet = config["nPDFerrSet"]
    else: # not used but set them to some default value for backward compatibility
        nPDFset = 3
        nPDFerrSet = 1

    powheg_proc = proc
    sep = powheg_proc.find('_')
    if sep >= 0:
        powheg_proc = powheg_proc[0:sep]

    if proc.endswith("_lo"):
        bornonly = True
    else:
        bornonly = False

    # Optional parameters
    if "qmass" in config["powheg_config"]:
        qmass = config["powheg_config"]["qmass"]
    else:
        qmass = None

    if not qmass or qmass < 0:
        if powheg_proc == "charm":
            qmass = 1.5
        elif powheg_proc == "beauty":
            qmass = 4.75

    if "facscfact" in config["powheg_config"]:
        facscfact = config["powheg_config"]["facscfact"]
    else:
        facscfact = 1

    if "renscfact" in config["powheg_config"]:
        renscfact = config["powheg_config"]["renscfact"]
    else:
        renscfact = 1

    if "storemintupb" in config["powheg_config"]:
        storemintupb = config["powheg_config"]["storemintupb"]
    else:
        storemintupb = 0

    if "bornktmin" in config["powheg_config"]:
        bornktmin = config["powheg_config"]["bornktmin"]
    else:
        bornktmin = 0

    if "bornsuppfact" in config["powheg_config"]:
        bornsuppfact = config["powheg_config"]["bornsuppfact"]
    else:
        bornsuppfact = 0

    if "powheg_buffer" in config:
        powheg_buffer = config["powheg_buffer"]
    else:
        powheg_buffer = 0.1

    shutil.copy("{}-powheg.input".format(powheg_proc), "{}/powheg.input".format(outputdir))

    if powheg_stage > 0 and powheg_stage <= 4:
        GenerateParallelPowhegInput(outputdir, powheg_stage, x_grid_iter, events, jobs, powheg_proc, bornonly, qmass, facscfact, renscfact, lhans, beamType, ebeam1, ebeam2, bornktmin, bornsuppfact, storemintupb, powheg_buffer, nPDFset, nPDFerrSet)
    else:
        GenerateSinglePowhegInput(outputdir, events, powheg_proc, bornonly, qmass, facscfact, renscfact, lhans, beamType, ebeam1, ebeam2, bornktmin, bornsuppfact, storemintupb, powheg_buffer, nPDFset, nPDFerrSet)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate POWHEG input file.')
    parser.add_argument('config', metavar='config.yaml',
                        default="default.yaml", help='YAML configuration file')
    parser.add_argument('--numevents', metavar='NEVT',
                        default=50000, type=int)
    parser.add_argument('-o', metavar='job',
                        default='./')
    parser.add_argument('--powheg-stage', metavar='STAGE',
                        default=0, type=int)
    args = parser.parse_args()

    main(args.config, args.o, args.numevents, args.powheg_stage)
