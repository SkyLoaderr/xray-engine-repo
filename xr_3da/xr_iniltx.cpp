#include "stdafx.h"
#include "ffileops.h"
#include "xr_ini.h"
#include "xr_trims.h"
#include "xr_iniltx.h"

BOOL	__fastcall LoadLtxFile			( CInifile* _ini, char* szFileName )
{
}

BOOL	__fastcall ReadSectionLtx		( CInifile *_ini, char* _name )
{
	char	name[256];
	_Trim	(strlwr	(strcpy	(name,_name)));
	vector<_SECTION>::iterator i = _ini->sections.begin();
	for(_ini->cur_section=0;i!=_ini->sections.end();i++,_ini->cur_section++)
		if(stricmp(i->section_name,_name)==0){
			return TRUE;
		}
	_ini->cur_section = -1;
	return FALSE;
}

BOOL	__fastcall ReadLineLtx			( CInifile *_ini, char* _section, int _index, char* _name, char* _value )
{
	_name[0]				= 0;
	_value[0]				= 0;
	char					sect[256];
	_Trim					(strlwr	(strcpy	(sect,_section)));

	if ((_ini->cur_section<0) || stricmp(_ini->sections[_ini->cur_section].section_name,sect))
		if( !ReadSectionLtx(_ini,sect) ) return FALSE;
	if (_index>=_ini->sections[_ini->cur_section].lines.size()) return FALSE;
	strcpy(_name, _ini->sections[_ini->cur_section].lines[_index].line_name);
	strcpy(_value, _ini->sections[_ini->cur_section].lines[_index].line_contents);
	return TRUE;
}

BOOL	__fastcall ReadStringLtx		( CInifile *_ini, char* _section, char* _name, char* _value )
{
	_value[0]				= 0;
	char					sect[256];
	_Trim					(strlwr	(strcpy	(sect,_section)));

	if ((_ini->cur_section<0) || stricmp(_ini->sections[_ini->cur_section].section_name,sect))
		if( !ReadSectionLtx(_ini,sect) ){
			return FALSE;
		}
	vector<_LINE>::iterator i = _ini->sections[_ini->cur_section].lines.begin();
	for(;i!=_ini->sections[_ini->cur_section].lines.end();i++){
		if(stricmp(i->line_name,_name)==0){
			strcpy(_value, i->line_contents);
			return TRUE;
		}
	}
	return FALSE;
}
