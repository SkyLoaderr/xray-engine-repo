////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory.cpp
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "object_factory.h"
#include "object_broker.h"

#ifndef NO_XR_GAME
#	include "xrServer_Objects_ALife_All.h"
#	include "attachable_item.h"
#endif

CObjectFactory::CObjectFactory	()
{
	m_initialized				= false;
	register_classes			();
#ifndef NO_SCRIPTS
	register_script_classes		();
#endif
	std::sort					(m_clsids.begin(),m_clsids.end(),CObjectItemPredicate());
	m_initialized				= true;
}

CObjectFactory::~CObjectFactory	()
{
	delete_data					(m_clsids);
}
