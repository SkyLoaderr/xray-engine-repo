////////////////////////////////////////////////////////////////////////////
//	Module 		: script_thread.h
//	Created 	: 19.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script thread class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CScriptThread {
private:
	lua_State			*m_virtual_machine;
	shared_str			m_script_name;
	int					m_thread_reference;
	bool				m_active;

public:
						CScriptThread		(LPCSTR caNamespaceName);
	virtual				~CScriptThread		();
			bool		Update				();
	IC		lua_State	*lua				() const;
	IC		bool		active				() const;
	IC		shared_str	script_name			() const;
	IC		int			thread_reference	() const;
};

#include "script_thread_inline.h"