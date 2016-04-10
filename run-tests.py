#!/bin/python

import os
import sys
import subprocess

try:
    # make a regular expression to parse the input
    progName = sys.argv[1]

except:

# Number of times to run each program
try:
    m = int(sys.argv[1])
except:
    print 'Run command like this: ./report.py <numver tests to run>'
    sys.exit(0)

if os.path.Exists('avarage.txt'):

for i in range(m):
    subprocess.call(["./processmat", "4"])

fhand = open(fname,"r")
