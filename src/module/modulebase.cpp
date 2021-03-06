#include "modulebase.h"

#ifdef HAVE_MAVLINK_H

using namespace std;

namespace mavhub {
	ModuleBase::ModuleBase(const map<string, string> args, string name) :
		AppInterface(name),
		AppLayer<mavlink_message_t>(name) {
	}

	ModuleBase::~ModuleBase() {
	}
}

#endif // HAVE_MAVLINK_H