.PHONY:clean
CC=g++
CPPFLAGS=-Wall -g
BIN=echosrv_poll echocli echosrv_epoll
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)
