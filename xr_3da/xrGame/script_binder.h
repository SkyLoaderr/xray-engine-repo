////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script objects binder
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_script_space.h"

class CScriptBinder : virtual public CGameObject {
protected:
	typedef CGameObject inherited;

protected:
	luabind::object		*m_lua_instance;

public:
						CScriptBinder	();
	virtual				~CScriptBinder	();
			void		init			();
	virtual void		reinit			();
	virtual void		Load			(LPCSTR section);
	virtual void		reload			(LPCSTR section);
	virtual BOOL		net_Spawn		(LPVOID DC);
	virtual void		net_Destroy		();
	virtual void		net_Import		(NET_Packet &net_packet);
	virtual void		net_Export		(NET_Packet &net_packet);
			void		set_lua_object	(const luabind::object &object);
};