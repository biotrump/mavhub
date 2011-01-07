#include "main.h"

#include "core/logger.h"
#include "core/protocolstack.h"
#include "core/protocollayer.h"
#include "mavshell.h"

#include <iostream> //cout
#include <cstring> //strcmp
#include <cstdlib> //exit

using namespace std;
using namespace mavhub;
using namespace cpp_pthread;
using namespace cpp_io;

uint16_t system_id(42);
int tcp_port(32000);
string cfg_filename("mavhub.d/mavhub.conf");

int main(int argc, char **argv) {

	//list<I2cSensor*> i2c_sensors;
	Logger::setLogLevel(Logger::LOGLEVEL_WARN);

	parse_argv(argc, argv);

	//open config file and read settings
	try {
		Setting settings(cfg_filename, std::ios_base::in);
		read_settings(settings);
	}
	catch(const invalid_argument &ia) {
		Logger::log(ia.what(), Logger::LOGLEVEL_WARN);
	}

	// start sensors
	SensorManager::instance().start_all_sensors();

	//activate stack
	ProtocolStack::instance().start();
	
	//start mav shell
	try {
		MAVShell *mav_shell = new MAVShell(tcp_port);
		mav_shell->start();
		mav_shell->join();
	}
	catch(const char* message) {
		cout << message << endl;
	}

	//join stack thread
	ProtocolStack::instance().join();
}

void read_settings(Setting &settings) {
	//read loglevel
	Logger::log_level_t loglevel;
	if( settings.value("loglevel", loglevel) )
		Logger::log("Loglevel is missing in config file: ", cfg_filename, Logger::LOGLEVEL_WARN);
	else
		Logger::setLogLevel(loglevel);

	//read system ID
	if( settings.value("system_id", system_id) )
		Logger::log("System ID is missing in config file: ", cfg_filename, Logger::LOGLEVEL_WARN);
	ProtocolStack::instance().system_id(system_id);

	//read TCP port of MAVShell
	if( settings.value("tcp_port", tcp_port) )
		Logger::log("TCP port is missing in config file: ", cfg_filename, Logger::LOGLEVEL_WARN);

	//read interfaces (links)
	list<string> link_list;
	if( settings.value("interfaces", link_list) ) {
		Logger::log("List of links is missing in config file: ", cfg_filename, Logger::LOGLEVEL_WARN);
	} else {
		add_links(link_list, settings);
	}

	//read apps
	list<string> app_list;
	if( settings.value("applications", app_list) ) {
		Logger::log("List of apps is missing in config file: ", cfg_filename, Logger::LOGLEVEL_WARN);
	} else {
		add_apps(app_list, settings);
	}

	//read sensors
	list<string> sensors_list;
	if ( settings.value("sensors", sensors_list) ) {
		Logger::log("List of sensors is missing in config file: ", cfg_filename, Logger::LOGLEVEL_WARN);
	} else {
		add_sensors(sensors_list, settings);
	}
}

void add_links(const list<string> link_list, Setting &settings) {
	LinkFactory::link_construction_plan_t link_construction_plan;

	for(list<string>::const_iterator link_iter = link_list.begin(); link_iter != link_list.end(); ++link_iter) {
		try { //first read from sub config file
			Setting link_settings(settings.path() + string("/") + *link_iter);
			read_link_configuration(link_construction_plan, link_settings);
		}
		catch(const invalid_argument &ia) {
			Logger::log(ia.what(), Logger::LOGLEVEL_DEBUG);
		}
		
		if( settings.begin_group(*link_iter) == 0) { //link group available
			//read from global config file
			read_link_configuration(link_construction_plan, settings);
			
			settings.end_group();
		} else {
			Logger::log(*link_iter, " group missing in config file", Logger::LOGLEVEL_DEBUG);
		}

		cpp_io::IOInterface *layer = LinkFactory::build(link_construction_plan);
		ProtocolStack::instance().add_link(layer, link_construction_plan.package_format);
	}
}

void read_link_configuration(LinkFactory::link_construction_plan_t &link_plan, Setting &settings) {
	settings.value("type", link_plan.link_type);
	settings.value("name", link_plan.dev_name);
	settings.value("port", link_plan.port);
	settings.value("baudrate", link_plan.baudrate);
	settings.value("protocol", link_plan.package_format);
	settings.value("members", link_plan.groupmember_list);
}

void add_apps(const std::list<std::string> &app_list, Setting &settings) {

	AppLayer *app;
	map<string, string> arg_map;
	for(list<string>::const_iterator app_iter = app_list.begin(); app_iter != app_list.end(); ++app_iter) {
		//read from global config file first (because map wouldn't overwrite existing entries)
		if( settings.begin_group(*app_iter) == 0) { //app group available
			settings.values(arg_map);
			settings.end_group();
		}
		try { //next read from sub config file
			Setting app_settings(settings.path() + string("/") + *app_iter);
			app_settings.values(arg_map);
		}
		catch(const invalid_argument &ia) {
			Logger::log(ia.what(), Logger::LOGLEVEL_DEBUG);
		}

		app = AppFactory::build(*app_iter, arg_map);
		ProtocolStack::instance().add_application(app);
		arg_map.clear();
	}
}

void add_sensors(const std::list<std::string> &sensors_list, Setting &settings) {

	map<string, string> arg_map;
	for(list<string>::const_iterator sensors_iter = sensors_list.begin(); sensors_iter != sensors_list.end(); ++sensors_iter) {
		//read from global config file first (because map wouldn't overwrite existing entries)
		if( settings.begin_group(*sensors_iter) == 0) { //sensor group available
			settings.values(arg_map);
			settings.end_group();
		}
		try { //next read from sub config file
			Setting sensors_settings(settings.path() + string("/") + *sensors_iter);
			sensors_settings.values(arg_map);
		}
		catch(const invalid_argument &ia) {
			Logger::log(ia.what(), Logger::LOGLEVEL_DEBUG);
		}

		SensorFactory::build(*sensors_iter, arg_map);
		arg_map.clear();
	}
}

void parse_argv(int argc, char **argv) {
	for(int i=1; i<argc; i++) {
		if( !strcmp(argv[i], "-c") || !strcmp(argv[i], "--config") ) {
			if(++i < argc) {
				cfg_filename = argv[i];
			} else {
				cout << "ERROR: file argument missing" << endl;
				exit(-1);
			}
		} else 	if( !strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") ) {
			print_help();
			exit(0);
		} else if( !strcmp(argv[i], "-p") || !strcmp(argv[i], "--port") ) {
			if(++i < argc) {
				tcp_port = atoi(argv[i]);
			} else {
				cout << "ERROR: port argument missing" << endl;
				exit(-1);
			}
		} else {
			cout << "ERROR: " << argv[i] << " is no valid argument" << endl;
		}
	}
}

void print_help() {
	cout << "NAME" << endl;
	cout << "\t" << "MAVHUB - Micro Air Vehicle HUB" << endl << endl;
	cout << "SYNOPSIS" << endl;
	cout << "\t" << "mavhub [-h]" << endl << endl;
	cout << "OPTIONS" << endl;
	cout << "\t-c <file> or --config <file>" << endl;
	cout << "\t\t" << "open config file <file>" << endl;
	cout << "\t-h or --help" << endl;
	cout << "\t\t" << "print usage summary" << endl;
	cout << "\t-p <port> or --port <port>" << endl;
	cout << "\t\t" << "set port of management console" << endl;
}