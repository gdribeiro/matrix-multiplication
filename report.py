#!/bin/python

import os
import sys
import subprocess
# For data analysing
import pandas as pd

# Pandas settings
pd.set_option('display.mpl_style', 'default')
# figsize(15,5)

fname = './tests-output/t_400_2_100.txt'

# if os.path.Exists('avarage.txt'):

# for i in range(m):
#     subprocess.call(["./processmat", "4"])
# lst = list()
# fhand = open(fname,"r")
# for line in fhand:
#      lst.append(int(line))
#
# print lst

pf = pd.read_table(fname, sep='\n')
pf.plot()
a = raw_input()
