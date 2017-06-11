#!/usr/local/bin/python

import os
import yaml

def LoadUserConfiguration(path):
    if not os.path.isfile(path):
        return GenerateUserConfiguration(path)

    f = open(path, 'r')
    config = yaml.load(f)
    f.close()
    return config

def GenerateUserConfiguration(path):
    config = dict()
    username = raw_input("Please enter your CERN user name: ")
    local_path = raw_input("Please enter a local path that I can use as working directory: ")
    config["username"] = username
    config["local_path"] = local_path
    with open(path, 'w') as f:
        yaml.dump(config, f)
    return config
