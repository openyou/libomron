/*

  Simple C++ wrapper to the C API from omron.h

*/
#ifndef OMRON_H_SEEN
#define OMRON_H_SEEN

#include "omron.h"

// Not in omron.h's API?
extern "C" {
int omron_get_daily_data_count(omron_device* dev, unsigned char bank);
}

#include <string>

class Omron {
    omron_device device;
public:
    typedef std::string data;

    Omron() {
	device.device = 0;
	device.device_mode = NULL_MODE;
    }
    ~Omron();

    int open(int VID, int PID, unsigned int device_index = 0) {
	return omron_open(&device, VID, PID, device_index);
    }

    int close() {
	return omron_close(&device);
    }

    int set_mode(omron_mode mode) {
	return omron_set_mode(&device, mode);
    }

    // not yet wrapped
    //int omron_read_data(omron_device* dev, unsigned char *input_report);
    //int omron_write_data(omron_device* dev, unsigned char *output_report);
    //int omron_get_device_serial(omron_device* dev, unsigned char* data);

    Omron::data get_device_version() {
	unsigned char buffer[30]={0}; /* buffer overflow begging to happen */
	omron_get_device_version(&device, buffer);
	return datify(buffer,30);
    }

    // Blood Pressure Functions
    Omron::data get_bp_profile() {
	unsigned char buffer[30]={0}; /* buffer overflow begging to happen */
	int ret = omron_get_bp_profile(&device, buffer);
	if (ret < 0) return Omron::data();
	return datify(buffer,30);
    }

    //daily data information

    int get_daily_data_count(int bank = 0) {
	return omron_get_daily_data_count(&device, bank);
    }

    // Not fully implemented
    // omron_bp_day_info* get_all_daily_bp_data(int* count) {
    // 	return omron_get_all_daily_bp_data(&device, count);
    // }

    // Note, reversal of arguments form C API
    omron_bp_day_info get_daily_bp_data(int index, int bank = 0) {
	return omron_get_daily_bp_data(&device, bank, index);
    }

    //weekly data information

    // Not fully implemented
    // omron_bp_week_info* get_all_weekly_bp_data(int* count) {
    // 	return omron_get_all_weekly_bp_data(&device, count);
    // }

    // Note, reversal of arguments from C API
    omron_bp_week_info get_weekly_bp_data(int index, 
					  bool evening=true, int bank=0) {
	return omron_get_weekly_bp_data(&device, bank, index, evening?1:0);
    }

    // Pedometer Functions
    omron_pd_profile_info get_pd_profile() {
	return omron_get_pd_profile(&device);
    }
    omron_pd_count_info get_pd_data_count(omron_device* dev) {
	return get_pd_data_count(&device);
    }
    omron_pd_daily_data get_pd_daily_data(int day) {
	return omron_get_pd_daily_data(&device, day);	
    }
    omron_pd_hourly_data* get_pd_hourly_data(int day) {
	return omron_get_pd_hourly_data(&device, day);
    }

private:

    Omron::data datify(unsigned char* data, int maxlen) {
	std::string ret;
	for (int ind=0; ind<maxlen && data[ind]; ++ind) {
	    char c = data[ind];
	    ret.push_back(c);
	}
	return ret;
    }

};


#endif
