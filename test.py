#!/usr/bin/env python

import socket
import sys
import time

TCP_IP = '127.0.0.1'
DEFAULT_PORT = 54321
DEFAULT_MSG = 'Go'
PERIOD_BEFORE_CLOSE = 2 # seconds

class Test:

    def __init__(self, port=DEFAULT_PORT):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((TCP_IP, port))
        self.data = ''

    def send(self, cmd=DEFAULT_MSG):
        self.sock.send(str(cmd))
        time.sleep(0.1)
        self.data = self.sock.recv(100)
        print 'Received ' + str(len(self.data)) + ' bytes in response'
        print '--> ' + str(self.data)

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

    foo = Test(port)
    foo.send(sys.argv[2])
    time.sleep(PERIOD_BEFORE_CLOSE)
    foo.close()

