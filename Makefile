INCLUDES = rench.h

all: rench

%.o: %.c $(INCLUDES)
	gcc -c -o $@ $<

rench: rench.o options.o
	gcc rench.o options.o -o rench
