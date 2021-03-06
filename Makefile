INCLUDES = rench.h
OBJS = rench.o options.o producer.o consumer.o buffer.o fops.o task.o

all: rench

%.o: %.c $(INCLUDES)
	gcc -c -o $@ $<

rench: $(OBJS)
	gcc $(OBJS) -o rench -lpthread -lgfapi -lrados
