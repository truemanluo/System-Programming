.PHONY: clean all
XX =g++
XFLAGS =-Wall -g
BIN =echoserver echoclient
all: $(BIN)
%.o:%.cpp
	$(XX) $(XFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)
