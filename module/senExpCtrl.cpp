#include "senExpCtrl.h"

#include <math.h> //pow
#include <iostream> //cout
#include <sys/time.h> //gettime
#include <sstream> //stringstream

#include "logger.h" //"printf"
#include "utility.h"
#include "datacenter.h"

using namespace std;

namespace mavhub {

SenExpCtrl::SenExpCtrl(unsigned short _dev_id, 
	unsigned short _func_id, 
	std::string _port, 
	int _update_rate, 
	int _debug, 
	int _timings ) throw(const char *)
{
	//FIXME Initialisierung
	dev_id = _dev_id;
	func_id = _func_id;
	update_rate = _update_rate;
	debug = _debug;
	timings = _timings; 

	Logger::log("ExpCtrl: init...", timings, Logger::LOGLEVEL_INFO);

	status = RUNNING;	
	try {
		/* check config */
		if (get_data_pointer(func_id << 16) == NULL) {
			throw "sensor doesn't support configured function";
		}

		fd = i2c_init(_port);
		status = STOPPED;
	}
	catch(const char *message) {
		status = UNINITIALIZED;

		string s(message);
		throw ("ExpCtrl(): " + s).c_str();
	}
}

SenExpCtrl::~SenExpCtrl() {
	status = STOPPED;
	join();
}

void SenExpCtrl::run() {

	char buffer[256];
	int wait_freq = update_rate? 1000000 / update_rate: 0;
	int wait_time = wait_freq;

	uint64_t frequency = wait_time;
	uint64_t start = get_time_us();
	uint64_t time_output = start + 1000000;
	uint64_t usec;
	vector<uint16_t> exprx_value(4);
	//mavlink_message_t msg;

	Logger::log("ExpCtrl: running", update_rate, Logger::LOGLEVEL_INFO);
	// uint64_t end = getTimeUs() + 1000000;

	try {
		status = RUNNING;
		
		while((status == RUNNING) && update_rate) {

			/* wait time */
			usec = get_time_us();
			uint64_t end = usec;
			wait_time = wait_freq - (end - start);
			wait_time = (wait_time < 0)? 0: wait_time;
		
			/* wait */
			//usleep(wait_time);
			usleep(10);

			/* calculate frequency */
			end = get_time_us();
			frequency = (15 * frequency + end - start) / 16;
			start = end;

			i2c_start_conversion(fd, EXPCTRL_ADR);

			// set read register
			buffer[0] = 0x00;
			i2c_write_bytes(fd, (uint8_t*)buffer, 1);

			// read data
			i2c_read_bytes(fd, (uint8_t*)buffer, 9);

			// get version
			exp_ctrl_rx_data.version = *buffer;
			
			i2c_end_conversion(fd);

			// get values
			memcpy(&exprx_value[0], buffer+1, 8);

			// XXX: smart++
			exp_ctrl_rx_data.value0 = exprx_value[0];
			exp_ctrl_rx_data.value1 = exprx_value[1];
			exp_ctrl_rx_data.value2 = exprx_value[2];
			exp_ctrl_rx_data.value3 = exprx_value[3];

			// XXX: kopter specific mapping
			// XXX: 0 is USS
			huch_ranger.ranger2 = exprx_value[2];
			huch_ranger.ranger3 = exprx_value[0];

			publish_data(start);

			/* assign buffer to data */
			{ // begin of data mutex scope
				cpp_pthread::Lock ri_lock(data_mutex);
				sensor_data[0].distance = exprx_value[2];
				sensor_data[0].usec = start;
				sensor_data[1].distance = exprx_value[0];
				sensor_data[1].usec = start;
			} // end of data mutex scope

			
			// Logger::log("ExpCtrl:", (int)exp_ctrl_rx_data.version, exprx_value, Logger::LOGLEVEL_INFO);
			//Logger::log("ExpCtrl rx_t:", (int)exp_ctrl_rx_data.version, exp_ctrl_rx_data.value0, Logger::LOGLEVEL_INFO);

			// pass more data
			// XXX: system_id
			// mavlink_msg_huch_exp_ctrl_rx_encode(39, static_cast<uint8_t>(component_id), &msg, &exp_ctrl_rx_data);

			/* debug data */
			if (debug) print_debug();

			/* timings/benchmark output */
			if (timings) {
				if (time_output <= end) {
					Logger::log("ExpCtrl frequency: ", (float)1000000/frequency, Logger::LOGLEVEL_DEBUG);
					//Logger::log("hmc5843 wait_time: ", wait_time, Logger::LOGLEVEL_DEBUG);
					time_output += 1000000;
				}
			}
		}
	}
	catch(const char *message) {
		i2c_end_conversion(fd);
		status = STRANGE;

		string s(message);
		throw ("ExpCtrl::run(): " + s).c_str();
	}

	Logger::debug("exp_ctrl: stopped");
}

void SenExpCtrl::print_debug() {
	ostringstream send_stream;
	send_stream << "ExpCtrl version: " << (int)exp_ctrl_rx_data.version << ", v0:" << exp_ctrl_rx_data.value0 << ", v1:" << exp_ctrl_rx_data.value1 << ", v2:" << exp_ctrl_rx_data.value2 << ", v3:" << exp_ctrl_rx_data.value3;
	Logger::debug(send_stream.str());
}

void SenExpCtrl::publish_data(uint64_t time) {
	DataCenter::set_exp_ctrl(exp_ctrl_rx_data);
	// XXX: hardware specific mapping
	DataCenter::set_huch_ranger_at(huch_ranger, 1);
	DataCenter::set_huch_ranger_at(huch_ranger, 2);
}

void* SenExpCtrl::get_data_pointer(unsigned int id) throw(const char *) {
	if (status == RUNNING) {
		switch ((0xFFFF0000 & id) >> 16) {
			case (DISTANCE_SENSOR): // sensor 0
				return &sensor_data[0];
			case (DISTANCE_SENSOR | 0x1000): // sensor 1
				return &sensor_data[1];
			default: throw "sensor exp_ctrl doesn't support this sensor type";
		}
	} throw "sensor exp_ctrl isn't running";
}


} // namespace mavhub
