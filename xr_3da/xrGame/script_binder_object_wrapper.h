////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object_wrapper.h
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_binder_object.h"
#include "script_game_object.h"

class CScriptBinderObjectWrapper : public CScriptBinderObject, public luabind::wrap_base {
public:
						CScriptBinderObjectWrapper	(CScriptGameObject *object);
	virtual				~CScriptBinderObjectWrapper	();
	virtual void		reinit						();
	static  void		reinit_static				(CScriptBinderObject *script_binder_object);
	virtual void		reload						(LPCSTR section);
	static  void		reload_static				(CScriptBinderObject *script_binder_object, LPCSTR section);
	virtual bool		net_Spawn					(SpawnType DC);
	static  bool		net_Spawn_static			(CScriptBinderObject *script_binder_object, SpawnType DC);
	virtual void		net_Destroy					();
	static  void		net_Destroy_static			(CScriptBinderObject *script_binder_object);
	virtual void		net_Import					(NetPacket *net_packet);
	static  void		net_Import_static			(CScriptBinderObject *script_binder_object, NetPacket *net_packet);
	virtual void		net_Export					(NetPacket *net_packet);
	static  void		net_Export_static			(CScriptBinderObject *script_binder_object, NetPacket *net_packet);
	virtual void		shedule_Update				(u32 time_delta);
	static  void		shedule_Update_static		(CScriptBinderObject *script_binder_object, u32 time_delta);
};