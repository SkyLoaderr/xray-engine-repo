////////////////////////////////////////////////////////////////////////////
//	Module 		: server_entity_wrapper.h
//	Created 	: 16.10.2004
//  Modified 	: 16.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Server entity wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_interfaces.h"

class CSE_Abstract;

class CServerEntityWrapper : public IPureSerializeObject {
private:
	CSE_Abstract	*m_object;

public:
	IC				CServerEntityWrapper	(CSE_Abstract *object = 0);
	virtual			~CServerEntityWrapper	();	
	virtual	void	save					(IWriter &stream);
	virtual	void	load					(IReader &stream);
};

#include "server_entity_wrapper_inline.h"