CC = gcc
BIN = cheer
OBJS =  main.o gen.o uld.o dbg.o cfg.o
CFLAGS += -Wall
CFLAGS += -g
#CFLAGS += -O3
LIBS += -lcurl -lpthread

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN) *.o

.phony:
	all clean
