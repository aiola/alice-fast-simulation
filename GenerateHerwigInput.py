#! /usr/bin/env python

import argparse
import yaml
import lhapdf_utils

def GetCMSEnergy(config_params):
    if config_params["beam_type"] != "pp":
        print("Herwig only works with pp, not implemented for {} (yet?)".format(config_params["beam_type"]))
        exit(1)
    elif config_params["ebeam1"] != config_params["ebeam2"]:
        print("Herwig only works with symmetric collisions.")
        exit(1)
    else:
        return config_params["ebeam1"] * 2

def GenerateHerwigInput(config_params, outputdir, events):
    fname = "{}/herwig.in".format(outputdir)

    with open(fname, "w") as myfile:
        myfile.write("read snippets/PPCollider.in\n")
        myfile.write("set /Herwig/Generators/EventGenerator:EventHandler:LuminosityFunction:Energy {}.0\n".format(GetCMSEnergy(config_params)))
        if config_params["proc"] == "beauty_lo":
            myfile.write("set /Herwig/MatrixElements/MEHeavyQuark:QuarkType 5\n")
            myfile.write("insert /Herwig/MatrixElements/SubProcess:MatrixElements[0] /Herwig/MatrixElements/MEHeavyQuark\n")
        elif config_params["proc"] == "charm_lo":
            myfile.write("set /Herwig/MatrixElements/MEHeavyQuark:QuarkType 4\n")
            myfile.write("insert /Herwig/MatrixElements/SubProcess:MatrixElements[0] /Herwig/MatrixElements/MEHeavyQuark\n")
        elif config_params["proc"] == "dijet_lo":
            myfile.write("insert /Herwig/MatrixElements/SubProcess:MatrixElements[0] /Herwig/MatrixElements/MEQCD2to2\n")
        elif config_params["proc"] == "mb":
            myfile.write("read snippets/MB.in\n")
            myfile.write("read snippets/SoftTune.in\n")
        else:
            print("Process '{}' not implemented for HERWIG!".format(config_params["proc"]))
            exit(1)
        # See: https://arxiv.org/abs/0803.0883
        # Chapter B.2
        myfile.write("set JetKtCut:MinKT 0.0*GeV")
        myfile.write("set QCDCuts:MHatMin 0.0*GeV")
        myfile.write("create ThePEG::LHAPDF /Herwig/Partons/PDFSet ThePEGLHAPDF.so\n")
        myfile.write("set /Herwig/Partons/PDFSet:PDFName {}\n".format(lhapdf_utils.GetPDFName(config_params["lhans"])))
        myfile.write("set /Herwig/Partons/PDFSet:RemnantHandler /Herwig/Partons/HadronRemnants\n")
        myfile.write("set /Herwig/Particles/p+:PDF /Herwig/Partons/PDFSet\n")
        myfile.write("set /Herwig/Particles/pbar-:PDF /Herwig/Partons/PDFSet\n")
        myfile.write("read snippets/HepMC.in\n")
        myfile.write("set /Herwig/Analysis/HepMC:Filename events.hepmc\n")
        myfile.write("set /Herwig/Analysis/HepMC:PrintEvent {}\n".format(events))
        myfile.write('saverun herwig /Herwig/Generators/EventGenerator\n')

def main(yamlConfigFile, outputdir, events):
    f = open(yamlConfigFile, 'r')
    configparams = yaml.load(f)
    f.close()

    GenerateHerwigInput(configparams, outputdir, events)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate HERWIG input file.')
    parser.add_argument('config', metavar='config.yaml',
                        default="default.yaml", help='YAML configuration file')
    parser.add_argument('--numevents', metavar='NEVT',
                        default=50000, type=int)
    parser.add_argument('-o', metavar='job',
                        default='./')
    args = parser.parse_args()

    main(args.config, args.o, args.numevents)
