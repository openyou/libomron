/*
 * Declaration file for Omron Health User Space Driver
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

#ifndef LIBOMRON_H
#define LIBOMRON_H

/*******************************************************************************
 *
 * Headers
 *
 ******************************************************************************/

#define E_NPUTIL_DRIVER_ERROR -1
#define E_NPUTIL_NOT_INITED -2
#define E_NPUTIL_NOT_OPENED -3

#include <stdint.h>

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if defined(OMRON_DYNAMIC)
#define OMRON_DECLSPEC __declspec(dllexport)
#else
#define OMRON_DECLSPEC
#endif

/**
 * Structure to hold information about Windows HID devices.
 *
 * @ingroup CoreFunctions
 */
typedef struct {
	/// Windows device handle
	HANDLE _dev;
	/// 0 if device is closed, > 0 otherwise
	int _is_open;
	/// 0 if device is initialized, > 0 otherwise
	int _is_inited;
} omron_device_impl;
#else
#define OMRON_DECLSPEC
#include "libusb-1.0/libusb.h"
typedef struct {
	struct libusb_context* _context;
	struct libusb_device_handle* _device;
	struct libusb_transfer* _in_transfer;
	struct libusb_transfer* _out_transfer;
	int _is_open;
	int _is_inited;
} omron_device_impl;
#endif

/*******************************************************************************
 *
 * Const global values
 *
 ******************************************************************************/

/// Vendor ID for all omron health devices
static const uint32_t OMRON_VID = 0x0590;
/// Product ID for all omron health devices
static const uint32_t OMRON_PID = 0x0028;

/// Out endpoint for all omron health devices
static const uint32_t OMRON_OUT_ENDPT = 0x02;
/// In endpoint for all omron health devices
static const uint32_t OMRON_IN_ENDPT  = 0x81;

/*******************************************************************************
 *
 * Omron device enumerations
 *
 ******************************************************************************/

/**
 * Enumeration for different omron device modes
 *
 * These modes dictate what we're currently trying to do with the
 * omron device. We send a control message with the mode, then
 * do things like get device info (serial number, version, etc...),
 * or engage in specific device communication events, like reading
 * pedometer or blood pressure data
 */
typedef enum
{
	/// Clearing modes and startup/shutdown
	NULL_MODE			= 0x0000,
	/// Getting serial numbers, version, etc...
	DEVICE_INFO_MODE	= 0x1111,
	/// Daily blood pressure info mode
	DAILY_INFO_MODE		= 0x74bc,
	/// Weekly blood pressure info mode
	WEEKLY_INFO_MODE	= 0x1074,
	/// Pedometer info mode
	PEDOMETER_MODE		= 0x0102
} omron_mode;

/**
 * Enumeration of device state
 *
 * Keeps the state of the device, and the current mode that it's in. If
 * we issue a command that needs a different mode, we can use the state
 * stored here to check that.
 */
typedef struct
{
	/// Device implementation
	omron_device_impl device;
	/// Mode the device is currently in
	omron_mode device_mode;
} omron_device;

/**
 * Enumeration of device information
 *
 * Stores information about the device version, serial, etc...
 */
typedef struct
{
	/// Version of the device
	uint8_t version[13];
	/// Can't remember what this is
	uint8_t prf[11];
	/// Serial number of the device
	uint8_t srl[8];
} omron_device_info;

/*******************************************************************************
 *
 * Blood pressure monitor specific enumerations
 *
 ******************************************************************************/

/**
 * Enumeration for daily blood pressure info
 *
 * Stores information taken on a daily basis for blood pressure
 * Usually, we consider there to be one morning and one evening
 * reading.
 */
typedef struct
{
	/// Day of reading
	uint32_t day;
	/// Month of reading
	uint32_t month;
	/// Year of reading
	uint32_t year;
	/// Hour of reading
	uint32_t hour;
	/// Minute of reading
	uint32_t minute;
	/// Second of reading
	uint32_t second;
	/// No idea
	uint8_t unknown_1[2];
	/// SYS reading
	uint32_t sys;
	/// DIA reading
	uint32_t dia;
	/// Pulse reading
	uint32_t pulse;
	/// No idea
	uint8_t unknown_2[3];
	/// 1 if week block is filled, 0 otherwise
	uint8_t present;
} omron_bp_day_info;

/**
 * Enumeration for weekly blood pressure info
 *
 * Stores information averages for a week
 */
typedef struct
{
	/// always 0x00
	uint8_t unknown_1;
	/// always 0x80
	uint8_t unknown_2;
	/// Year of reading
	uint32_t year;
	/// Month of reading
	uint32_t month;
	/// Day that weekly average starts on
	uint32_t day;
	/// always 0
	uint8_t unknown_3;
	/// SYS average for week
	int32_t sys;
	/// DIA average for week
	int32_t dia;
	/// Pulse average for week
	int32_t pulse;
	/// 1 if week block is filled, 0 otherwise
	uint8_t present;
} omron_bp_week_info;


/*******************************************************************************
 *
 * Pedometer specific enumerations
 *
 ******************************************************************************/

/**
 * Enumeration for pedometer profile information
 *
 * Stores information about user (weight, stride length, etc...)
 */
typedef struct
{
	/// ???
	uint8_t unknown_1[2];
	/// lbs times 10? i.e. 190 = {0x01, 0x90} off the device
	uint32_t weight;
	/// kg times 10? same as last
	uint32_t stride;
	/// ???
	uint8_t unknown_2[2];
} omron_pd_profile_info;

/**
 * Enumeration for count of valid pedometer information packets
 *
 * Contains the number of valid daily and hourly packets, for use by
 * programs for reading information off the device
 */
typedef struct
{
	/// Number of valid daily packets
	int32_t daily_count;
	/// Number of valid hourly packets
	int32_t hourly_count;
	/// No idea.
	uint8_t unknown_1;
} omron_pd_count_info;

/**
 * Enumeration for daily data packets from pedometer
 *
 * Daily information from pedometer, including steps, distance, etc...
 *
 */
typedef struct
{
	/// Total number of steps for the day
	int32_t total_steps;
	/// Total number of "aerobic" steps for the day
	int32_t total_aerobic_steps;
	/// Total time spent "aerobically" walking throughout the day (in minutes)
	int32_t total_aerobic_walking_time;
	/// Total calories burned kcal
	int32_t total_calories;
	/// Total distance (steps * stride) miles
	float total_distance;
	/// Total fat burned grams
	float total_fat_burn;
	/// Offset of date from current day
	int32_t day_serial;
	/// No idea
	uint8_t unknown_1;
} omron_pd_daily_data;

/**
 * Enumeration for hourly data packets from pedometer
 *
 * Hourly information about steps taken during a certain day
 */
typedef struct
{
	/// Offset of day from current day
	int32_t day_serial;
	/// Index of hour
	int32_t hour_serial;
	/// Was anything happening for the pedometer to record?
	uint8_t is_attached;
	/// Regular steps taken
	int32_t regular_steps;
	/// Aerobic steps taken
	int32_t aerobic_steps;
} omron_pd_hourly_data;


#ifdef __cplusplus
extern "C" {
#endif

	////////////////////////////////////////////////////////////////////////////////////
	//
	// Platform Specific Functions
	//
	////////////////////////////////////////////////////////////////////////////////////
	
	/**
	 * Returns the number of devices connected, though does not specify device type
	 *
	 * @param dev Device pointer
	 * @param VID Vendor ID, defaults to 0x0590
	 * @param PID Product ID, defaults to 0x0028
	 *
	 * @return Number of devices connected, or < 0 if error
	 */
	OMRON_DECLSPEC omron_device* omron_create();
	OMRON_DECLSPEC void omron_delete(omron_device* dev);

	/**
	 * Returns the number of devices connected, though does not specify device type
	 *
	 * @param dev Device pointer
	 * @param VID Vendor ID, defaults to 0x0590
	 * @param PID Product ID, defaults to 0x0028
	 *
	 * @return Number of devices connected, or < 0 if error
	 */
	OMRON_DECLSPEC int omron_get_count(omron_device* dev, int VID, int PID);

	/**
	 * Returns the number of devices connected, though does not specify device type
	 *
	 * @param dev Device pointer
	 * @param device_index Index of the device to open
	 * @param VID Vendor ID, defaults to 0x0590
	 * @param PID Product ID, defaults to 0x0028
	 *
	 * @return > 0 if ok, otherwise < 0
	 */
	OMRON_DECLSPEC int omron_open(omron_device* dev, int VID, int PID, uint32_t device_index);
	/**
	 * Closes an open omron device
	 *
	 * @param dev Device pointer to close
	 *
	 * @return > 0 if ok, otherwise < 0
	 */
	OMRON_DECLSPEC int omron_close(omron_device* dev);

	/**
	 * Sends the control message to set a new mode for the device
	 *
	 * @param dev Device pointer to set mode for
	 * @param mode Mode enumeration value, from omron_mode enum
	 *
	 * @return > 0 if ok, otherwise < 0
	 */
	OMRON_DECLSPEC int omron_set_mode(omron_device* dev, omron_mode mode);

	/**
	 * Reads data from the device
	 *
	 * @param dev Device pointer to read from
	 * @param input_report Buffer to read into (always 8 bytes)
	 *
	 * @return > 0 if ok, otherwise < 0
	 */
	OMRON_DECLSPEC int omron_read_data(omron_device* dev, uint8_t *input_report);

	/**
	 * Writes data to the device
	 *
	 * @param dev Device pointer to write to
	 * @param input_report Buffer to read from (always 8 bytes)
	 *
	 * @return > 0 if ok, otherwise < 0
	 */
	OMRON_DECLSPEC int omron_write_data(omron_device* dev, uint8_t *output_report);

	////////////////////////////////////////////////////////////////////////////////////
	//
	// Device Information Retrieval Functions
	//
	////////////////////////////////////////////////////////////////////////////////////

	/**
	 * Retrieves the serial number of the device
	 *
	 * @param dev Device to get serial number from
	 * @param data 8 byte buffer to read serial number into
	 *
	 * @return > 0 if ok, otherwise < 0
	 */
	OMRON_DECLSPEC int omron_get_device_serial(omron_device* dev, uint8_t* data);

	/**
	 * Retrieves the version number of the device
	 *
	 * @param dev Device to get version number from
	 * @param data 8 byte buffer to read version number into
	 *
	 * @return > 0 if ok, otherwise < 0
	 */
	OMRON_DECLSPEC int omron_get_device_version(omron_device* dev, uint8_t* data);

	////////////////////////////////////////////////////////////////////////////////////
	//
	// Blood Pressure Functions
	//
	////////////////////////////////////////////////////////////////////////////////////

	/**
	 * Get profile information for pedometer
	 *
	 * @param dev Device to query
	 * @param data 11 byte buffer for profile information
	 *
	 * @return 0 if successful, < 0 otherwise
	 */
	OMRON_DECLSPEC int omron_get_bp_profile(omron_device* dev, uint8_t* data);

	/**
	 * Gets a specific data index from a specific bank of readings
	 *
	 * @param dev Device to query
	 * @param bank Bank to query (A or B switch on device)
	 * @param index Index of packet to query in bank
	 *
	 * @return Data packet with requested information
	 */
	OMRON_DECLSPEC omron_bp_day_info omron_get_daily_bp_data(omron_device* dev, int bank, int index);

	/**
	 * Gets a specfic data index from a specific bank of readings
	 *
	 * @param dev Device to query
	 * @param bank Bank to query (A or B switch on device)
	 * @param index Index of packet to query in bank
	 * @param evening If 0, get morning average, If 1, get evening average.
	 *
	 * @return Data packet with requested information
	 */
	OMRON_DECLSPEC omron_bp_week_info omron_get_weekly_bp_data(omron_device* dev, int bank, int index, int evening);

	////////////////////////////////////////////////////////////////////////////////////
	//
	// Pedometer Functions
	//
	////////////////////////////////////////////////////////////////////////////////////

	/**
	 * Get pedometer profile information
	 *
	 * @param dev Device to query
	 *
	 * @return Struct with weight and stride info
	 */
	OMRON_DECLSPEC omron_pd_profile_info omron_get_pd_profile(omron_device* dev);

	/**
	 * Query device for number of valid data packets
	 *
	 * @param dev Device to query
	 *
	 * @return Struct with count information
	 */
	OMRON_DECLSPEC omron_pd_count_info omron_get_pd_data_count(omron_device* dev);

	/**
	 * Get data for a specific day index
	 *
	 * @param dev Device to query
	 * @param day Day index, should be between 0 and info retrieved from omron_get_pd_data_count
	 *
	 * @return Struct with data for day
	 */
	OMRON_DECLSPEC omron_pd_daily_data omron_get_pd_daily_data(omron_device* dev, int day);

	/**
	 * Get hourly data for a specific day index
	 *
	 * @param dev Device to query
	 * @param day Day index, should be between 0 and info retrieved from omron_get_pd_data_count
	 *
	 * @return Struct with hourly info for day
	 */
	OMRON_DECLSPEC omron_pd_hourly_data* omron_get_pd_hourly_data(omron_device* dev, int day);

#ifdef __cplusplus
}
#endif


#endif //LIBOMRON_H
