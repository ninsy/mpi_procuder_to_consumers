EXECS=solution
RUNS=exec
MPICC?=mpic++
PROC_COUNT=6

all: ${EXECS} ${RUNS}

solution: better_solution.cpp
	${MPICC} -std=c++11 -o better_solution better_solution.cpp

exec:
	mpirun -n ${PROC_COUNT} ./better_solution

clean:
	rm -f ${EXECS}
