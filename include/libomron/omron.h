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
#define OMRON_OUT_ENDPT	0x02
#define OMRON_IN_ENDPT  0x81


// #ifdef USE_LIBHID
// #include <hid.h>
// typedef HIDInterface* omron_device_impl;
// #elif WIN32
// #include <windows.h>
// typedef HANDLE omron_device_impl;
// #else
#include <stdint.h>
#include "nputil/nputil_libusb1.h"
typedef nputil_libusb1_struct* omron_device_impl;
// #endif

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
	uint8_t version[13];
	uint8_t prf[11];
	uint8_t srl[8];
} omron_device_info;

typedef struct
{
	uint32_t day;
	uint32_t month;
	uint32_t year;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
	uint8_t unknown_1[2];
	uint32_t sys;
	uint32_t dia;
	uint32_t pulse;
	uint8_t unknown_2[3];
} omron_bp_day_info;

typedef struct
{
	int32_t present;
	uint8_t unknown_1; // always 0x00
	uint8_t unknown_2; // always 0x80
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint8_t unknown_3; // always 0 
	int32_t sys;		 // always 0?
	int32_t dia;		 // always 0?
	int32_t pulse;
} omron_bp_week_info;

typedef struct
{
	uint8_t unknown_1[2];
	uint32_t weight; // lbs times 10? i.e. 190 = {0x01, 0x90} off the device
	uint32_t stride; // kg times 10? same as last
	uint8_t unknown_2[2];
} omron_pd_profile_info;

typedef struct
{
	int32_t daily_count;
	int32_t hourly_count;
	uint8_t unknown_1;
} omron_pd_count_info;

typedef struct
{
	int32_t total_steps;
	int32_t total_aerobic_steps;
	int32_t total_walking_time;
	int32_t total_calories;
	float total_distance;
	float total_fat_burn;
	int32_t day_serial;
	uint8_t unknown_1;
} omron_pd_daily_data;

typedef struct
{
	int32_t day_serial;
	int32_t hour_serial;
	uint8_t is_attached;
	int32_t regular_steps;
	int32_t aerobic_steps;
} omron_pd_hourly_data;
   

#ifdef __cplusplus
extern "C" {
#endif

	//platform specific functions
	int omron_get_count(omron_device* dev, int VID, int PID);
	int omron_open(omron_device* dev, int VID, int PID, uint32_t device_index);
	int omron_close(omron_device* dev);
	int omron_set_mode(omron_device* dev, omron_mode mode);
	int omron_read_data(omron_device* dev, uint8_t *input_report);
	int omron_write_data(omron_device* dev, uint8_t *output_report);

	//platform independant functions
	int omron_get_device_serial(omron_device* dev, uint8_t* data);
	int omron_get_device_version(omron_device* dev, uint8_t* data);

	////////////////////////////////////////////////////////////////////////////////////
	//
	// Blood Pressure Functions
	//
	////////////////////////////////////////////////////////////////////////////////////
	
	int omron_get_bp_profile(omron_device* dev, uint8_t* data);

	//daily data information
	int omron_get_daily_bp_data_count(omron_device* dev, uint8_t bank);
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
