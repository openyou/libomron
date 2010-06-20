#!/usr/bin/env python

import sqlite3, os, sys, time

def ymdhms2seconds(YY,MM,DD,hh=0,mm=0,ss=0):
    format = "%y %m %d %H %M %S"
    t = time.strptime("%d %d %d %d %d %d"%(YY,MM,DD,hh,mm,ss),format)
    return time.mktime(t)

class Data(object):
    def __init__(self,filename="omron.sqlite3",tablename="omron"):
        self.tablename = tablename
        self.conn = sqlite3.connect(filename)

        try:
            self.initdb(tablename)
        except sqlite3.OperationalError:
            print 'Using existing %s:%s'%(filename,tablename)
        else:
            print 'Made new %s:%s'%(filename,tablename)
        return

    def initdb(self,tablename):
        c = self.conn.cursor()
        c.execute('''create table %s
        (timestamp INTEGER PRIMARY KEY ON CONFLICT REPLACE,
        sys int, dia int, pulse int, type int)'''%tablename)
        self.conn.commit()
        c.close()
        return

    def add(self,timestamp,sys,dia,pulse,by_day = True):
        if by_day: by_day = 0
        else: by_day = 1
        c = self.conn.cursor()
        c.execute('''replace into %s values (%d,%d,%d,%d,%d)'''%\
                      (self.tablename,timestamp,sys,dia,pulse,by_day))
        self.conn.commit()
        c.close()
        return

    def all(self,by_day = True):
        if by_day: by_day = 0
        else: by_day = 1
        c = self.conn.cursor()
        c.execute('select * from %s where type = %d order by timestamp'%\
                      (self.tablename,by_day))
        ret = []
        for row in c:
            ret.append(row)
        c.close()
        return ret

    pass

if '__main__' == __name__:
    d = Data()
    rows = d.all()
    for row in rows: print row
