#include <libusb.h>

#include "omron.h"

static usb_dev_handle *libusb_dev_handle;
static usb_device *libusb_dev;

#define OMRON_USB_INTERFACE	0

int omron_get_count(int VID, int PID)
{
	/* STUB */
}

int omron_open(omron_device* dev, int VID, int PID, unsigned int device_index)
{
	/* STUB */
	usb_claim_interface(libusb_dev, OMRON_USB_INTERFACE);
	libusb_dev_handle = libusb_open(libusb_dev);
}

int omron_close(omron_device* dev)
{
	/* STUB */
}
int omron_set_mode(omron_device* dev, omron_mode mode)
{
	/* STUB */
}

int omron_read_data(omron_device* dev)
{
	/* STUB */
}

int omron_write_data(omron_device* dev)
{
	/* STUB */
}

