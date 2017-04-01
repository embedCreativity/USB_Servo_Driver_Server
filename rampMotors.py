#!/usr/bin/env python

import time
from embedcreativityAPI import PalmettoAPI

foo = PalmettoAPI()
foo.send('mon')
foo.send('setled 4')
foo.send('setservo 1 0')

# ramp up to 1000
for i in range(100):
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
    for i in range(100):
        cmd = 'setMotor 1 ' + str(1000 - (20 * i))
        foo.send(cmd)
        cmd = 'setMotor 2 ' + str(1000 - (20 * i))
        foo.send(cmd)
        cmd = 'setMotor 3 ' + str(1000 - (20 * i))
        foo.send(cmd)
        cmd = 'setMotor 4 ' + str(1000 - (20 * i))
        foo.send(cmd)
        cmd = 'setServo 1 ' + str(15 * i)
        foo.send(cmd)
        cmd = 'setLED ' + str(3 * i)
        foo.send(cmd)
        time.sleep(0.01)

    for i in range(100):
        cmd = 'setMotor 1 ' + str(-1000 + (20 * i))
        foo.send(cmd)
        cmd = 'setMotor 2 ' + str(-1000 + (20 * i))
        foo.send(cmd)
        cmd = 'setMotor 3 ' + str(-1000 + (20 * i))
        foo.send(cmd)
        cmd = 'setMotor 4 ' + str(-1000 + (20 * i))
        foo.send(cmd)
        cmd = 'setServo 1 ' + str(1500 - (15 * i))
        foo.send(cmd)
        cmd = 'setLED ' + str(300 - (3 * i))
        foo.send(cmd)
        time.sleep(0.01)

