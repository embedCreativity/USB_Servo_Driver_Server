ProgramName := socketToSerial

LIBS += -lSocket
CFLAGS += -g -Wall -static
LDFLAGS += -L.

socketToSerial: socketToSerial.o SerialControl.o
	$(CC) $(CFLAGS) $(LDFLAGS) socketToSerial.o SerialControl.o -o socketToSerial $(LIBS)
socketToSerial.o: socketToSerial.c
	$(CC) $(CFLAGS) -c socketToSerial.c

SerialControl.o: SerialControl.c
	$(CC) $(CFLAGS) -c SerialControl.c

all: $(ProgramName)

# remove object files and executable when user executes "make clean"
clean:
	rm -f *.o socketToSerial


