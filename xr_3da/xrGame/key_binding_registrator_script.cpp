#include "stdafx.h"
#include "key_binding_registrator.h"
#include "xr_level_controller.h"


#include "script_space.h"

using namespace luabind;

void key_binding_registrator::script_register(lua_State *L)
{
	module(L)
	[
		class_<key_binding_registrator>("key_bindings")
		.enum_("commands")
		[
			value("kFWD",						int(kFWD)),
			value("kBACK",						int(kBACK)),
			value("kL_STRAFE",					int(kL_STRAFE)),
			value("kR_STRAFE",					int(kR_STRAFE)),
			value("kLEFT",						int(kLEFT)),
			value("kRIGHT",						int(kRIGHT)),
			value("kUP",						int(kUP)),
			value("kDOWN",						int(kDOWN)),
			value("kJUMP",						int(kJUMP)),
			value("kCROUCH",					int(kCROUCH)),
			value("kACCEL",						int(kACCEL)),
			value("kREPAIR",					int(kREPAIR)),
			value("kCAM_1",						int(kCAM_1)),
			value("kCAM_2",						int(kCAM_2)),
			value("kCAM_3",						int(kCAM_3)),
			value("kCAM_4",						int(kCAM_4)),
			value("kCAM_ZOOM_IN",				int(kCAM_ZOOM_IN)),
			value("kCAM_ZOOM_OUT",				int(kCAM_ZOOM_OUT)),
			value("kBINOCULARS",				int(kBINOCULARS)),
			value("kTORCH",						int(kTORCH)),
			value("kWPN_1",						int(kWPN_1)),
			value("kWPN_2",						int(kWPN_2)),
			value("kWPN_3",						int(kWPN_3)),
			value("kWPN_4",						int(kWPN_4)),
			value("kWPN_5",						int(kWPN_5)),
			value("kWPN_6",						int(kWPN_6)),
			value("kWPN_7",						int(kWPN_7)),
			value("kWPN_8",						int(kWPN_8)),
			value("kWPN_9",						int(kWPN_9)),
			value("kWPN_NEXT",					int(kWPN_NEXT)),
			value("kWPN_PREV",					int(kWPN_PREV)),
			value("kWPN_FIRE",					int(kWPN_FIRE)),
			value("kWPN_RELOAD",				int(kWPN_RELOAD)),
			value("kWPN_ZOOM",					int(kWPN_ZOOM)),
			value("kWPN_AMMO",					int(kWPN_AMMO)),
			value("kWPN_FUNC",					int(kWPN_FUNC)),
			value("kUSE",						int(kUSE)),
			value("kDROP",						int(kDROP)),
			value("kSCORES",					int(kSCORES)),
			value("kCHAT",						int(kCHAT)),
			value("kSCREENSHOT",				int(kSCREENSHOT)),
			value("kQUIT",						int(kQUIT)),
			value("kCONSOLE",					int(kCONSOLE)),
			value("kINVENTORY",					int(kINVENTORY)),
			value("kBUY",						int(kBUY)),
			value("kSKIN",						int(kSKIN)),
			value("kTEAM",						int(kTEAM)),
			value("kEXT_1",						int(kEXT_1)),
			value("kEXT_2",						int(kEXT_2)),
			value("kEXT_3",						int(kEXT_3)),
			value("kEXT_4",						int(kEXT_4)),
			value("kEXT_5",						int(kEXT_5)),
			value("kEXT_6",						int(kEXT_6)),
			value("kEXT_7",						int(kEXT_7)),
			value("kEXT_8",						int(kEXT_8)),
			value("kEXT_9",						int(kEXT_9)),
			value("kEXT_10",					int(kEXT_10)),
			value("kEXT_11",					int(kEXT_11)),
			value("kEXT_12",					int(kEXT_12)),
			value("kEXT_13",					int(kEXT_13)),
			value("kEXT_14",					int(kEXT_14)),
			value("kEXT_15",					int(kEXT_15))
		]

	];
}
