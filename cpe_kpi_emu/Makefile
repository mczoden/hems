CC = gcc
BIN = cpe_kpi_emu
OBJS =  main.o gen.o uld.o dbg.o
CFLAGS += -Wall -Werror
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
