#!/usr/bin/env python

from simpleAPI import RoboAPI

foo = RoboAPI(49000)
foo.send('mon')
foo.send('setled 4')
foo.send('setservo 1 0')

while True:
    for i in range(10):
        cmd = 'setMotor 1 ' + str(100 * i)
        foo.send(cmd)

    for i in range(10):
        cmd = 'setMotor 1 ' + str(1000 - (100 * i))
        foo.send(cmd)
