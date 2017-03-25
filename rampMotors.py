#!/usr/bin/env python

import time
from simpleAPI import RoboAPI

foo = RoboAPI(40000)
foo.send('mon')
foo.send('setled 4')
foo.send('setservo 1 0')

# ramp up to 1000
for i in range(60):
    cmd = 'setMotor 1 ' + str(10 * i)
    foo.send(cmd)
    cmd = 'setMotor 2 ' + str(10 * i)
    foo.send(cmd)
    cmd = 'setMotor 3 ' + str(10 * i)
    foo.send(cmd)
    cmd = 'setMotor 4 ' + str(10 * i)
    foo.send(cmd)
    time.sleep(0.01)

# now cycle between maximums
while True:
    for i in range(60):
        cmd = 'setMotor 1 ' + str(600 - (20 * i))
        foo.send(cmd)
        cmd = 'setMotor 2 ' + str(600 - (20 * i))
        foo.send(cmd)
        cmd = 'setMotor 3 ' + str(600 - (20 * i))
        foo.send(cmd)
        cmd = 'setMotor 4 ' + str(600 - (20 * i))
        foo.send(cmd)
        time.sleep(0.01)

    for i in range(60):
        cmd = 'setMotor 1 ' + str(-600 + (20 * i))
        foo.send(cmd)
        cmd = 'setMotor 2 ' + str(-600 + (20 * i))
        foo.send(cmd)
        cmd = 'setMotor 3 ' + str(-600 + (20 * i))
        foo.send(cmd)
        cmd = 'setMotor 4 ' + str(-600 + (20 * i))
        foo.send(cmd)
        time.sleep(0.01)

