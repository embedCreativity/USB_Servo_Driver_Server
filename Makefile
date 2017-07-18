ProgramName := socketToSerial

LIBS += -pthread
#LIBS += -lsocket -lec_serial -lsqlite3
CXXFLAGS += -std=c++11 -g -Wall
#CFLAGS += -std=c++11 -pthread -g -Wall
LDFLAGS += -L/usr/lib

main: main.o commManager.o socketInterface.o watchDog.o batteryMonitor.o sqlUpdater.o pubsub.o
	$(CXX) $(LDFLAGS) main.o commManager.o socketInterface.o watchDog.o batteryMonitor.o sqlUpdater.o pubsub.o -o $(ProgramName) $(LIBS)
main.o: main.cc
	$(CXX) $(CXXFLAGS) -c main.cc

commManager: commManager.o
	$(CXX) $(LDFLAGS) commManager.o $(LIBS)
commManager.o: commManager.cc
	$(CXX) $(CXXFLAGS) -c commManager.cc

socketInterface: socketInterface.o
	$(CXX) $(LDFLAGS) socketInterface.o $(LIBS)
socketInterface.o: socketInterface.cc
	$(CXX) $(CXXFLAGS) -c socketInterface.cc

pubsub: pubsub.o
	$(CXX) $(LDFLAGS) pubsub.o $(LIBS)
pubsub.o: pubsub.cc
	$(CXX) $(CXXFLAGS) -c pubsub.cc

watchDog: watchDog.o
	$(CXX) $(LDFLAGS) watchDog.o $(LIBS)
watchDog.o: watchDog.cc
	$(CXX) $(CXXFLAGS) -c watchDog.cc

batteryMonitor: batteryMonitor.o
	$(CXX) $(LDFLAGS) batteryMonitor.o $(LIBS)
batteryMonitor.o: batteryMonitor.cc
	$(CXX) $(CXXFLAGS) -c batteryMonitor.cc

sqlUpdater: sqlUpdater.o
	$(CXX) $(LDFLAGS) sqlUpdater.o $(LIBS)
sqlUpdater.o: sqlUpdater.cc
	$(CXX) $(CXXFLAGS) -c sqlUpdater.cc

all: $(ProgramName)

install: $(ProgramName)
	sudo cp --preserve=timestamps $(ProgramName) /usr/local/bin/

# remove object files and executable when user executes "make clean"
clean:
	rm -f *.o socketToSerial


