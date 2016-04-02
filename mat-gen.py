#!/bin/python
import random
import sys

# Linhas
m = int(sys.argv[1])
# Colunas
n = int(sys.argv[2])
#file to save the matrix
fname = sys.argv[3]

# 
mat = [[random.randint(0, 10) for x in range(n)] for x in range(m)]
out = ""
out += "LINHAS = %d" % int(sys.argv[1])
out += '\n'
out += "COLUNAS = %d" % int(sys.argv[2])
out += "\n"
# makes the matrix a string to be saved in the file
for row in mat:
    for num in row:
        out += "%d " % num
    out += "\n"

with open(fname,"w") as file:
     file.write(out)
