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

#ifdef USE_LIBHID
#include <hid.h>
typedef HIDInterface* omron_device_impl;
#else
#include <windows.h>
typedef HANDLE omron_device_impl;
#endif

typedef enum
{
	NULL_MODE	 = 0x0000,
	DEVICE_INFO_MODE = 0x1111,
	DAILY_INFO_MODE	 = 0x74bc,
	WEEKLY_INFO_MODE = 0x1074
} omron_mode;

typedef struct
{
	omron_device_impl device;
	omron_mode device_mode;
	unsigned char input_report[8];
	unsigned char output_report[8];
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
	unsigned char unknown_1[6];
	int sys;
	int dia;
	int pulse;
	unsigned char unknown_2[1];
} omron_bp_week_info;


#ifdef __cplusplus
extern "C" {
#endif

	//platform specific functions
	int omron_get_count(int VID, int PID);
	int omron_open(omron_device* dev, int VID, int PID, unsigned int device_index);
	int omron_close(omron_device* dev);
	int omron_set_mode(omron_device* dev, omron_mode mode);
	int omron_read_data(omron_device* dev);
	int omron_write_data(omron_device* dev);

	//platform independant functions
	int omron_get_device_serial(omron_device* dev, unsigned char* data);
	int omron_get_device_version(omron_device* dev, unsigned char* data);
	int omron_get_device_prf(omron_device* dev, unsigned char* data);

	//daily data information
	int omron_get_daily_data_count(omron_device* dev, unsigned char bank);
	omron_bp_day_info* omron_get_all_daily_bp_data(omron_device* dev, int* count);
	omron_bp_day_info omron_get_daily_bp_data(omron_device* dev, int index);

	//weekly data information
	omron_bp_week_info* omron_get_all_weekly_bp_data(omron_device* dev, int* count);
	omron_bp_week_info omron_get_weekly_bp_data(omron_device* dev, int index, int evening);

#ifdef __cplusplus
}
#endif


#endif //LIBLIGHTSTONE_H
