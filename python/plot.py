#!/usr/bin/env python

def plot(ts,sys,dia,pulse):

    import numpy as np
    import matplotlib.pyplot as plt
    import matplotlib.mlab as mlab
    import matplotlib.ticker as ticker
    import time

    def format_date(x, pos=None):
        return time.strftime('%Y-%m-%d',time.localtime(x))

    # first we'll do it the default way, with gaps on weekends
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.plot(ts, sys, 'k-', ts, dia, 'b-', ts, pulse, 'r')
    ax.xaxis.set_major_formatter(ticker.FuncFormatter(format_date))
    fig.autofmt_xdate()

    # # next we'll write a custom formatter
    # N = len(ts)
    # ind = np.arange(N)  # the evenly spaced plot indices

    # def format_date(x, pos=None):
    #     return time.strftime('%Y-%m-%d',time.mktime(x))

    # fig = plt.figure()
    # ax = fig.add_subplot(111)
    # ax.plot(ts, sys, 'k', ts, dia, 'b', ts, pulse, 'r')
    # ax.xaxis.set_major_formatter(ticker.FuncFormatter(format_date))
    # fig.autofmt_xdate()
    
    plt.show()

if '__main__' == __name__:
    import store
    d = store.Data()
    rows = d.all()
    timestamp = list()
    sys = list()
    dia = list()
    pulse = list()
    for row in rows: 
        timestamp.append(row[0])
        sys.append(row[1])
        dia.append(row[2])
        pulse.append(row[3])

    plot(timestamp,sys,dia,pulse)

