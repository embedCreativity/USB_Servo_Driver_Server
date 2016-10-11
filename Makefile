LIBS += -lpthread

servoServer: servoServer.o SerialControl.o serialthread.o Console.o
	$(CC) $(LDFLAGS) serialthread.o servoServer.o SerialControl.o Console.o -o servoServer $(LIBS)
servoServer.o: servoServer.c
	$(CC) $(CFLAGS) -c servoServer.c

SerialControl.o: SerialControl.c
	$(CC) $(CFLAGS) -c SerialControl.c

serialthread.o: serialthread.c
	$(CC) $(CFLAGS) -c serialthread.c

Console.o: Console.c
	$(CC) $(CFLAGS) -c Console.c

# remove object files and executable when user executes "make clean"
clean:
	rm *.o servoServer


