#include "libomron/omron.h"
#include <stdio.h>
#include <stdlib.h>		/* atoi */

int main(int argc, char** argv)
{
	omron_device* test;
	int ret;
	int i;
	int data_count;
	unsigned char str[30];
	int bank = 0;

	if (argc > 1)
		bank = atoi(argv[1]);

    //Uncomment for libhid debug messages
	//#ifdef USE_LIBHID
	//hid_set_debug(HID_DEBUG_ALL);
	//hid_set_debug_stream(stderr);
	//hid_set_usb_debug(0);
	//#endif USE_LIBHID

	test = omron_create();
	
	ret = omron_get_count(test, OMRON_790IT_VID, OMRON_790IT_PID);

	if(!ret)
	{
		printf("No omron 790ITs connected!\n");
		return 1;
	}
	printf("Found %d omron 790ITs\n", ret);

	ret = omron_open(test, OMRON_790IT_VID, OMRON_790IT_PID, 0);
	if(ret < 0)
	{
		printf("Cannot open omron 790IT!\n");
		return 1;
	}
	printf("Opened omron 720IT\n", ret);

	ret = omron_get_device_version(test, str);
	if(ret < 0)
	{
		printf("Cannot get device version!\n");
	}
	else
	{
		printf("Device serial: %s\n", str);
	}

	omron_pd_profile_info p = omron_get_pd_profile(test);
	printf("Weight: %d pounds\n", p.weight);
	printf("Stride: %d inches\n", p.stride); 
	omron_pd_count_info c = omron_get_pd_data_count(test);
	printf("Daily Reading Blocks: %d\n", c.daily_count);
	printf("Hourly Reading Blocks: %d\n", c.hourly_count);

	for(i = 0; i < c.daily_count - 1; ++i)
	{
		omron_pd_daily_data d = omron_get_pd_daily_data(test, i + 1);
		printf("Daily Steps: %d\n", d.total_steps);
		omron_pd_hourly_data* h = omron_get_pd_hourly_data(test, i + 1);
		int j;
		for(j = 0; j < 24; ++j)
		{
			printf("Hour: %d - On: %d - Steps:  %d\n", j, h[j].is_attached > 0, h[j].regular_steps);
		}
	}

	ret = omron_close(test);
	if(ret < 0)
	{
		printf("Cannot close omron 790IT!\n");
		return 1;
	}
	return 0;
}
