---
layout: post
project: libomron
pledgie: 14378
title: Investing Money
---

Ok, I finally broke down and bought an HEM-790IT Blood Pressure Monitor, to make sure I could keep the blood pressure monitor core of libomron together. So, I've actually become financially invested in this project now. If you'd like to offset some of that cost and keep my motivation level up, feel free to press the button below:

<a href='http://www.pledgie.com/campaigns/10066'><img alt='Click here to lend your support to: libomron and make a donation at www.pledgie.com !' src='http://www.pledgie.com/campaigns/10066.png?skin_name=chrome' border='0' /></a>

Things continue to progress on the libusb branch of development. This is the first time I've had the pedometer and the blood pressure monitor together in the same place at the same time, so I can actually test how both work. Since they both register as the same VID and PID (Damn you, cheap hardware vendors!), first order of business is writing a tester that detects what device you're using (using the serial number) and routes calls for that correctly.
