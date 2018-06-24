all: sim

sim: simulation.c
	gcc -o sim simulation.c -std=c99
