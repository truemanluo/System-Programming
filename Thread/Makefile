.PHONY: clean all
CC =gcc
CFLAGS =-Wall -g
BIN =consumer_producer consumer_producer_cond
all: $(BIN)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
consumer_producer:consumer_producer.o
	$(CC) $(CFLAGS) $< -o $@ -lpthread
consumer_producer_cond:consumer_producer_cond.o
	$(CC) $(CFLAGS) $< -o $@ -lpthread
clean:
	rm -f *.o $(BIN)
