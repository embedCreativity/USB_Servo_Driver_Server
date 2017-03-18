#!/usr/bin/env python

import time
from simpleAPI import RoboAPI

foo = RoboAPI(49000)

while True:
    for i in range(50):
        cmd = 'setLED ' + str(10 * i)
        foo.send(cmd)
        time.sleep(0.01)

    for i in range(50):
        cmd = 'setLED ' + str(500 - (10 * i))
        foo.send(cmd)
        time.sleep(0.01)

