#include "libomron/omron.h"
#include <stdio.h>
#include <stdlib.h>		/* atoi */

int main(int argc, char** argv)
{
	omron_device* test = omron_create();
	int ret;
	int i;
	int data_count;
	unsigned char str[30];
	int bank = 0;
	omron_pd_profile_info p;
	omron_pd_count_info c;
	
	if (argc > 1)
		bank = atoi(argv[1]);

	if(test == NULL)
	{
		printf("Cannot initialize USB core!\n");
		return 1;		
	}

	ret = omron_get_count(test, OMRON_VID, OMRON_PID);

	if(!ret)
	{
		printf("No omron 720ITs connected!\n");
		return 1;
	}
	printf("Found %d omron 720ITs\n", ret);

	ret = omron_open(test, OMRON_VID, OMRON_PID, 0);
	if(ret < 0)
	{
		printf("Cannot open omron 720IT!\n");
		return 1;
	}
	printf("Opened omron 720IT\n");

	ret = omron_get_device_version(test, str);
	if(ret < 0)
	{
		printf("Cannot get device version!\n");
	}
	else
	{
		printf("Device serial: %s\n", str);
	}

	p = omron_get_pd_profile(test);
	printf("Weight: %d pounds\n", p.weight);
	printf("Stride: %d inches\n", p.stride); 
	c = omron_get_pd_data_count(test);
	printf("Daily Reading Blocks: %d\n", c.daily_count);
	printf("Hourly Reading Blocks: %d\n", c.hourly_count);

	for(i = 0; i < c.daily_count - 1; ++i)
	{
		omron_pd_daily_data d = omron_get_pd_daily_data(test, i + 1);
		omron_pd_hourly_data* h = omron_get_pd_hourly_data(test, i + 1);
		int j;
		printf("Daily Steps: %d\n", d.total_steps);

		for(j = 0; j < 24; ++j)
		{
			printf("Hour: %d - On: %d - Steps:  %d\n", j, h[j].is_attached > 0, h[j].regular_steps);
		}
	}

	ret = omron_close(test);
	if(ret < 0)
	{
		printf("Cannot close omron 720IT!\n");
		return 1;
	}

	omron_delete(test);
	return 0;
}
