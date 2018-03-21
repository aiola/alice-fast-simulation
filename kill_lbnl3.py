#!/usr/bin/env python

import subprocess
import argparse


def main(cmd, minjobid, maxjobid):
    for jobid in range(minjobid, maxjobid + 1):
        subprocess.call([cmd, str(jobid)])


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Local final merging for LEGO train results.')
    parser.add_argument('minjobid', metavar='jobid')
    parser.add_argument('maxjobid', metavar='jobid')
    parser.add_argument("-d", action='store_const',
                        default=False, const=True)
    args = parser.parse_args()

    if args.d:
        cmd = "qdel"
    else:
        cmd = "qsig"

    main(cmd, args.minjobid, args.maxjobid)
