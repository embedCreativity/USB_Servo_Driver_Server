#!/usr/bin/env python

import time
from embedcreativityAPI import PalmettoAPI

foo = PalmettoAPI()

while True:
    for i in range(100):
        cmd = 'setLED ' + str(10 * i)
        foo.send(cmd)
        time.sleep(0.01)

    for i in range(100):
        cmd = 'setLED ' + str(1000 - (10 * i))
        foo.send(cmd)
        time.sleep(0.01)

