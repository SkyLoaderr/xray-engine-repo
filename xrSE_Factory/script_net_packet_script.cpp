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

using namespace luabind;

void CScriptNetPacket::script_register(lua_State *L)
{
	module(L)
	[
		class_<NET_Packet>("net_packet")
			.def(	constructor<>())
	];
}