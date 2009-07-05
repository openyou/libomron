/*
 * Generic function file for Omron Health User Space Driver
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

#include "omron.h"
#include <string.h>
#include <stdio.h>

int omron_send_command(omron_device* dev, int size, const unsigned char* buf)
{
	int total_write_size = 0;
	int current_write_size;

	while(total_write_size < size)
	{
		current_write_size = size - total_write_size;
		if(current_write_size > 7) current_write_size = 7;
		dev->output_report[0] = current_write_size;
		memcpy((dev->output_report + 1), buf+total_write_size, current_write_size);
		omron_write_data(dev);
		total_write_size += current_write_size;
	}
	return 0;
}

int omron_check_success(omron_device* dev)
{
	return (dev->input_report[1] == 'O' && dev->input_report[2] == 'K')
		? 0 : -1;
}

int omron_send_clear(omron_device* dev)
{
	static const unsigned char zero[23]; /* = all zeroes */

	do {
		omron_send_command(dev, 23, zero);
		omron_read_data(dev);

	} while (omron_check_success(dev) != 0);

	return 0;
}

int omron_get_command_return(omron_device* dev, int size, unsigned char* data)
{
	int total_read_size = 0;
	int current_read_size = 0;
	int has_checked = 0;
	while(total_read_size < size)
	{
		omron_read_data(dev);
		current_read_size = dev->input_report[0];
		if(!has_checked)
		{
			if(omron_check_success(dev) != 0)
			{
				return -1;
			}
			memcpy(data, dev->input_report + 4, current_read_size - 3);
			total_read_size += current_read_size - 3;
			has_checked = 1;
		}
		else
		{
			memcpy(data + total_read_size, dev->input_report + 1, current_read_size);
			total_read_size += current_read_size;
		}
	}
	return 0;
}

int omron_check_mode(omron_device* dev, omron_mode mode)
{
	int ret;
	if(dev->device_mode == mode) return 0;
	ret = omron_set_mode(dev, mode);
	if(ret == 0)
	{
		dev->device_mode = mode;
		omron_send_clear(dev);
	}
	return ret;
}

static void
omron_dev_info_command(omron_device* dev,
		       const char *cmd,
		       unsigned char *result,
		       int result_max_len)
{
	omron_check_mode(dev, DEVICE_INFO_MODE);
	omron_send_command(dev, strlen(cmd),
			   (const unsigned char*) cmd);
	omron_get_command_return(dev, result_max_len, result);
}


//platform independant functions
int omron_get_device_version(omron_device* dev, unsigned char* data)
{
	omron_dev_info_command(dev, "VER00", data, 12);
	data[12] = 0;
	return 0;
}

int omron_get_device_prf(omron_device* dev, unsigned char* data)
{
	omron_dev_info_command(dev, "PRF00", data, 10);
	return 0;
}

int omron_get_device_serial(omron_device* dev, unsigned char* data)
{
	omron_dev_info_command(dev, "SRL00", data, 8);
	return 0;
}

//daily data information
int omron_get_daily_data_count(omron_device* dev, unsigned char bank)
{
	unsigned char data[5];
	unsigned char command[8] =
		{ 'G', 'D', 'C', 0x00, 0x00, 0x00, 0x00, bank };

	omron_check_mode(dev, DAILY_INFO_MODE);
	omron_send_command(dev, 8, command);
	omron_get_command_return(dev, 5, data);
	printf("Data units found: %d\n", (int)data[3]);
	return (int)data[3];
}

omron_bp_day_info* omron_get_all_daily_bp_data(omron_device* dev, int* count)
{
	omron_check_mode(dev, DAILY_INFO_MODE);
	return 0;
}

static void hexdump(void *data, int n_bytes)
{
	while (n_bytes--) {
		printf(" %02x", *(unsigned char*) data);
		data++;
	}
}

static void omron_exchange_cmd(omron_device *dev,
			       int cmd_len,
			       const unsigned char *cmd,
			       int response_len,
			       unsigned char *response)
{
	//Commands fail sometimes. Loop until it works.
	do {
		omron_send_command(dev, cmd_len, cmd);
	} while (omron_get_command_return(dev, response_len, response) < 0);
}

omron_bp_day_info omron_get_daily_bp_data(omron_device* dev, int index)
{
	omron_bp_day_info r;
	unsigned char data[17];
	unsigned char command[8] = {'G', 'M', 'E', 0x00, 0x00, 0x00, index, index};
	omron_check_mode(dev, DAILY_INFO_MODE);
	omron_exchange_cmd(dev, 8, command, 14, data);

	r.year = data[0];
	r.month = data[1];
	r.day = data[2];
	r.hour = data[3];
	r.minute = data[4];
	r.second = data[5];
	r.sys = data[8];
	r.dia = data[9];
	r.pulse = data[10];
	return r;
}

//weekly data information
omron_bp_week_info* omron_get_all_weekly_bp_data(omron_device* dev, int* count)
{
	omron_bp_week_info* r;
	omron_check_mode(dev, DEVICE_INFO_MODE);
	return 0;
}

omron_bp_week_info omron_get_weekly_bp_data(omron_device* dev, int index, int evening)
{
	omron_bp_week_info r;
	unsigned char data[12];
	unsigned char command[9] = { 'G',
				     (evening ? 'E' : 'M'),
				     'A',
				     0,
				     1,
				     index,
				     0,
				     0,
				     index^1 };
				     
	omron_check_mode(dev, DEVICE_INFO_MODE);

	omron_exchange_cmd(dev, 9, command, 9, data);

	printf("Weekly data:");
	hexdump(data, sizeof(data));
	printf("\n");

	r.sys = data[6] + 25;
	r.dia = data[7];
	r.pulse = data[8];
	return r;
}
