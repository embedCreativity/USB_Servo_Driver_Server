LIBS += -lpthread

socketToSerial: socketToSerial.o SerialControl.o
	$(CC) $(LDFLAGS) socketToSerial.o SerialControl.o -o socketToSerial $(LIBS)
socketToSerial.o: socketToSerial.c
	$(CC) $(CFLAGS) -c socketToSerial.c

SerialControl.o: SerialControl.c
	$(CC) $(CFLAGS) -c SerialControl.c

# remove object files and executable when user executes "make clean"
clean:
	rm *.o socketToSerial


