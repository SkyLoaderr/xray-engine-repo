////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory.cpp
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_factory.h"
#include "object_broker.h"

#ifndef _EDITOR
#	include "script_space.h"
#	include "ai_space.h"
#	include "script_engine.h"
#endif

CObjectFactory::CObjectFactory			()
{
	m_initialized				= false;
	register_classes			();
	std::sort					(m_clsids.begin(),m_clsids.end(),CObjectItemPredicate());
	m_initialized				= true;
}

CObjectFactory::~CObjectFactory			()
{
	delete_data					(m_clsids);
}

#ifndef _EDITOR
void CObjectFactory::register_script	() const
{
	luabind::class_<CInternal>	instance("clsid");
	
	const_iterator				I = clsids().begin(), B = I;
	const_iterator				E = clsids().end();
	for ( ; I != E; ++I)
		instance.enum_			("_clsid")[luabind::value(*(*I)->script_clsid(),int(I - B))];
	
	luabind::module				(ai().script_engine().lua())[instance];
}
#endif
