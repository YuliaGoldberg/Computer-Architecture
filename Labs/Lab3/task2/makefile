# All Targets
all: task2b

task2b: task2b.o task2b.o
	gcc -m32 -g -Wall -o task2b task2b.o

task2b.o: task2b.c
	gcc -m32 -g -Wall -c -o task2b.o task2b.c 

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o task2b
 
