ProgramName := socketToSerial

LIBS += -lsocket -lec_serial -lsqlite3 -pthread
#LIBS += -lsocket -lec_serial -lsqlite3
CXXFLAGS += -std=c++11 -g -Wall
#CFLAGS += -std=c++11 -pthread -g -Wall
LDFLAGS += -L/usr/lib

main: main.o commManager.o socketInterface.o watchDog.o batteryMonitor.o sqlUpdater.o pubsub.o types.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) main.o commManager.o socketInterface.o watchDog.o batteryMonitor.o sqlUpdater.o pubsub.o types.o -o $(ProgramName) $(LIBS)
main.o: main.cc
	$(CXX) $(CXXFLAGS) -c main.cc

commManager: commManager.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) commManager.o $(LIBS)
commManager.o: commManager.cc
	$(CXX) $(CXXFLAGS) -c commManager.cc

socketInterface: socketInterface.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) socketInterface.o $(LIBS)
socketInterface.o: socketInterface.cc
	$(CXX) $(CXXFLAGS) -c socketInterface.cc

pubsub: pubsub.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) pubsub.o $(LIBS)
pubsub.o: pubsub.cc
	$(CXX) $(CXXFLAGS) -c pubsub.cc

types: types.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) types.o $(LIBS)
types.o: types.cc
	$(CXX) $(CXXFLAGS) -c types.cc

watchDog: watchDog.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) watchDog.o $(LIBS)
watchDog.o: watchDog.cc
	$(CXX) $(CXXFLAGS) -c watchDog.cc

batteryMonitor: batteryMonitor.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) batteryMonitor.o $(LIBS)
batteryMonitor.o: batteryMonitor.cc
	$(CXX) $(CXXFLAGS) -c batteryMonitor.cc

sqlUpdater: sqlUpdater.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) sqlUpdater.o $(LIBS)
sqlUpdater.o: sqlUpdater.cc
	$(CXX) $(CXXFLAGS) -c sqlUpdater.cc

all: $(ProgramName)

install: $(ProgramName)
	sudo cp --preserve=timestamps $(ProgramName) /usr/local/bin/

# remove object files and executable when user executes "make clean"
clean:
	rm -f *.o socketToSerial


