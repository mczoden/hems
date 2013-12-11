CC = gcc
BIN = cpe_kpi_emu
OBJS = main.o
CFLAGS += -Wall
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
