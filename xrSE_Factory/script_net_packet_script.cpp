////////////////////////////////////////////////////////////////////////////
//	Module 		: script_net packet_script.cpp
//	Created 	: 06.02.2004
//  Modified 	: 24.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script net packet class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_net_packet.h"
#include "script_space.h"
#include "net_utils.h"
#include <luabind/adopt_policy.hpp>

using namespace luabind;

LPCSTR script_section = "script";
LPCSTR current_version = "current_server_entity_version";

bool r_eof(NET_Packet *self)
{
	return			(!!self->r_eof());
}

LPSTR r_stringZ(NET_Packet *self)
{
	ref_str			temp;
	self->r_stringZ	(temp);
	return			(xr_strdup(*temp));
}

extern u16	script_server_object_version	();

void CScriptNetPacket::script_register(lua_State *L)
{
	module(L)
	[
		def("script_server_object_version", &script_server_object_version),

		class_<NET_Packet>("net_packet")
			.def(					constructor<>()				)
			.def("w_begin",			&NET_Packet::w_begin		)
//			.def("w",				&NET_Packet::w				)
//			.def("w_seek",			&NET_Packet::w_seek			)
			.def("w_tell",			&NET_Packet::w_tell			)
			.def("w_vec3",			&NET_Packet::w_vec3			)
			.def("w_float",			&NET_Packet::w_float		)
			.def("w_u64",			&NET_Packet::w_u64			)
			.def("w_s64",			&NET_Packet::w_s64			)
			.def("w_u32",			&NET_Packet::w_u32			)
			.def("w_s32",			&NET_Packet::w_s32			)
			.def("w_u24",			&NET_Packet::w_u24			)
			.def("w_u16",			&NET_Packet::w_u16			)
			.def("w_s16",			&NET_Packet::w_s16			)
			.def("w_u8",			&NET_Packet::w_u8			)
			.def("w_s8",			&NET_Packet::w_s8			)
			.def("w_float_q16",		&NET_Packet::w_float_q16	)
			.def("w_float_q8",		&NET_Packet::w_float_q8		)
			.def("w_angle16",		&NET_Packet::w_angle16		)
			.def("w_angle8",		&NET_Packet::w_angle8		)
			.def("w_dir",			&NET_Packet::w_dir			)
			.def("w_sdir",			&NET_Packet::w_sdir			)
			.def("w_stringZ",		(void (NET_Packet::*)(LPCSTR))(&NET_Packet::w_stringZ	))
			.def("w_matrix",		&NET_Packet::w_matrix		)
			.def("w_chunk_open8",	&NET_Packet::w_chunk_open8	)
			.def("w_chunk_close8",	&NET_Packet::w_chunk_close8	)
			.def("r_begin",			&NET_Packet::r_begin		)
//			.def("r",				&NET_Packet::r				)
			.def("r_seek",			&NET_Packet::r_seek			)
			.def("r_tell",			&NET_Packet::r_tell			)
			.def("r_vec3",			&NET_Packet::r_vec3			)
			.def("r_float",			(void (NET_Packet::*)(float&))(&NET_Packet::r_float		))
			.def("r_u64",			(void (NET_Packet::*)(u64&	))(&NET_Packet::r_u64		))
			.def("r_s64",			(void (NET_Packet::*)(s64&	))(&NET_Packet::r_s64		))
			.def("r_u32",			(void (NET_Packet::*)(u32&	))(&NET_Packet::r_u32		))
			.def("r_s32",			(void (NET_Packet::*)(s32&	))(&NET_Packet::r_s32		))
			.def("r_u24",			(void (NET_Packet::*)(u32&	))(&NET_Packet::r_u24		))
			.def("r_u16",			(void (NET_Packet::*)(u16&	))(&NET_Packet::r_u16		))
			.def("r_s16",			(void (NET_Packet::*)(s16&	))(&NET_Packet::r_s16		))
			.def("r_u8",			(void (NET_Packet::*)(u8&)	)(&NET_Packet::r_u8			))
			.def("r_s8",			(void (NET_Packet::*)(s8&)	)(&NET_Packet::r_s8			))
			.def("r_float",			(float	(NET_Packet::*)()	)(&NET_Packet::r_float		))
			.def("r_u64",			(u64	(NET_Packet::*)()	)(&NET_Packet::r_u64		))
			.def("r_s64",			(s64	(NET_Packet::*)()	)(&NET_Packet::r_s64		))
			.def("r_u32",			(u32	(NET_Packet::*)()	)(&NET_Packet::r_u32		))
			.def("r_s32",			(s32	(NET_Packet::*)()	)(&NET_Packet::r_s32		))
			.def("r_u24",			(u32	(NET_Packet::*)()	)(&NET_Packet::r_u24		))
			.def("r_u16",			(u16	(NET_Packet::*)()	)(&NET_Packet::r_u16		))
			.def("r_s16",			(s16	(NET_Packet::*)()	)(&NET_Packet::r_s16		))
			.def("r_u8",			(u8		(NET_Packet::*)()	)(&NET_Packet::r_u8			))
			.def("r_s8",			(s8		(NET_Packet::*)()	)(&NET_Packet::r_s8			))
			.def("r_float_q16",		&NET_Packet::r_float_q16	)
			.def("r_float_q8",		&NET_Packet::r_float_q8		)
			.def("r_angle16",		&NET_Packet::r_angle16		)
			.def("r_angle8",		&NET_Packet::r_angle8		)
			.def("r_dir",			&NET_Packet::r_dir			)
			.def("r_sdir",			&NET_Packet::r_sdir			)
			.def("r_stringZ",		&r_stringZ					, adopt(return_value))
			.def("r_matrix",		&NET_Packet::r_matrix		)
			.def("r_elapsed",		&NET_Packet::r_elapsed		)
			.def("r_advance",		&NET_Packet::r_advance		)
			.def("r_eof",			&r_eof						)

	];
}