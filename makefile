EXECS=buffered
RUNS=exec
MPICC?=mpic++

all: ${EXECS} ${RUNS}

buffered: buffered.c
	${MPICC} -o buffered buffered.c

exec:
	mpirun -n 4 ./buffered

clean:
	rm -f ${EXECS}
