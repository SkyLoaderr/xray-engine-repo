////////////////////////////////////////////////////////////////////////////
//	Module 		: script_thread.h
//	Created 	: 19.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script thread class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_stack_tracker.h"

class CScriptThread : public CScriptStackTracker {
public:
	LPSTR			m_script_name;
	int				m_thread_reference;
	bool			m_bActive;

public:
					CScriptThread		(LPCSTR caNamespaceName);
	virtual			~CScriptThread		();
			bool	Update				();
};