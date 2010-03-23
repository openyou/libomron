/*
 * Declaration file for Omron Health User Space Driver
 *
 * Copyright (c) 2007-2009 Kyle Machulis/Nonpolynomial Labs <kyle@nonpolynomial.com>
 *
 * More info on Nonpolynomial Labs @ http://www.nonpolynomial.com
 *
 * Sourceforge project @ http://www.github.com/qdot/libomron/
 *
 * This library is covered by the BSD License
 * Read LICENSE_BSD.txt for details.
 */

#ifndef LIBOMRON_H
#define LIBOMRON_H

#define OMRON_790IT_VID 0x0590
#define OMRON_790IT_PID 0x0028
#define OMRON_720IT_VID 0x0590
#define OMRON_720IT_PID 0x0028

#ifdef USE_LIBHID
#include <hid.h>
typedef HIDInterface* omron_device_impl;
#elif WIN32
#include <windows.h>
typedef HANDLE omron_device_impl;
#else
typedef int omron_device_impl;
#endif

typedef enum
{
	NULL_MODE	 = 0x0000,
	DEVICE_INFO_MODE = 0x1111,
	DAILY_INFO_MODE	 = 0x74bc,
	WEEKLY_INFO_MODE = 0x1074,
	PEDOMETER_MODE = 0x0102
} omron_mode;

typedef struct
{
	omron_device_impl device;
	omron_mode device_mode;
} omron_device;

typedef struct
{
	unsigned char version[13];
	unsigned char prf[11];
	unsigned char srl[8];
} omron_device_info;

typedef struct
{
	unsigned int day;
	unsigned int month;
	unsigned int year;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned char unknown_1[2];
	unsigned int sys;
	unsigned int dia;
	unsigned int pulse;
	unsigned char unknown_2[3];
} omron_bp_day_info;

typedef struct
{
	int present;
	unsigned char unknown_1; // always 0x00
	unsigned char unknown_2; // always 0x80
	unsigned int year;
	unsigned int month;
	unsigned int day;
	unsigned char unknown_3; // always 0 
	int sys;		 // always 0?
	int dia;		 // always 0?
	int pulse;
} omron_bp_week_info;

typedef struct
{
	unsigned char unknown_1[2];
	unsigned int weight; // lbs times 10? i.e. 190 = {0x01, 0x90} off the device
	unsigned int stride; // kg times 10? same as last
	unsigned char unknown_2[2];
} omron_pd_profile_info;

typedef struct
{
	int daily_count;
	int hourly_count;
	unsigned char unknown_1;
} omron_pd_count_info;

typedef struct
{
	int total_steps;
	int total_aerobic_steps;
	int total_walking_time;
	int total_calories;
	float total_distance;
	float total_fat_burn;
	int day_serial;
	unsigned char unknown_1;
} omron_pd_daily_data;

typedef struct
{
	int day_serial;
	int hour_serial;
	unsigned char is_attached;
	int regular_steps;
	int aerobic_steps;
} omron_pd_hourly_data;
   

#ifdef __cplusplus
extern "C" {
#endif

	//platform specific functions
	int omron_get_count(int VID, int PID);
	int omron_open(omron_device* dev, int VID, int PID, unsigned int device_index);
	int omron_close(omron_device* dev);
	int omron_set_mode(omron_device* dev, omron_mode mode);
	int omron_read_data(omron_device* dev, unsigned char *input_report);
	int omron_write_data(omron_device* dev, unsigned char *output_report);

	//platform independant functions
	int omron_get_device_serial(omron_device* dev, unsigned char* data);
	int omron_get_device_version(omron_device* dev, unsigned char* data);

	////////////////////////////////////////////////////////////////////////////////////
	//
	// Blood Pressure Functions
	//
	////////////////////////////////////////////////////////////////////////////////////
	
	int omron_get_bp_profile(omron_device* dev, unsigned char* data);

	//daily data information
	int omron_get_daily_bp_data_count(omron_device* dev, unsigned char bank);
	omron_bp_day_info* omron_get_all_daily_bp_data(omron_device* dev, int* count);
	omron_bp_day_info omron_get_daily_bp_data(omron_device* dev, int bank, int index);

	//weekly data information
	omron_bp_week_info* omron_get_all_weekly_bp_data(omron_device* dev, int* count);
	omron_bp_week_info omron_get_weekly_bp_data(omron_device* dev, int bank, int index, int evening);

	////////////////////////////////////////////////////////////////////////////////////
	//
	// Pedometer Functions
	//
	////////////////////////////////////////////////////////////////////////////////////
	
	omron_pd_profile_info omron_get_pd_profile(omron_device* dev);
	omron_pd_count_info omron_get_pd_data_count(omron_device* dev);
	omron_pd_daily_data omron_get_pd_daily_data(omron_device* dev, int day);	
	omron_pd_hourly_data* omron_get_pd_hourly_data(omron_device* dev, int day);
#ifdef __cplusplus
}
#endif


#endif //LIBLIGHTSTONE_H
