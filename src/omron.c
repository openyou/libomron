/*
 * Generic function file for Omron Health User Space Driver
 *
 * Copyright (c) 2009-2010 Kyle Machulis <kyle@nonpolynomial.com>
 *
 * More info on Nonpolynomial Labs @ http://www.nonpolynomial.com
 *
 * Sourceforge project @ http://www.github.com/qdot/libomron/
 *
 * This library is covered by the BSD License
 * Read LICENSE_BSD.txt for details.
 */

#include "libomron/omron.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#define DEBUG

#ifdef DEBUG
#define IF_DEBUG(x)	do { x; } while (0)
#else
#define IF_DEBUG(x)	do { } while (0)
#endif

#define DPRINTF(args, ...)	IF_DEBUG(printf(args))

static void hexdump(uint8_t *data, int n_bytes)
{
	while (n_bytes--) {
		printf(" %02x", *(unsigned char*) data);
		data++;
	}
	printf("\n");
}

int bcd_to_int(unsigned char *data, int start, int length)
{
	int ret = 0;
	int pos = start;
	int i;
	for(i = 0; i < length; ++i)
	{
		if(!(i % 2))
		{
			if(i >= 2)
				pos++;
			ret += (data[pos] >> 4) * (pow(10, length - i - 1));
		}
		else
		{
			ret += (data[pos] & 0x0f) * (pow(10, length - i - 1));
		}
	}
	return ret;
}

/*
* For data not starting on byte boundaries
*/
int bcd_to_int2(unsigned char *data, int start_nibble, int len_nibbles)
{
	int ret = 0;
	int nib, abs_nib, i;
	for(i=0; i < len_nibbles; i++) {
		abs_nib = start_nibble + i;
		nib = (data[abs_nib / 2] >> (4 * (1 - abs_nib % 2))) & 0x0F;
		ret += nib * pow(10, len_nibbles - i - 1);
	}
	return ret;
}

short short_to_bcd(int number)
{
	return ((number/10) << 4) | (number % 10);
}


int omron_send_command(omron_device* dev, int size, const unsigned char* buf)
{
	int total_write_size = 0;
	int current_write_size;
	unsigned char output_report[8];

	DPRINTF("omron_send:");
	IF_DEBUG(hexdump(buf, size));
	DPRINTF("\n");

	while(total_write_size < size)
	{
		current_write_size = size - total_write_size;
		if(current_write_size > 7)
			current_write_size = 7;

		output_report[0] = current_write_size;
		memcpy(output_report + 1, buf+total_write_size,
		       current_write_size);

		omron_write_data(dev, output_report);
		total_write_size += current_write_size;
	}

	return 0;
}

int omron_check_success(unsigned char *input_report, int start_index)
{
	return (input_report[start_index] == 'O' && input_report[start_index + 1] == 'K') ? 0 : -1;
}

int omron_send_clear(omron_device* dev)
{
	//static const unsigned char zero[23]; /* = all zeroes */
	static const unsigned char zero[12]; /* = all zeroes */
	//unsigned char input_report[9];
	unsigned char input_report[8];
	int read_result;
	read_result = omron_read_data(dev, input_report);
	do {
		omron_send_command(dev, sizeof(zero), zero);
		read_result = omron_read_data(dev, input_report);
	} while (omron_check_success(input_report, 1) != 0);

	return 0;
}

static int
xor_checksum(unsigned char *data, int len)
{
	unsigned char checksum = 0;

	unsigned int begin_len = len;
	
	while (len--)
		checksum ^= *(data++);

	if (checksum)
		DPRINTF("bad checksum 0x%x\n", checksum);

	return checksum;
}

/*
  omron_get_command_return returns:
	#of bytes
  0 : Valid response ("OK..." or "NO").
  1 : Garbled response
  <0 : IO error
*/

int omron_get_command_return(omron_device* dev, int size, unsigned char* data)
{
	int total_read_size = 0;
	int current_read_size = 0;
	int has_checked = 0;
	unsigned char input_report[9];
	int read_result;


	DPRINTF("\n");		/* AJR */
	while(total_read_size < size)
	{
		read_result = omron_read_data(dev, input_report);
		DPRINTF("AJR read result=%d.\n", read_result);
		if (read_result < 0) {
			fprintf(stderr, "omron_get_command_return: read_result_result %d < zero\n", read_result);
			return read_result;
		}

		//assert(read_result == 8);
		current_read_size = input_report[0];
		IF_DEBUG(hexdump(input_report, current_read_size+1));
		/* printf(" current_read=%d size=%d total_read_size=%d.\n", */
		/* 		current_read_size, size, total_read_size); */
	   
		assert(current_read_size <= 8);

		if (current_read_size == 8)
			current_read_size = 7; /* FIXME? Bug? */

		assert(current_read_size < 8);
		assert(current_read_size >= 0);

		assert(total_read_size >= 0);


		assert(current_read_size <= size - total_read_size);

		memcpy(data + total_read_size, input_report + 1,
		       current_read_size);
		total_read_size += current_read_size;

		if(!has_checked && total_read_size >= 2)
		{
			if (total_read_size == 2 &&
			    data[0] == 'N' && data[1] == 'O')
				return 0; /* "NO" is valid response */

			if (strncmp((const char*) data, "END\r\n",
				    total_read_size) == 0)
				has_checked = (total_read_size >= 5);
			else {
				if (data[0] != 'O' || data[1] != 'K')
					return 1; /* garbled response */
				
				has_checked = 1;
			}
		}
	}

	if (total_read_size >= 3 && data[0] == 'O' && data[1] == 'K')
		return xor_checksum(data+2, total_read_size-2); /* 0 = OK */
	else
		return 0;
}

int omron_check_mode(omron_device* dev, omron_mode mode)
{
	int ret;

	if(dev->device_mode == mode)
		return 0;

	ret = omron_set_mode(dev, mode);
	if(ret == 0)
	{
		dev->device_mode = mode;
		omron_send_clear(dev);
		return 0;
	}
	else
	{
		DPRINTF("omron_exchange_cmd: I/O error, status=%d\n",
			ret);		
	}
	return ret;
}

static void omron_exchange_cmd(omron_device *dev,
			       omron_mode mode,
			       int cmd_len,
			       const unsigned char *cmd,
			       int response_len,
			       unsigned char *response)
{
	int status;
	struct timeval timeout;
	
	// Retry command if the response is garbled, but accept "NO" as
	// a valid response.
	do {
		omron_check_mode(dev, mode);
		omron_send_command(dev, cmd_len, cmd);
		status = omron_get_command_return(dev, response_len, response);
		if (status > 0)
			DPRINTF("garbled (resp_len=%d)\n", response_len);
		if(mode == PEDOMETER_MODE) {
			DPRINTF("Sleeping\n");
			// Adding a short wait to see if it helps the bad data
			// give it 0.15 seconds to recover
			timeout.tv_sec = 0;
			timeout.tv_usec = 200000;
			select(0, NULL, NULL, NULL, &timeout);
		}
	} while (status > 0);

	if (status < 0)
		DPRINTF("omron_exchange_cmd: I/O error, status=%d\n",
			status);
}

static void
omron_dev_info_command(omron_device* dev,
		       const char *cmd,
		       unsigned char *result,
		       int result_max_len)
{
	unsigned char* tmp = (unsigned char*)malloc(result_max_len+3);

	omron_exchange_cmd(dev, PEDOMETER_MODE, strlen(cmd),
			   (const unsigned char*) cmd,
			   result_max_len+3, tmp);

	memcpy(result, tmp + 3, result_max_len);
	free(tmp);
}

//platform independant functions
OMRON_DECLSPEC int omron_get_device_version(omron_device* dev, unsigned char* data)
{
	omron_dev_info_command(dev, "VER00", data, 12);
	data[12] = 0;
	return 0;
}

OMRON_DECLSPEC int omron_get_bp_profile(omron_device* dev, unsigned char* data)
{
	omron_dev_info_command(dev, "PRF00", data, 11);
	return 0;
}

OMRON_DECLSPEC int omron_get_device_serial(omron_device* dev, unsigned char* data)
{
	omron_dev_info_command(dev, "SRL00", data, 8);
	return 0;
}

//daily data information
OMRON_DECLSPEC int omron_get_daily_data_count(omron_device* dev, unsigned char bank)
{
	unsigned char data[8];
	unsigned char command[8] =
		{ 'G', 'D', 'C', 0x00, bank, 0x00, 0x00, bank };

	// assert(bank < 2);
	omron_exchange_cmd(dev, DAILY_INFO_MODE, 8, command,
			   sizeof(data), data);
	DPRINTF("Data units found: %d\n", (int)data[6]);
	return (int)data[6];
}

OMRON_DECLSPEC omron_bp_day_info omron_get_daily_bp_data(omron_device* dev, int bank, int index)
{
	omron_bp_day_info r;
	unsigned char data[17];
	unsigned char command[8] = {'G', 'M', 'E', 0x00, bank, 0x00,
				    index, index ^ bank};

	memset(&r, 0 , sizeof(r));
	memset(data, 0, sizeof(data));

	omron_exchange_cmd(dev, DAILY_INFO_MODE, 8, command,
			   sizeof(data), data);

	//printf("Daily data:");
	//hexdump(data, sizeof(data));
	//printf("\n");

        // added by bv
	// if (data[0] == 'O' && data[1] == 'K') {
	// 	r.year = data[3];
	// 	r.month = data[4];
	// 	r.day = data[5];
	// 	r.hour = data[6];
	// 	r.minute = data[7];
	// 	r.second = data[8];
	// 	r.unknown_1[0] = data[9];
	// 	r.unknown_1[1] = data[10];
	// 	r.sys = data[11];
	// 	r.dia = data[12];
	// 	r.pulse = data[13];
	// 	r.unknown_2[0] = data[14];
	// 	r.unknown_2[1] = data[15];
	// 	r.unknown_2[2] = data[16];
	// }
	// return r;

	if (omron_check_success(data, 0) < 0)
	{
		r.present = 0;
	}
	else
	{
		r.present = 1;
		r.year = data[3];
		r.month = data[4];
		r.day = data[5];
		r.hour = data[6];
		r.minute = data[7];
		r.second = data[8];
		r.unknown_1[0] = data[9];
		r.unknown_1[1] = data[10];
		r.sys = data[11];
		r.dia = data[12];
		r.pulse = data[13];
		r.unknown_2[0] = data[14];
		r.unknown_2[1] = data[15];
		r.unknown_2[2] = data[16];
	}
	return r;
}

OMRON_DECLSPEC omron_bp_week_info omron_get_weekly_bp_data(omron_device* dev, int bank, int index, int evening)
{
	omron_bp_week_info r;
	unsigned char data[12];	/* 12? */
	unsigned char command[9] = { 'G',
				     (evening ? 'E' : 'M'),
				     'A',
				     0,
				     bank,
				     index,
				     0,
				     0,
				     index^bank };
				     
	memset(&r, 0 , sizeof(r));
	memset(data, 0, sizeof(data));

	omron_exchange_cmd(dev, WEEKLY_INFO_MODE, 9, command,
			   sizeof(data), data);

	if (omron_check_success(data, 0) < 0)
	{
		r.present = 0;
	}
	else {
		r.present = 1;
		/* printf("Weekly data:"); */
		/* hexdump(data, sizeof(data)); */
		/* printf("\n"); */

		r.year = data[5];
		r.month = data[6];
		r.day = data[7];
		r.sys = data[8] + 25;
		r.dia = data[9];
		r.pulse = data[10];
	}
	return r;
}

OMRON_DECLSPEC omron_pd_profile_info omron_get_pd_profile(omron_device* dev)
{
	unsigned char data[11];
	omron_pd_profile_info profile_info;
	omron_dev_info_command(dev, "PRF00", data, 11);
	profile_info.weight = bcd_to_int(data, 2, 4) / 10;
	profile_info.stride = bcd_to_int(data, 6, 4) / 10;
	return profile_info;
}

OMRON_DECLSPEC int omron_clear_pd_memory(omron_device* dev)
{
	unsigned char data[2];
	omron_exchange_cmd(dev, PEDOMETER_MODE, strlen("CTD00"),"CTD00", 2, data);
	if((data[0]=='O') && (data[1]=='K')) {
		return 0;
	} else {
		return 1;
	}
}

OMRON_DECLSPEC omron_pd_count_info omron_get_pd_data_count(omron_device* dev)
{
	omron_pd_count_info count_info;
	unsigned char data[5];
	omron_dev_info_command(dev, "CNT00", data, 5);
	hexdump(data, 5);
	//count_info.daily_count = bcd_to_int(data, 0, 4);
	//count_info.hourly_count = bcd_to_int(data, 2, 4);
	count_info.daily_count = data[1];
	count_info.hourly_count = data[3];
	return count_info;
}

OMRON_DECLSPEC omron_pd_daily_data omron_get_pd_daily_data(omron_device* dev, int day)
{
	omron_pd_daily_data daily_data;
	unsigned char data[20];
	unsigned char command[7] =
		{ 'M', 'E', 'S', 0x00, 0x00, day, 0x00 ^ day};

	// assert(bank < 2);
	omron_exchange_cmd(dev, PEDOMETER_MODE, sizeof(command), command,
			   sizeof(data), data);
	daily_data.total_steps = bcd_to_int2(data, 6, 5);
	daily_data.total_aerobic_steps = bcd_to_int2(data, 11, 5);
	daily_data.total_aerobic_walking_time = bcd_to_int2(data, 16, 4);
	daily_data.total_calories = bcd_to_int2(data, 20, 5);
	daily_data.total_distance = bcd_to_int2(data, 25, 5) / 100.0;
	daily_data.total_fat_burn = bcd_to_int2(data, 30, 4) / 10.0;
	daily_data.day_serial = day;
	return daily_data;
}

OMRON_DECLSPEC omron_pd_hourly_data* omron_get_pd_hourly_data(omron_device* dev, int day)
{
	omron_pd_hourly_data* hourly_data = malloc(sizeof(omron_pd_hourly_data) * 24);
	unsigned char data[37];

	// assert(bank < 2);
	int i, j;
	for(i = 0; i < 3; ++i)
	{
		unsigned char command[8] =
			{ 'G', 'T', 'D', 0x00, 0, day, i + 1, day ^ (i + 1)};
		omron_exchange_cmd(dev, PEDOMETER_MODE, sizeof(command), command,
						   sizeof(data), data);
		for(j = 0; j <= 7; ++j)
		{
			int offset = j * 4 + 4;
			int hour = (i * 8) + j;
			hourly_data[hour].is_attached = (data[(offset)] & (1 << 6)) > 0;
			hourly_data[hour].regular_steps = ((data[(offset)] & (~0xc0)) << 8) | (data[(offset) + 1]);
			hourly_data[hour].event = (data[(offset) + 2] & (1 << 6)) > 0;
			hourly_data[hour].aerobic_steps = ((data[(offset) + 2] & (~0xc0)) << 8) | (data[(offset) + 3]);
			hourly_data[hour].hour_serial = hour;
			hourly_data[hour].day_serial = day;
		}
	}
	return hourly_data;
}
