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
	virtual void				shedule_Update	(u32 time_delta);
	virtual void				save			(NET_Packet &output_packet);
	virtual void				load			(IReader &input_packet);
			void				set_object		(CScriptBinderObject *object);
	IC		CScriptBinderObject	*object			();
};

#include "script_binder_inline.h"