////////////////////////////////////////////////////////////////////////////
//	Module 		: script_ini_file.cpp
//	Created 	: 21.05.2004
//  Modified 	: 21.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Script ini file class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_ini_file.h"
#include "script_engine.h"
#include "ai_space.h"
#include "object_factory.h"

CScriptIniFile::CScriptIniFile		(IReader *F, LPCSTR path) :
	inherited	(F,path)
{
}

CScriptIniFile::CScriptIniFile		(LPCSTR szFileName, BOOL ReadOnly, BOOL bLoadAtStart, BOOL SaveAtEnd) :
	inherited	(update(szFileName), ReadOnly, bLoadAtStart, SaveAtEnd)
{
}

CScriptIniFile::~CScriptIniFile		()
{
}

LPCSTR	CScriptIniFile::update		(LPCSTR file_name)
{
	string256			S1;
	FS.update_path		(S1,"$game_data$",file_name);
	return				(*shared_str(S1));
}

bool CScriptIniFile::line_exist		(LPCSTR S, LPCSTR L)
{
	return		(!!inherited::line_exist(S,L));
}

bool CScriptIniFile::section_exist	(LPCSTR S)
{
	return		(!!inherited::section_exist(S));
}

int	 CScriptIniFile::r_clsid		(LPCSTR S, LPCSTR L)
{
	return		(object_factory().script_clsid(inherited::r_clsid(S,L)));
}

bool CScriptIniFile::r_bool			(LPCSTR S, LPCSTR L)
{
	return		(!!inherited::r_bool(S,L));
}

int	 CScriptIniFile::r_token		(LPCSTR S, LPCSTR L, const CScriptTokenList &token_list)
{
	return		(inherited::r_token(S,L,&*token_list.tokens().begin()));
}
