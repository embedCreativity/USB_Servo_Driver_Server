LIBS += -lpthread

servoServer: servoServer.o SerialControl.o serialthread.o
	$(CC) $(LDFLAGS) serialthread.o servoServer.o SerialControl.o -o servoServer $(LIBS)
servoServer.o: servoServer.c
	$(CC) $(CFLAGS) -c servoServer.c

SerialControl.o: SerialControl.c
	$(CC) $(CFLAGS) -c SerialControl.c

serialthread.o: serialthread.c
	$(CC) $(CFLAGS) -c serialthread.c

# remove object files and executable when user executes "make clean"
clean:
	rm *.o servoServer


