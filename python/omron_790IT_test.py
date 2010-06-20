#!/usr/bin/env python

import os, sys
import omron, store, device

print 'Using',device.device()

ret = omron.get_count(omron.OMRON_790IT_VID,omron.OMRON_790IT_PID)
if not ret:
    print 'No omron 790ITs connected!'
    sys.exit(1)
print 'Found %d omron 790ITs'%ret

o = omron.Omron()
print 'Openning'
ret = o.open(omron.OMRON_790IT_VID, omron.OMRON_790IT_PID)
if ret < 0:
    print 'Cannot open omron 790IT'
    sys.exit(1)

print 'Getting device version'
ret = o.get_device_version()
if not ret:
    print 'Cannot get device version!'
    sys.exit(1)
print 'Device version: "%s"'%ret

ret = o.get_bp_profile()
if not ret:
     print 'Cannot get device prf!'
     sys.exit(1)
print 'Device version: %s'%ret

data_count = o.get_daily_data_count();
print 'AJR data count: %d'%data_count
if data_count < 0:
    print 'Cannot get device prf!'

def bad_data(r):
    return not r.day and not r.month and not r.year and \
        not r.hour and not r.minute and not r.second and \
        not r.sys and not r.dia and not r.pulse


d = store.Data()
for ind in range(data_count-1,-1,-1):
    for trial in range(3):
        r = o.get_daily_bp_data(ind)
        if bad_data(r): continue
        break

    ts = store.ymdhms2seconds(r.year,r.month,r.day,r.hour,r.minute,r.second)
    print trial, ts,r.sys,r.dia,r.pulse

    if bad_data(r): continue

    d.add(ts,r.sys,r.dia,r.pulse)

    continue



