#!/usr/bin/python3.6

import sys
with sys.stdin as f:
    for i in f.readlines():
        x, y, filename = i.strip().split(" ")
        if not (int(x) < 15 and int(y) < 15):
            print(i.strip())
