---
layout: post
project: libomron
pledgie: 14372
title: Alive again. Sorta.
---

I'm getting a steady 1-3 emails a week about this library, so I kinda feel like I should get my act together and actually get this to a 1.0 release. My main focus at the moment is going to be getting the libusb based core solid (I've figured out how to work around the need for libhid), and making sure it works on linux and os x. The windows code, however, I'm somewhat worried about. In the move from the DDK to the WDK, some of the HID headers and functions I used disappeared, and I really don't enjoy the idea of having to go back and fix that. So, we'll see what happens there.
