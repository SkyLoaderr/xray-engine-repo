////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_script.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item script class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_factory_space.h"
#include "object_item_abstract.h"
#include "script_space.h"

class CObjectItemScript : public CObjectItemAbstract {
protected:
	typedef CObjectItemAbstract	inherited;
	luabind::functor<void>		m_client_creator;
	luabind::functor<void>		m_server_creator;

public:
	IC											CObjectItemScript	(
#ifndef NO_XR_GAME
													luabind::functor<void> client_creator, 
#endif
													luabind::functor<void> server_creator, 
													const CLASS_ID &clsid, 
													LPCSTR script_clsid
												);
#ifndef NO_XR_GAME
	virtual ObjectFactory::CLIENT_BASE_CLASS	*client_object		() const;
#endif
	virtual ObjectFactory::SERVER_BASE_CLASS	*server_object		(LPCSTR section) const;
};

#include "object_item_script_inline.h"