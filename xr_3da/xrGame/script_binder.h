////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script objects binder
////////////////////////////////////////////////////////////////////////////

#pragma once

class CScriptBinderObject;

class CScriptBinder {
protected:
	CScriptBinderObject			*m_object;

public:
								CScriptBinder	();
	virtual						~CScriptBinder	();
			void				init			();
	virtual void				reinit			();
	virtual void				Load			(LPCSTR section);
	virtual void				reload			(LPCSTR section);
	virtual BOOL				net_Spawn		(LPVOID DC);
	virtual void				net_Destroy		();
	virtual void				net_Import		(NET_Packet &net_packet);
	virtual void				net_Export		(NET_Packet &net_packet);
			void				set_object		(CScriptBinderObject *object);
	IC		CScriptBinderObject	*object			();
};

#include "script_binder_inline.h"