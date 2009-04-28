#include "omron.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	omron_device test;
	int ret;
	int i;
	int data_count;
	char str[30];

    //Uncomment for libhid debug messages
	//#ifdef USE_LIBHID
	//hid_set_debug(HID_DEBUG_ALL);
	//hid_set_debug_stream(stderr);
	//hid_set_usb_debug(0);
	//#endif USE_LIBHID
	
	ret = omron_get_count(OMRON_790IT_VID, OMRON_790IT_PID);

	if(!ret)
	{
		printf("No omron 790ITs connected!\n");
		return 1;
	}
	printf("Found %d omron 790ITs\n", ret);

	ret = omron_open(&test, OMRON_790IT_VID, OMRON_790IT_PID, 0);
	if(ret < 0)
	{
		printf("Cannot open omron 790IT!\n");
		return 1;
	}

	ret = omron_get_device_version(&test, str);
	if(ret < 0)
	{
		printf("Cannot get device version!\n");
	}
	else
	{
		printf("Device serial: %s\n", str);
	}

	ret = omron_get_device_prf(&test, str);
	if(ret < 0)
	{
		printf("Cannot get device prf!\n");
	}
	else
	{
		printf("Device version: %s\n", str);
	}

	data_count = omron_get_daily_data_count(&test, 0);
	if(data_count < 0)
	{
		printf("Cannot get device prf!\n");
	}

	for(i = data_count - 1; i >= 0; --i)
	{
		omron_bp_day_info r = omron_get_daily_bp_data(&test, i);
		printf("Reading on %.2d/%.2d/%.2d %.2d:%.2d:%.2d\nSYS: %d DIA: %d PULSE: %d\n", r.day, r.month, r.year, r.hour, r.minute, r.second, r.sys, r.dia, r.pulse);
	}

	ret = omron_close(&test);
	if(ret < 0)
	{
		printf("Cannot close omron 790IT!\n");
		return 1;
	}
	return 0;
}
