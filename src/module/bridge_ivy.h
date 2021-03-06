#ifndef _BRIDGE_IVY_H_
#define _BRIDGE_IVY_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#ifdef HAVE_MAVLINK_H
#include <mavlink.h>

#ifdef HAVE_IVY_IVY_H

#include "Ivy/ivy.h"
#include "Ivy/ivyloop.h"
#include "Ivy/timer.h"

#include <inttypes.h> //uint8_t
#include "protocol/protocollayer.h"

namespace mavhub {

	class Bridge_Ivy : public AppLayer<mavlink_message_t> {
		public:
			Bridge_Ivy(const std::map<std::string, std::string> args);
			virtual ~Bridge_Ivy();
			virtual void handle_input(const mavlink_message_t &msg);

			virtual void send_mavlink_msg(mavlink_message_t* msg);

			virtual bool doDlValue_();
			virtual void _doDlValue(bool val);

		protected:
			virtual void run();

	private:
			uint16_t component_id;

			bool doDlValue;

			virtual void read_conf(const std::map<std::string, std::string> args);
			//virtual void handle_timer (TimerId id, void *data, unsigned long delta);

			// trying to use member functions as callbacks in libivy
			//virtual void  afterSelect_cb(void* arg);
			//void handle_timer (TimerId id, void *data, unsigned long delta);
			//void DlSettingCallback (IvyClientPtr app, void *data, int argc, char **argv);
			
	};
	
	// ----------------------------------------------------------------------------
	// Bridge_Ivy
	// ----------------------------------------------------------------------------


} // namespace mavhub

#endif // HAVE_IVY_IVY_H
#endif // HAVE_MAVLINK_H
#endif
