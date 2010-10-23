#!/usr/bin/env python
'''
Gui for reading and plotting out blood pressure values via libomron.
'''


import Tkinter as tk
import matplotlib 
matplotlib.use('TkAgg') 

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure 

def format_date(seconds, pos=None):
    import time
    return time.strftime('%Y-%m-%d', time.localtime(seconds))

class Gui(object):
    'A simple gui for omron'
    def __init__(self, master, name=None, dbfile=None):
        self.master = master
        print name, dbfile
        # import omron
        # self.omron = omron.Omron()
        self.omron = None

        self.name = name

        self.data = None
        self.use_averaged = False
        self.use_am_pm = False
        self.dbfile = dbfile
        if self.dbfile: self.cb_open_file()

        # frame = tk.Frame(master)
        # frame.pack()

        menubar = tk.Menu(master)
        menubar.add_command(label="Quit", command = master.quit)
        menubar.add_command(label="Open", command = self.cb_open_file)
        menubar.add_command(label="Plot", command = self.cb_update_plot)
        menubar.add_command(label="Acquire", command = self.cb_acquire)
        menubar.add_command(label="Avg", command = self.cb_average)
        menubar.add_command(label="AM/PM", command = self.cb_am_pm)
        self.menubar = menubar

        #menubar.pack()

        master.config(menu=menubar)

        self.figure = Figure(figsize = (5,4), dpi = 100)
        self.canvas = FigureCanvasTkAgg(self.figure, master = master)
        self.canvas.show() 
        self.canvas.get_tk_widget().pack(side = tk.TOP, fill = tk.BOTH,
                                         expand = 1) 

        self.cb_update_plot()

        return

    def get_data(self):
        '''Return the current data to plot.  Return a list of tuples.
        First value in each tuple is the time followed by 3
        measurements (sys,dia,pulse)'''
        if self.use_averaged:
            return self.average()
        return self.data.all()

    def cb_open_file(self):
        'If given a file, open it, else prompt user'
        import store
        if self.dbfile:
            print 'Loading DB file %s using name %s' % (self.dbfile, self.name)
            self.data = store.Data(self.dbfile, self.name)
            return

        from tkFileDialog import askopenfilename
        self.dbfile = askopenfilename()
        if not self.dbfile: 
            print 'Canceled'
            return
        return self.cb_open_file()
            
    def cb_update_plot(self):
        'Update the plot given current parameters'
        rows = self.get_data()
        if not rows: return

        import matplotlib.ticker as ticker
        import time

        # Convert row based to column based
        ts1 = list()
        sys1 = list()
        dia1 = list()
        pulse1 = list()
        if self.use_am_pm:      # these will be pm
            ts2 = list()
            sys2 = list()
            dia2 = list()
            pulse2 = list()
            pass

        # sort out data
        for row in rows: 
            if self.use_am_pm:
                hour = time.localtime(row[0])[3]
                if hour < 12:
                    ts1.append(row[0])
                    sys1.append(row[1])
                    dia1.append(row[2])
                    pulse1.append(row[3])
                else:
                    ts2.append(row[0])
                    sys2.append(row[1])
                    dia2.append(row[2])
                    pulse2.append(row[3])
                    pass
                continue
            ts1.append(row[0])
            sys1.append(row[1])
            dia1.append(row[2])
            pulse1.append(row[3])
            continue
        
        self.figure.clear()
        axis = self.figure.add_subplot(111)
        axis.plot(ts1, sys1, 'k', ts1, dia1, 'b', ts1, pulse1, 'r')
        if self.use_am_pm:
            axis.plot(ts2, sys2, 'k--', ts2, dia2, 'b--', ts2, pulse2, 'r--')
        axis.xaxis.set_major_formatter(ticker.FuncFormatter(format_date))
        self.figure.autofmt_xdate()
        self.canvas.show() 

        return

    def cb_acquire(self): 
        print 'Acquiring data'
        import omron
        import store

        omron_obj = omron.Omron()

        if omron_obj.open(omron.OMRON_790IT_PID, omron.OMRON_790IT_VID) < 0:
            print 'Failed to open device'
            return

        ret = omron_obj.get_device_version()
        if not ret: print 'Failed to get device version'
        else: print 'Opened device version %s' % ret

        ret = omron_obj.get_bp_profile()
        if not ret: print 'Failed to get device profile'
        else: print 'Opened device with profile: %s' % ret

        data_count = omron_obj.get_daily_data_count()
        print 'Trying to get %d readings' % data_count

        def bad_data(data):
            return not data.day and not data.month and not data.year and \
                not data.hour and not data.minute and not data.second and \
                not data.sys and not data.dia and not data.pulse

        for ind in range(data_count-1, -1, -1):
            for trial in range(3):
                bp_data = omron_obj.get_daily_bp_data(ind)
                if bad_data(bp_data): continue
                break
            tsec = store.ymdhms2seconds(bp_data.year, bp_data.month,
                                        bp_data.day, bp_data.hour,
                                        bp_data.minute, bp_data.second)
            print trial, tsec, bp_data.sys, bp_data.dia, bp_data.pulse

            if bad_data(bp_data): continue

            self.data.add(tsec, bp_data.sys, bp_data.dia, bp_data.pulse)
            continue

        self.cb_update_plot()
        return

    def average(self, window=600, data=None):
        'Respond to Avg button, average nearby data in window (seconds)'
        if data is None:
            rows = self.data.all()
        else:
            rows = data
        new_rows = []
        count = 0
        last_time = sys_sum = dia_sum = pulse_sum = 0
        for row in rows:
            delta_t = row[0] - last_time
            last_time = row[0]
            if delta_t > window: # save accumulated and start new with current
                if count:       # first time through
                    new_rows.append((last_time, sys_sum/count, dia_sum/count,
                                     pulse_sum/count))
                count = 1
                sys_sum = row[1]
                dia_sum = row[2]
                pulse_sum = row[3]
                continue
            # we are inside window of last data, accumulate
            count += 1
            sys_sum += row[1]
            dia_sum += row[2]
            pulse_sum += row[3]
            continue
        print 'Averaged %d values down to %d' % (len(rows), len(new_rows))
        return new_rows

    def cb_average(self):
        'Toggle Avg/All'
        if self.use_averaged:
            label = 'Avg'
        else:
            label = 'All'
        self.menubar.entryconfig(5, label=label)
        self.use_averaged = not self.use_averaged 
        self.cb_update_plot()
        return

    def cb_am_pm(self):
        'Toggle AM/PM and Combined'
        if self.use_am_pm:
            label = 'AM/PM'
        else:
            label = 'Full'
        self.menubar.entryconfig(6, label=label)
        self.use_am_pm = not self.use_am_pm
        self.cb_update_plot()
        return

    pass


class Main(object):
    def __init__(self, argv=None):
        import optparse
        parser = optparse.OptionParser()
        parser.add_option('-f', '--file', type = 'string', default = None,
                          help = 'Set the SQLite3 data file')
        parser.add_option('-n', '--name', type = 'string', default = 'omron',
                          help = 'Set a short name to distinguish the ' \
                               + 'data between individuals')
        parser.add_option('-d', '--device', type = 'string', default = None,
                          help = 'Set the USB device file, ' \
                               + 'overrides OMRON_DEV environment variable')
        self.parser = parser
        if argv:
            self.parse_args(argv)
        return

    def parse_args(self, argv):
        self.opts, self.args = self.parser.parse_args(argv)
        return

    pass

def run_app():
    'Run the application'
    import sys, os
    main_obj = Main(sys.argv)
    print main_obj.opts
    if main_obj.opts.device:
        os.environ['OMRON_DEV'] = main_obj.opts.device

    import device
    omron_dev = device.device()
    if omron_dev:
        print 'Using device: "%s"' % omron_dev
    else:
        print 'No device autodetected, try running "lsusb" and ' \
            + 'setting OMRON_DEV=/dev/bus/usb/BBB/DDD'
        pass
    root = tk.Tk()
    app = Gui(root, main_obj.opts.name, main_obj.opts.file)
    root.mainloop()

if '__main__' == __name__:
    run_app()
