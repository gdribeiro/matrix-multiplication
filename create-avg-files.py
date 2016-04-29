#!/bin/python
import os
import sys
import subprocess
import math

def calcAvg(filePath):
    s = 0
    lineCount = 0
    f = open(filePath, 'r')
    for line in f:
        lineCount = lineCount + 1
        s = s + int(line)
    avg = s / lineCount
    return avg

######## Main Stuff ###########
# Path Names Definition
fromDirName = 'tests-output'
toDirName = 'avg-files'
fromDir = os.path.join(os.getcwd(), fromDirName)
toDir = os.path.join(os.getcwd(), toDirName)

for dirpath, dirnames, filenames in os.walk(fromDir):
    for f in filenames:
        sName = f.split('_')
        avgFileName = sName[0] + '_' + sName[1] + '.txt'
        # Create Average File if it doesn't exists yet
        # if not (os.path.exists(os.path.join(toDir, avgFileName))):
        # Open file to append data or creates it if it doesn't exist
        avghand = open(os.path.join(toDir, avgFileName), 'a')

        avg = calcAvg(os.path.join(dirpath, f))

        line = sName[2] + ' ' + str(avg) + '\n'
        avghand.write(line)

# Mas que Ganbiarrera!!!!
for dirpath, dirnames, filenames in os.walk(toDir):
    for f in filenames:
        lst1 = list()
        lst2 = list()
        lst3 = list()
        avghand = open(os.path.join(toDir, f), 'r')
        for line in avghand:
            lst1.append(line.strip())
        for i in lst1:
            lst2.append(i.split(' '))
        print lst2
        for a,b in lst2:
            lst3.append([int(a), int(b)])
        print lst3
        lst3.sort()
        print lst3

        newFile = open(os.path.join(toDir, f), 'w')
        for line in lst3:
            newFile.write(str(line[0]) + ' ' + str(line[1]) + '\n')
