#!/usr/bin/env python

import os

def device():
    'Return omron device file name'
    try:
        return os.environ['OMRON_DEV']
    except KeyError:
        pass

    from subprocess import Popen, PIPE
    stuff = Popen(["lsusb"],stdout=PIPE).communicate()[0]
    bus=None; dev=None
    for line in stuff.split('\n'):
        if not 'Omron' in line: continue
        line = line.split()
        bus = line[1][:3]
        dev = line[3][:3]
        break
    if bus is None or dev is None: return ""
    devfile='/dev/bus/usb/%s/%s'%(bus,dev)
    os.environ['OMRON_DEV'] = devfile
    return devfile

if '__main__' == __name__:
    print device()
