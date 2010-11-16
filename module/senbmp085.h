#ifndef _SENBMP085_H_
#define _SENBMP085_H_

#include <inttypes.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include <mavlink.h>

#include "logger.h"
#include "i2csensor.h"


#define	BMP085_ADR	0x77

namespace mavhub {

	typedef struct {
		int16_t ac1;
		int16_t ac2;
		int16_t ac3;
		uint16_t ac4;
		uint16_t ac5;
		uint16_t ac6;
		int16_t b1;
		int16_t b2;
		int16_t mb;
		int16_t mc;
		int16_t md;
	} calibration_data_t;


	class SenBmp085 : public I2cSensor {
		public:
			SenBmp085(int _fd, int _update_temp, int _oversampling, int _output);
			virtual ~SenBmp085();
			void print_debug();

		protected:
			virtual void run();

		private:
			void publish_data();
			void i2c_set_adr(const int fd, const int adr); // throw error
			static void read_calibration_data(const int fd, calibration_data_t &cal_data); // throw error
			static void request_temp_data(const int fd); // throw error
			static uint64_t request_pres_data(const int fd, const int oversampling); // throw error
			static int get_temp_data(const int fd); // throw error
			static int get_pres_data(const int fd, const int oversampling); // throw error

			static int calc_temp(const int uncomp_temp, int &b5, const calibration_data_t &cal_data);
			static int calc_pres(const int b5, const int oversampling, const int uncomp_pres, const calibration_data_t &cal_data);
			static float calc_altitude(const int p, const int p0);

			mavlink_huch_bmp085_t bmp085_data;
			calibration_data_t calibration_data;

			int oversampling;
			int pressure_0;
			int fd;
			int update_temp;
			int output;
			bool running;

			static const int wait_oversampling[4];
			#define WAITTEMP 4500
	};
	// ----------------------------------------------------------------------------
	// I2cSensors
	// ----------------------------------------------------------------------------	
	inline void SenBmp085::i2c_set_adr(const int fd, const int adr) {
		if (ioctl(fd, I2C_SLAVE, adr) < 0) {
			Logger::warn("SenBmp085: Failed to acquire bus access and/or talk to slave.");
		}
	}


} // namespace mavhub

#endif
