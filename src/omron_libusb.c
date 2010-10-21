/*
 * Generic function file for Omron Health User Space Driver - libusb version
 *
 * Copyright (c) 2009-2010 Kyle Machulis/Nonpolynomial Labs <kyle@nonpolynomial.com>
 *
 * More info on Nonpolynomial Labs @ http://www.nonpolynomial.com
 *
 * Source code available at http://www.github.com/qdot/libomron/
 *
 * This library is covered by the BSD License
 * Read LICENSE_BSD.txt for details.
 */


#include "libomron/omron.h"
#include <stdlib.h>

#define OMRON_USB_INTERFACE	0

omron_device* omron_create()
{
	omron_device* s = (omron_device*)malloc(sizeof(omron_device));
	s->device._is_open = 0;
	s->device._is_inited = 0;
	if(libusb_init(&s->device._context) < 0)
	{
		return NULL;
	}
	s->device._is_inited = 1;	
	return s;
}

int omron_get_count(omron_device* s, int device_vid, int device_pid)
{
	struct libusb_device **devs;
	struct libusb_device *found = NULL;
	struct libusb_device *dev;
	size_t i = 0;
	int count = 0;

	if (!s->device._is_inited)
	{
		return E_NPUTIL_NOT_INITED;
	}
	
	if (libusb_get_device_list(s->device._context, &devs) < 0)
	{
		return E_NPUTIL_DRIVER_ERROR;
	}

	while ((dev = devs[i++]) != NULL)
	{
		struct libusb_device_descriptor desc;
		int dev_error_code;
		dev_error_code = libusb_get_device_descriptor(dev, &desc);
		if (dev_error_code < 0)
		{
			break;
		}
		if (desc.idVendor == device_vid && desc.idProduct == device_pid)
		{
			++count;
		}
	}

	libusb_free_device_list(devs, 1);
	return count;
}

int omron_open(omron_device* s, int device_vid, int device_pid, unsigned int device_index)
{
	int ret;
	struct libusb_device **devs;
	struct libusb_device *found = NULL;
	struct libusb_device *dev;
	size_t i = 0;
	int count = 0;
	int device_error_code = 0;

	if (!s->device._is_inited)
	{
		return E_NPUTIL_NOT_INITED;
	}

	if ((device_error_code = libusb_get_device_list(s->device._context, &devs)) < 0)
	{
		return E_NPUTIL_DRIVER_ERROR;
	}

	while ((dev = devs[i++]) != NULL)
	{
		struct libusb_device_descriptor desc;
		device_error_code = libusb_get_device_descriptor(dev, &desc);
		if (device_error_code < 0)
		{
			libusb_free_device_list(devs, 1);
			return E_NPUTIL_NOT_INITED;
		}
		if (desc.idVendor == device_vid && desc.idProduct == device_pid)
		{
			if(count == device_index)
			{
				found = dev;
				break;
			}
			++count;
		}
	}

	if (found)
	{
		device_error_code = libusb_open(found, &s->device._device);
		if (device_error_code < 0)
		{
			libusb_free_device_list(devs, 1);
			return E_NPUTIL_NOT_INITED;
		}
	}
	else
	{
		return E_NPUTIL_NOT_INITED;		
	}
	s->device._is_open = 1;

	if(libusb_kernel_driver_active(s->device._device, 0))
	{
		libusb_detach_kernel_driver(s->device._device, 0);
	}
	ret = libusb_claim_interface(s->device._device, 0);

	return ret;
}

int omron_close(omron_device* s)
{
	if(!s->device._is_open)
	{
		return E_NPUTIL_NOT_OPENED;
	}
	if (libusb_release_interface(s->device._device, 0) < 0)
	{
		return E_NPUTIL_NOT_INITED;				
	}
	libusb_close(s->device._device);
	s->device._is_open = 0;
	return 0;
}

void omron_delete(omron_device* dev)
{
	free(dev);
}

int omron_set_mode(omron_device* dev, omron_mode mode)
{

	uint8_t feature_report[2] = {(mode & 0xff00) >> 8, (mode & 0x00ff)};
	const uint feature_report_id = 0;
	const uint feature_interface_num = 0;
	const int REQ_HID_SET_REPORT = 0x09;
	const int HID_REPORT_TYPE_FEATURE = 3;
	return libusb_control_transfer(dev->device._device, LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, REQ_HID_SET_REPORT, (HID_REPORT_TYPE_FEATURE << 8) | feature_report_id, feature_interface_num, feature_report, sizeof(feature_report), 1000);
}

int omron_read_data(omron_device* dev, uint8_t* input_report)
{
	int trans;
	int ret = libusb_bulk_transfer(dev->device._device, OMRON_IN_ENDPT, input_report, 8, &trans, 1000);
	printf("%d\n", ret);
	return ret;
}

int omron_write_data(omron_device* dev, uint8_t* output_report)
{
	int trans;
	return libusb_bulk_transfer(dev->device._device, OMRON_OUT_ENDPT, output_report, 8, &trans, 1000);
}

