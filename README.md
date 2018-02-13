# PalmettoMega's Host Daemon Service

This is the software that needs to run on your RaspberryPi/TinkerBoard/PC/Linux-thingy host machine. The "Host" is a Linux-based machine or embedded single-board computer, which is connected to your PalmettoMega via the USB connection. The "Host" runs this service and provides connectivity with its network interface (WiFi/Ethernet) for the user to connect to.

The "Host" service opens a socket which takes PalmettoAPI command strings in as input, then translates them to the binary TLV (type-length-value) commands the PalmettoMega is expecting. The PalmettoMega acknowledges the command by responding with power information.

