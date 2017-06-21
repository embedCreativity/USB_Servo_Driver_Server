#!/usr/bin/env python

import socket
import sys
import time

TCP_IP = '127.0.0.1'
DEFAULT_PORT = 49000
DEFAULT_MSG = 'Go'
PERIOD_BEFORE_CLOSE = 2 # seconds

class PalmettoAPI:

    def __init__(self, port=DEFAULT_PORT):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((TCP_IP, port))
        self.data = ''

    def send(self, cmd=DEFAULT_MSG):
        self.sock.send(str(cmd))
        #time.sleep(0.001)
        self.data = self.sock.recv(100)
        try:
            values = self.data.split(',')
            voltage = (int(values[1],16) / 4096.0) * 19.8
            # 200X voltage amplifier coming off of 0.0003Ohm current sense resistor
            # V = IR ->  I = V/R = (200X)/(0.0003), where 200X will be between
            # 0V to 3.3V. Thus, 0V = 0A and 3.3V = 200X where X = 3.3/200 = 0.0165V
            # and 0.0165V leads to I = (0.0165V)/(0.0003Ohms) = 55Amps.
            # 12-bit ADC result 4095(12-bit max) = 3.3V. Thus for every single bit
            # in ADC result, that's an increase of 0.01343101A
            # Benchtop testing shows an offset of 158mA in idle output
            adcCurrent = max(0, int(values[2],16))
            current = (float(adcCurrent)) * (55.0/4096.0)
            print 'Battery: {:.2f}V, {:.3f}A'.format(voltage, current)
        except:
            print "ERROR: parsing response"

    def close(self):
        self.sock.close()

if (__name__ == '__main__' ):
    if len(sys.argv) != 3:
        print "usage: " + str(sys.argv[0]) + " <port> <command>"
        quit()

    try:
        port = int(sys.argv[1])
    except:
        print "usage: " + str(sys.argv[0]) + " <port> <command>"
        quit()

    foo = PalmettoAPI(port)
    foo.send(sys.argv[2])
    time.sleep(PERIOD_BEFORE_CLOSE)
    foo.close()

