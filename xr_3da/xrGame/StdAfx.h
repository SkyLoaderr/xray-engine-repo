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

#if XRAY_EXCEPTIONS
#	define	THROW(xpr)				if (!(xpr)) {throw __FILE__LINE__"\""#xpr"\"";}
#	define	THROW2(xpr,msg0)		if (!(xpr)) {throw *shared_str().sprintf("%s \"%s\" : %s",__FILE__LINE__,#xpr,msg0 ? msg0 : "");}
#	define	THROW3(xpr,msg0,msg1)	if (!(xpr)) {throw *shared_str().sprintf("%s \"%s\" : %s, %s",__FILE__LINE__,#xpr,msg0 ? msg0 : "",msg1 ? msg1 : "");}
#else
#	define	THROW					VERIFY
#	define	THROW2					VERIFY2
#	define	THROW3					VERIFY3
#endif
