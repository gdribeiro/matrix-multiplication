#!/bin/python

import os
import sys
import subprocess
import argparse

# Parsing input
parser = argparse.ArgumentParser(description='Options for Matrix Multiplication Tests.')
parser.add_argument('-n','--ntest', metavar='N', type=int, nargs='+',
                    help="Number of times to run each test.")
parser.add_argument('-s','--size', metavar='N', type=int, nargs='+',
                    help="Matrices's sizes to be used in the tests.")
parser.add_argument('-i','--instances', metavar='N', type=int, nargs='+',
                    help="Number of Processes and Threads to use.")
args = parser.parse_args()
if args.size is None:
    sizes = list()
else:
    sizes = args.size
if args.ntest is None:
    numberOfRepetions = list()
else:
    numberOfRepetions = args.ntest[0]
if args.instances is None:
    instances = list()
else:
    instances = args.instances
print 'sizes', sizes
print 'numberOfRepetions', numberOfRepetions
print 'instances', instances

# Creates directory to save test's results if it doesn't exist
testsDir = os.path.join('./', 'tests-output')
if os.path.exists(testsDir):
    print testsDir
else:
    os.mkdir(testsDir)

# Run tests for each size of matrix - To simplify we use only square matreices
for i in sizes:
    s = str(i)
    subprocess.call(['./matgen', s, s, s, s])
    # Run tests for each number of instances
    for i in instances:
        n = str(i)
        testname = '_' + str(s) + '_' + str(i) + '_' + str(numberOfRepetions) + '.txt'
        logFileThread = os.path.join(testsDir,'t' + testname)
        logFileProcess = os.path.join(testsDir,'p' + testname)
        # Do the same a certain number of times
        for t in range(numberOfRepetions):
            print t
            subprocess.call(['./pmat', '-n', str(i), '-l', logFileProcess])
            subprocess.call(['./tmat', '-n', str(i), '-l', logFileThread])
