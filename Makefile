ProgramName := socketToSerial

LIBS += -lsocket -lec_serial -lpthread -lsqlite3
CFLAGS += -g -Wall
LDFLAGS += -L/usr/lib

socketToSerial: socketToSerial.o
	$(CC) $(CFLAGS) $(LDFLAGS) socketToSerial.o -o socketToSerial $(LIBS)
socketToSerial.o: socketToSerial.c
	$(CC) $(CFLAGS) -c socketToSerial.c

setDefaults: setDefaults.o
	$(CC) setDefaults.o -o setDefaults
setDefaults.o: setDefaults.c
	$(CC) $(CFLAGS) -c setDefaults.c

all: $(ProgramName) setDefaults

install: $(ProgramName)
	sudo cp --preserve=timestamps $(ProgramName) /usr/local/bin/
	sudo cp --preserve=timestamps setDefaults /usr/local/bin/

# remove object files and executable when user executes "make clean"
clean:
	rm -f *.o socketToSerial setDefaults


