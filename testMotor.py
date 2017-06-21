#!/usr/bin/env python

import time
from embedcreativityAPI import PalmettoAPI

foo = PalmettoAPI()
foo.send('mon')
foo.send('setled 4')
foo.send('setservo 1 0')

# ramp up to 1000
for i in range(80):
    cmd = 'setMotor 4 ' + str(10 * i)
    foo.send(cmd)
    time.sleep(0.06)

# now cycle between maximums
while True:
    for i in range(80):
        cmd = 'setMotor 4 ' + str(800 - (20 * i))
        foo.send(cmd)
        cmd = 'setLED ' + str(10 * i)
        foo.send(cmd)
        time.sleep(0.06)

    for i in range(80):
        cmd = 'setMotor 4 ' + str(-800 + (20 * i))
        foo.send(cmd)
        cmd = 'setLED ' + str(800 - (10 * i))
        foo.send(cmd)
        time.sleep(0.06)

