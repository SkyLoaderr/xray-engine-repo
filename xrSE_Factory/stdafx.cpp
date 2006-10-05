////////////////////////////////////////////////////////////////////////////
//	Module 		: stdafx.cpp
//	Created 	: 18.06.2004
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Precompiled header creator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma comment(lib,"x:/xrCore.lib")
#pragma comment(lib,"x:/xrLUA.lib")

// I need this because we have to exclude option /EHsc (exception handling) from the project
namespace boost {
	void throw_exception(const std::exception &A)
	{
		Debug.fatal(DEBUG_INFO,"Boost exception raised %s",A.what());
	}
};
