#!/bin/python

import os
import sys
import subprocess
# For data analysing
import pandas as pd

# Pandas settings
pd.set_option('display.mpl_style', 'default')
# figsize(15,5)

fname = 'avarage.txt'

# Number of times to run each program
try:
    m = int(sys.argv[1])
except:
    print 'Run command like this: ./report.py <numver tests to run>'
    sys.exit(0)

# if os.path.Exists('avarage.txt'):

# for i in range(m):
#     subprocess.call(["./processmat", "4"])
# lst = list()
# fhand = open(fname,"r")
# for line in fhand:
#      lst.append(int(line))
#
# print lst

pf = pd.read_table('./avarage.txt', sep='\n')
pf.plot()
a = raw_input()
