# All Targets
all: encoder

encoder: encoder.o
	gcc -m32 -g -Wall -o encoder encoder.o

encoder.o: encoder.c
	gcc -m32 -g -Wall -c -o encoder.o encoder.c 

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o encoder
 
