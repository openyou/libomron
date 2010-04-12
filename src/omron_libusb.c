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
	s->device = nputil_libusb1_create_struct();
	nputil_libusb1_init(s->device);
	return s;
}

int omron_get_count(omron_device* dev, int VID, int PID)
{
	int count = nputil_libusb1_count(dev->device, VID, PID);
	return count;
}

int omron_open(omron_device* dev, int VID, int PID, unsigned int device_index)
{
	int ret;
	if((ret = nputil_libusb1_open(dev->device, VID, PID, device_index)) < 0)
	{
		return ret;
	}
	if(libusb_kernel_driver_active(dev->device->_device, 0))
	{
		libusb_detach_kernel_driver(dev->device->_device, 0);
	}
	ret = libusb_claim_interface(dev->device->_device, 0);
	printf("Claim Interface %d\n", ret); 
	return ret;
}

int omron_close(omron_device* dev)
{
	nputil_libusb1_close(dev->device);	
}

int omron_delete(omron_device* dev)
{
	nputil_libusb1_delete_struct(dev->device);
	free(dev);
}

int omron_set_mode(omron_device* dev, omron_mode mode)
{

	uint8_t feature_report[2] = {(mode & 0xff00) >> 8, (mode & 0x00ff)};
	const uint feature_report_id = 0;
	const uint feature_interface_num = 0;
	/*
	int err;

	const struct usbdevfs_ctrltransfer set_mode_cmd = {
		.bRequestType	= USB_TYPE_CLASS | USB_RECIP_INTERFACE,
		.bRequest	= USB_REQ_HID_SET_REPORT,
		.wValue		= (HID_REPORT_TYPE_FEATURE << 8) | feature_report_id,
		.wIndex		= feature_interface_num,
		.wLength	= sizeof(feature_report),
		.timeout	= TIMEOUT_MILLISECS,
		.data		= feature_report,
		
	};

	err = ioctl(omron_fd, USBDEVFS_CONTROL, &set_mode_cmd);
	if (err < 0) {
		perror("omron_set_mode USBDEVFS_CONTROL");
		exit(1);
	}
	*/

	const int REQ_HID_SET_REPORT = 0x09;
	const int HID_REPORT_TYPE_FEATURE = 3;
	return libusb_control_transfer(dev->device->_device, LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, REQ_HID_SET_REPORT, (HID_REPORT_TYPE_FEATURE << 8) | feature_report_id, feature_interface_num, feature_report, sizeof(feature_report), 1000);
}

int omron_read_data(omron_device* dev, uint8_t* input_report)
{
	int trans;
	return libusb_bulk_transfer(dev->device->_device, OMRON_IN_ENDPT, input_report, 8, &trans, 1000);
}

int omron_write_data(omron_device* dev, uint8_t* output_report)
{
	int trans;
	return libusb_bulk_transfer(dev->device->_device, OMRON_OUT_ENDPT, output_report, 8, &trans, 1000);
}

/*
lightstone_info lightstone_get_info(lightstone* dev)
{
	lightstone_info ret;
	//hid_return t;
	ret.hrv = -1;
	ret.scl = -1;
	//	if (DeviceHandle != INVALID_HANDLE_VALUE)
	{
		int NumberOfBytesRead;
		char rawAscii[300];
		unsigned char InputReport[256];
		char message_started = 0;
		int transferred = 0;
		int char_count = 0;
		int ii;
		int t;
		while(1)
		{
			t = libusb_interrupt_transfer(dev->_device, 0x81, InputReport, 8, &transferred, 0x10);
			if(transferred == 0x8)
			{
				for(ii = 1; ii < InputReport[0]+1; ++ii)
				{
					if(!message_started && InputReport[ii] != '<') continue;
					message_started = 1;
					if (InputReport[ii] != 10 && InputReport[ii] != 13)
					{
						rawAscii[char_count] = InputReport[ii];
						++char_count;
					}
					else if ( InputReport[ii] == 13 ) 
					{
						rawAscii[char_count] = 0;
						if ( strlen(rawAscii) > 18) 
						{
							ret.scl = ((float)(((hex2dec(rawAscii+5,2)) << 8) | (hex2dec(rawAscii+7,2)))) * .01;
							ret.hrv = ((float)(((hex2dec(rawAscii+10,2)) << 8) | (hex2dec(rawAscii+12,2)))) * .001;
							return ret;
						}
						message_started = 0;
						char_count = 0;
					}
				}
			}
		}
	}
	return ret;
}
*/
