////////////////////////////////////////////////////////////////////////////
//	Module 		: script_ini_file.h
//	Created 	: 21.05.2004
//  Modified 	: 21.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Script ini file class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_token_list.h"

class CScriptIniFile : public CInifile {
protected:
	typedef CInifile inherited;

public:
						CScriptIniFile		(IReader *F, LPCSTR path=0);
						CScriptIniFile		(LPCSTR szFileName, BOOL ReadOnly=TRUE, BOOL bLoadAtStart=TRUE, BOOL SaveAtEnd=TRUE);
	virtual 			~CScriptIniFile		();
			bool		line_exist			(LPCSTR S, LPCSTR L);
			bool		section_exist		(LPCSTR S);
			int			r_clsid				(LPCSTR S, LPCSTR L);
			bool		r_bool				(LPCSTR S, LPCSTR L);
			int			r_token				(LPCSTR S, LPCSTR L, const CScriptTokenList &token_list);
			LPCSTR		update				(LPCSTR file_name);
};

#include "script_ini_file_inline.h"