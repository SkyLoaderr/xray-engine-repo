////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object.h
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

class CSE_ALifeObject;
class CScriptGameObject;

class CScriptBinderObject {
public:
	typedef CSE_ALifeObject*	SpawnType;
	typedef NET_Packet			NetPacket;
	CScriptGameObject				*m_object;

public:
						CScriptBinderObject	(CScriptGameObject *object);
	virtual				~CScriptBinderObject();
	virtual void		reinit				();
	virtual void		reload				(LPCSTR section);
	virtual bool		net_Spawn			(SpawnType DC);
	virtual void		net_Destroy			();
	virtual void		net_Import			(NetPacket *net_packet);
	virtual void		net_Export			(NetPacket *net_packet);
	virtual void		shedule_Update		(u32 time_delta);
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CScriptBinderObject)
#undef script_type_list
#define script_type_list save_type_list(CScriptBinderObject)
