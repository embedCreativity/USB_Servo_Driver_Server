#!/usr/bin/env python

import time
from embedcreativityAPI import PalmettoAPI

foo = PalmettoAPI()
foo.send('setled 4')

# now cycle between maximums
while True:
    foo.send('moff')
    time.sleep(0.01)
