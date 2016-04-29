all: matgen pmat tmat


matgen : matgen.c
		gcc -o matgen matgen.c
pmat : pmat.c
		gcc -o pmat pmat.c
tmat : tmat.c
		gcc -o tmat tmat.c -pthread

clean:
		rm matgen pmat tmat
