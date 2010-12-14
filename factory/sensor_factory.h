#ifndef _SENSOR_FACTORY_H_
#define _SENSOR_FACTORY_H_

#include <string>
#include <map>

#include "sensormanager.h"

// Sensors
#include "module/senbmp085.h"
#include "module/senhmc5843.h"
#include "module/senExpCtrl.h"

namespace mavhub {
	class SensorFactory {
	public:
		static void build(const std::string& app_name, const std::map<std::string, std::string> args);
	};
}
#endif
