////////////////////////////////////////////////////////////////////////////
//	Module 		: object_broker.h
//	Created 	: 21.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object broker
////////////////////////////////////////////////////////////////////////////

#pragma once

//#define XRAY_COMPATIBIBLE

#ifdef XRAY_COMPATIBIBLE
#	include "net_utils.h"
#else
#	define xr_free		c_free
#	define xr_delete	c_delete
#endif

#include <queue>
#include "object_interfaces.h"
#include "object_type_traits.h"
#include "object_comparer.h"
#include "object_cloner.h"
#include "object_destroyer.h"
#include "object_loader.h"
#include "object_saver.h"