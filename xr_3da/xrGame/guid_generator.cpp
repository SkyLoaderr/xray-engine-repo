////////////////////////////////////////////////////////////////////////////
//	Module 		: guid_generator.cpp
//	Created 	: 21.03.2005
//  Modified 	: 21.03.2005
//	Author		: Dmitriy Iassenev
//	Description : GUID generator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include <rpcdce.h>
#include <rpc.h>

#pragma comment(lib,"rpcrt4.lib")

GUID generate_guid()
{
	GUID			result;
	RPC_STATUS		gen_result = UuidCreate(&result);
	switch (gen_result) {
		case RPC_S_OK				: return(result);
		case RPC_S_UUID_LOCAL_ONLY	: return(result);
		case RPC_S_UUID_NO_ADDRESS	: 
		default						: break;
	}
	STATIC_CHECK(sizeof(result) >= sizeof(u64),GUID_must_have_size_greater_or_equal_to_the_long_long);
	ZeroMemory		(&result,sizeof(result));
	u64				temp = CPU::GetCycleCount();
	Memory.mem_copy	(&result,&temp,sizeof(temp));
	return			(result);
}
