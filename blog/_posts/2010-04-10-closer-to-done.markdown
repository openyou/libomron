---
layout: post
project: libomron
title: Closer to Done
---

So, as of right now, I finally got the libusb driver core verified and working on both linux and os x. Really, this is only required for OS X, but since it works on linux too, I figured why not give it a shot. 

The main problem was getting the OS X HID Manager to give up the device. I've written about this before while working with the [lightstone](http://qdot.github.com/liblightstone), but this time I'd forgotten to take out the bcdDevice field from the kext, and it was causing the device to not be recognized and picked up by the kext. Removed that, and now things are working great. 

I have a LOT of cleanup to do before this gets released, but this is a pretty major step toward v1.0
