////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object.h
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder
////////////////////////////////////////////////////////////////////////////

#pragma once

class CScriptBinderObject {
public:
	typedef CSE_Abstract*	SpawnType;
	typedef NET_Packet		NetPacket;
//	typedef int				SpawnType;
//	typedef int				NetPacket;
	CLuaGameObject			*m_object;

public:
						CScriptBinderObject	(CLuaGameObject *object);
	virtual				~CScriptBinderObject();
	virtual void		reinit				();
	virtual void		reload				(LPCSTR section);
	virtual bool		net_Spawn			(SpawnType DC);
	virtual void		net_Destroy			();
	virtual void		net_Import			(NetPacket *net_packet);
	virtual void		net_Export			(NetPacket *net_packet);
};