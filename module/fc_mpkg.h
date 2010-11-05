// talk to FC with mkpackage

#ifndef _FC_MPKG_H_
#define _FC_MPKG_H_

#include "logger.h"
// #include "thread.h"
#include "protocollayer.h"
//#include "qk_datatypes.h"

namespace mavhub {
  // class FC_Mpkg : public cpp_pthread::PThread {
  class FC_Mpkg : public AppLayer {
  public:
	 FC_Mpkg();
	 virtual ~FC_Mpkg();
	 virtual void handle_input(const mavlink_message_t &msg);

	 enum mk_debugout_map_t {
		USSvalue = 0,
		USSlastvalid = 1,
		USScred = 2,
		ADval_press = 3,
		ATTabsh = 4,
		ATTrelh = 5,
		USSoffset = 6,
		USSstatus = 7,
		ADval_gyrroll = 8,
		ADval_gyrnick = 9,
		ADval_gyryaw = 10,
		ATTrelacctopint = 11,
		ADval_ubat = 12,
		GASmixfrac1 = 13,
		GASmixfrac2 = 14,
		RC_rssi = 15,
		ATTmeanaccnick = 16,
		ATTmeanaccroll = 17,
		ATTmeanacctop = 18,
		ATTintnickl = 19,
		ATTintrolll = 20,
		ATTintyawl = 21,
		FCParam_extctlswitch = 22,
		FCParam_gpsswitch = 23,
		ADval_accnick = 24,
		ADval_accroll = 25,
		ADval_acctop = 26,
		CTL_stickgas = 27,
		ADval_acctopraw = 28,
		ATTintnickh = 29,
		ATTintrollh = 30,
		ATTintyawh = 31
	 };

  protected:
	 virtual void run();
  private:
	 // DebugOut_t *mk_debugout_o;
	 mavlink_mk_debugout_t mk_debugout;
	 mavlink_huch_attitude_t huch_attitude;
	 mavlink_huch_altitude_t huch_altitude;
	 mavlink_huch_exp_ctrl_t huch_exp_ctrl;
	 mavlink_huch_ranger_t huch_ranger;
	 // void test();
	 int mk_debugout_digital_offset;

	 // fetch different types from byte based mk_debugout
	 uint16_t debugout_getval_u(mavlink_mk_debugout_t* dbgout,
										 int index);
	 int16_t debugout_getval_s(mavlink_mk_debugout_t* dbgout,
										 int index);
	 int32_t debugout_getval_s32(mavlink_mk_debugout_t* dbgout,
										  int indexl, int indexh);

	 void debugout2attitude(mavlink_mk_debugout_t* dbgout, mavlink_huch_attitude_t* attitude);
	 void debugout2altitude(mavlink_mk_debugout_t* dbgout, mavlink_huch_altitude_t* altitude);
	 void debugout2exp_ctrl(mavlink_mk_debugout_t* dbgout, mavlink_huch_exp_ctrl_t* exp_ctrl);
	 void debugout2ranger(mavlink_mk_debugout_t* dbgout, mavlink_huch_ranger_t* ranger);
  };
}

#endif
