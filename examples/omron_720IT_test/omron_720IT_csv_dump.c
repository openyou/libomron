#include "libomron/omron.h"
#include <stdio.h>
#include <stdlib.h>		/* atoi */
#include <time.h>
#include <unistd.h> /* getopt */

#define SECONDS_PER_DAY 86400

int main(int argc, char** argv)
{
	omron_device* test = omron_create();
	int ret;
	int i;
	int day;
	int data_count;
	unsigned char str[30];
	unsigned char data[37];
	int bank = 0;
	omron_pd_profile_info p;
	omron_pd_count_info c;
	char time_str[20];
	struct tm *timeptr;
	time_t today_secs, other_secs;
	int ch, clear_flag = 0;
	
	while((ch = getopt(argc, argv, "d")) != -1) {
		switch (ch) {
		case 'd':	
			clear_flag = 1;
			break;
		}
	}

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
	//printf("Found %d omron 720ITs\n", ret);

	ret = omron_open(test, OMRON_VID, OMRON_PID, 0);
	if(ret < 0)
	{
		printf("Cannot open omron 720IT!\n");
		return 1;
	}
	//printf("Opened omron 720IT\n");

	
	ret = omron_get_device_version(test, str);
	if(ret < 0)
	{
		printf("Cannot get device version!\n");
	}
	else
	{
		//printf("Device serial: %s\n", str);
	}
	
		
	c = omron_get_pd_data_count(test);
	
	printf("Daily Reading Blocks: %d\n", c.daily_count);
	printf("Hourly Reading Blocks: %d\n", c.hourly_count);
	
	data_count = c.daily_count < c.hourly_count ? c.daily_count : c.hourly_count;
	
	if(data_count > 0) {
		printf("Date,Total Steps,Aerobic Steps,Aerobic Walking Time,Calories,Distance,Fat Burned,Steps 12AM,Steps 1AM,Steps 2AM,Steps 3AM,Steps 4AM,Steps 5AM,Steps 6AM,Steps 7AM,Steps 8AM,Steps 9AM,Steps 10AM,Steps 11AM,Steps 12PM,Steps 1PM,Steps 2PM,Steps 3PM,Steps 4PM,Steps 5PM,Steps 6PM,Steps 7PM,Steps 8PM,Steps 9PM,Steps 10PM,Steps 11PM,Aerobic Steps 12AM,Aerobic Steps 1AM,Aerobic Steps 2AM,Aerobic Steps 3AM,Aerobic Steps 4AM,Aerobic Steps 5AM,Aerobic Steps 6AM,Aerobic Steps 7AM,Aerobic Steps 8AM,Aerobic Steps 9AM,Aerobic Steps 10AM,Aerobic Steps 11AM,Aerobic Steps 12PM,Aerobic Steps 1PM,Aerobic Steps 2PM,Aerobic Steps 3PM,Aerobic Steps 4PM,Aerobic Steps 5PM,Aerobic Steps 6PM,Aerobic Steps 7PM,Aerobic Steps 8PM,Aerobic Steps 9PM,Aerobic Steps 10PM,Aerobic Steps 11PM,Used 12AM,Used 1AM,Used 2AM,Used 3AM,Used 4AM,Used 5AM,Used 6AM,Used 7AM,Used 8AM,Used 9AM,Used 10AM,Used 11AM,Used 12PM,Used 1PM,Used 2PM,Used 3PM,Used 4PM,Used 5PM,Used 6PM,Used 7PM,Used 8PM,Used 9PM,Used 10PM,Used 11PM,Event 12AM,Event 1AM,Event 2AM,Event 3AM,Event 4AM,Event 5AM,Event 6AM,Event 7AM,Event 8AM,Event 9AM,Event 10AM,Event 11AM,Event 12PM,Event 1PM,Event 2PM,Event 3PM,Event 4PM,Event 5PM,Event 6PM,Event 7PM,Event 8PM,Event 9PM,Event 10PM,Event 11PM\n");
		today_secs = time(NULL);
		for(i = 0; i < data_count; ++i) {
			other_secs = (time_t)(today_secs - i * SECONDS_PER_DAY);
			timeptr = localtime(&other_secs);
			strftime(time_str, 20, "%m/%d/%Y", timeptr);
		
			omron_pd_daily_data d = omron_get_pd_daily_data(test, i);
			printf("%s,%d,%d,%d,%d,%0.2f,%0.1f", time_str, d.total_steps, d.total_aerobic_steps, d.total_aerobic_walking_time, d.total_calories, d.total_distance, d.total_fat_burn);

			omron_pd_hourly_data* h = omron_get_pd_hourly_data(test, i);
			// hour loops
			int j;
			for(j = 0; j < 24; ++j)
			{
				printf(",%d", h[j].regular_steps);
				//printf("d: %d Hour: %d - On: %d - Steps:  %d roby: %d Event: %d \n", h[j].day_serial, h[j].hour_serial, h[j].is_attached > 0, h[j].regular_steps, h[j].aerobic_steps, h[j].event);
			}
			for(j = 0; j < 24; ++j)
			{
				printf(",%d", h[j].aerobic_steps);
			}
			for(j = 0; j < 24; ++j)
			{
				printf(",%d", h[j].is_attached);
			}
			for(j = 0; j < 24; ++j)
			{
				printf(",%d", h[j].event);
			}
			printf("\n");
			
			free(h);
		}
	}
	if(clear_flag) {
		ret = omron_clear_pd_memory(test);
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
