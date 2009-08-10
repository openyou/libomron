========
libomron
========

by Kyle Machulis <kyle@nonpolynomial.com>
Nonpolynomial Labs - http://www.nonpolynomial.com

Thanks to all project contributors

===========
Description
===========

libomron is an implementation of the data retreival protocol for the USB enabled line of omron home medical equipment. Currently, this includes:

HEM-790-IT Blood Pressure Monitor

There are also plans to include:

HJ-720-IT Pedometer

Once we figure out the protocol

=================
Development Notes
=================

Please note that this library is in the very early stages of development, and may change at any time.

Protocol documentation for what is known so far is available in the doc directory. There are also logs, in USBlyzer and SniffUSB format, for the different monitors.

============
Requirements
============

------
libusb
------

v0.1x series, v0.12 or later. Not v1.0 compatible

http://libusb.sourceforge.net

