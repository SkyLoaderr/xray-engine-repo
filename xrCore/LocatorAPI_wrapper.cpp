#include "stdafx.h"
#include "LocatorAPI_wrapper.h"
#include "xr_ini.h"

LPCSTR  CLocatorAPI_wrapper::get_path		(LPCSTR path)
{
	FS_Path* p = FS.get_path(path);
	return p->m_Path;
}



// inifile
CInifile_wrapper::CInifile_wrapper(LPCSTR name)
{
	m_ini = xr_new<CInifile>(name,true);
}

BOOL CInifile_wrapper::section_exist	( LPCSTR S			)
{
	return m_ini->section_exist(S);
}

BOOL CInifile_wrapper::line_exist		( LPCSTR S, LPCSTR L )
{
	return m_ini->line_exist(S,L);
}

LPCSTR CInifile_wrapper::r_string		( LPCSTR S, LPCSTR L)
{
	return *m_ini->r_string_wb(S,L);
}
u32 CInifile_wrapper::line_count		( LPCSTR S			)
{
	return m_ini->line_count(S);
}

BOOL CInifile_wrapper::r_line			( LPCSTR S, int L,	LPCSTR* N, LPCSTR* V )
{
	return m_ini->r_line(S,L,N,V);
}
