////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script.h
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Scripting system with Lua as a scripting language usage
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_script_space.h"

DEFINE_VECTOR(CLuaVirtualMachine*,LUA_VM_VECTOR,LUA_VM_IT);

class CScript {
public:
	CLuaVirtualMachine	*m_tpLuaVirtualMachine;
	LUA_VM_VECTOR		m_tpThreads;

						CScript						(LPCSTR				caFileName);
	virtual				~CScript					();
			void		Update						();
};

class CStreamRedirector {
public:
	FILE				*m_tpOutputLog;
	FILE				*m_tpErrorLog;

	CStreamRedirector()
	{
		string256		l_caLogFileName;
		strconcat		(l_caLogFileName,Core.ApplicationName,"_",Core.UserName,".lualog");
		FS.update_path	(l_caLogFileName,"$logs$",l_caLogFileName);
		m_tpOutputLog	= freopen(l_caLogFileName,"wt",stdout);
		strcat			(l_caLogFileName,"err");
		m_tpErrorLog	= freopen(l_caLogFileName,"wt",stderr);
		R_ASSERT		(m_tpOutputLog && m_tpErrorLog);
	}

	virtual				~CStreamRedirector()
	{
		fclose			(m_tpOutputLog);
		fclose			(m_tpErrorLog);
	}

	void				Flush()
	{
		fflush			(m_tpOutputLog);
		fflush			(m_tpErrorLog);
	}
};

extern CStreamRedirector *g_tpStreamRedirector;

IC CStreamRedirector &getSTDStreams()
{
	if (!g_tpStreamRedirector)
		g_tpStreamRedirector = xr_new<CStreamRedirector>();
	return				(*g_tpStreamRedirector);
};