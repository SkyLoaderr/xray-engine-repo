#pragma once

#pragma warning(disable:4995)
#include "../stdafx.h"
#pragma warning(default:4995)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )

#pragma warning(disable:4505)

// this include MUST be here, since smart_cast is used >1800 times in the project
#include "smart_cast.h"

#define READ_IF_EXISTS(ltx,method,section,name,default_value)\
	(ltx->line_exist(section,name)) ? ltx->method(section,name) : default_value

