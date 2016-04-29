#!/bin/python

import os
import sys
import subprocess
# For data analysing
import pandas as pd

# Pandas settings
pd.set_option('display.mpl_style', 'default')
# figsize(15,5)


######## Main Stuff ###########
fieldnames = ['Class','Own Time (ms)']
classList = list()
# walk down the directories to find the csv files
for dirpath, dirnames, filenames in os.walk('.'):
    for f in filenames:
        classList = list()
        # Tests if the fies we need to use are in the directory
        if os.path.exists(os.path.join(dirpath, mtl)):
            print dirpath
            projCategory = dirpath.split('/')[1]
            projName = dirpath.split('/')[2]
            projectFileName = '../' + 'avg/' + projCategory + '-' + projName + '.csv'
            print projectFileName

            classList = readMethodListCPU_2(dirpath)

            a = raw_input()

            print len(classList)

            with open(projectFileName, 'w') as csvfile:
                writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                writer.writeheader()
                for l in classList:
                    writer.writerow(l)
            classList = []






pf = pd.read_table(fname, sep='\n')
pf.plot()
a = raw_input()
