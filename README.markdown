# libomron

by Kyle Machulis

[Nonpolynomial Labs](http://www.nonpolynomial.com)

Thanks to all project contributors

## Description

libomron is an implementation of the data retreival protocol for the USB enabled line of omron home medical equipment. Currently, this includes:

* HEM-790-IT Blood Pressure Monitor
* HJ-720-IT Pedometer

While basic communication functions in the library should work for any Omron product, there may be differences in packet layout and measure units between US/European/Japanese/etc products... We will try to cover these as requested, but the library itself is produced in the US, so that's what I have easiest access to.

## Development Notes

Please note that this library is in the very early stages of development, and may change at any time.

Protocol documentation for what is known so far is available in the doc directory. There are also logs, in USBlyzer and SniffUSB format, for the different monitors.

## Library Requirements

### libusb

v0.1x series, v0.12 or later. Not v1.0 compatible yet.

[libusb website](http://libusb.sourceforge.net)

