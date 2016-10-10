CC	= cc
CFLAGS	= -O2 --std=c99 -Wall

driver: driver.o ringbuffer.o
	$(CC) -o driver driver.o ringbuffer.o -lpthread

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o driver
