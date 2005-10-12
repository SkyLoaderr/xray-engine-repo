#pragma once

class XRCORE_API CLocatorAPI_wrapper  
{
public:
	LPCSTR  get_path		(LPCSTR path);
};

class CInifile;
class XRCORE_API CInifile_wrapper
{
	CInifile* m_ini;
public:
	CInifile_wrapper(LPCSTR name);

	BOOL		section_exist	( LPCSTR S			);
	BOOL		line_exist		( LPCSTR S, LPCSTR L );
	LPCSTR 		r_string		( LPCSTR S, LPCSTR L);															// оставляет кавычки
	u32			line_count		( LPCSTR S			);
	BOOL		r_line			( LPCSTR S, int L,	LPCSTR* N, LPCSTR* V );

};