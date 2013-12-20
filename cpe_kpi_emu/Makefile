CC = gcc
BIN = cheer
OBJS =  main.o gen.o uld.o dbg.o cfg.o
CFLAGS += -Wall
CFLAGS += -g
CFLAGS += -O3
#CFLAGS += -ansi -std=c99 -Wpedantic
CFLAGS += -std=c99 -Wpedantic
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
