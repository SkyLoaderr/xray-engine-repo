#include "stdafx.h"
#include "xr_ini.h"
#include "xr_inibtx.h"
#include "ffileops.h"

LPXR_BTXFILEHEADER			fHeader				= NULL;
LPXR_BTXSECTIONHEADER		lpCurSection		= NULL;
LPXR_BTXVARIABLEHEADER		lpCurVariable		= NULL;

int		__cdecl _CompareSections	(const void *s1, const void *s2)
{
	LPXR_BTXSECTIONHEADER	sect1 = (LPXR_BTXSECTIONHEADER) s2;
	char*					_name = (char*) s1;
	return					stricmp (_name,sect1->section_name);
}

int		__cdecl _CompareVariables	(const void *s1, const void *s2)
{
	LPXR_BTXVARIABLEHEADER	var1 = (LPXR_BTXVARIABLEHEADER) s2;
	char*					_name = (char*) s1;
	return					stricmp (_name,var1->variable_name);
}

BOOL	__fastcall _CorrectionOfPointers( LPVOID lpData, DWORD datasize )
{
	_CHECK					(lpData);
	_CHECK					(datasize);
	DWORD correction		= (DWORD)lpData;
	fHeader					= (LPXR_BTXFILEHEADER) lpData;
	fHeader->lpSections		= (LPXR_BTXSECTIONHEADER)(correction+8+(DWORD)fHeader->lpSections);
	for (DWORD i=0;i<fHeader->section_count;i++)
	{
		fHeader->lpSections[i].section_name += correction;
		fHeader->lpSections[i].lpVariables  = (LPXR_BTXVARIABLEHEADER)(correction+(DWORD)fHeader->lpSections[i].lpVariables);
		for (DWORD j=0;j<fHeader->lpSections[i].variables_count;j++)
		{
			fHeader->lpSections[i].lpVariables[j].variable_name += correction;
			fHeader->lpSections[i].lpVariables[j].strvalue += correction;
		}
	}
	_OK;
}

BOOL	__fastcall LoadBtxFile			( CInifile* _ini, char* szFileName )
{
	_ini->lpData			= (char*)DownloadFile(szFileName,&(_ini->wDataSize));
	_CorrectionOfPointers	( _ini->lpData, _ini->wDataSize );
	_OK;
}

BOOL	__fastcall ReadSectionBtx		( CInifile *_ini, char* _name )
{
	_name					= strlwr	( _name );
	fHeader					= (LPXR_BTXFILEHEADER)		_ini->lpData;
	lpCurSection			= (LPXR_BTXSECTIONHEADER)	bsearch ( _name,
																  fHeader->lpSections,
																  fHeader->section_count,
																  sizeof(XR_BTXSECTIONHEADER),
																  _CompareSections);
	_CHECK					(lpCurSection);
	_OK;
}

BOOL	__fastcall ReadLineBtx			( CInifile *_ini, char* section, int index, char* name, char* value )
{
	name	[0]				=	0;
	value	[0]				=	0;
	section					= _strlwr( section );

	_CHECK					( ReadSectionBtx(_ini,section) );
	_CHECK					( DWORD(index)<lpCurSection->variables_count );

	strcpy					( name, lpCurSection->lpVariables[index].variable_name);
	strcpy					( value,lpCurSection->lpVariables[index].strvalue);

	_OK;
}

BOOL	__fastcall ReadStringBtx		( CInifile *_ini, char* _section, char* _string, char* value )
{
	value[0]				= 0;

	CHAR					__section[256],
							__string[256],
							*section,
							*string;
	strcpy					( __section, _section );
	strcpy					( __string,  _string );
	section					= strlwr (__section);
	string					= strlwr (__string);

	_CHECK					( ReadSectionBtx(_ini,section) );
	lpCurVariable			= (LPXR_BTXVARIABLEHEADER)	bsearch ( string,
																  lpCurSection->lpVariables,
																  lpCurSection->variables_count,
																  sizeof(XR_BTXVARIABLEHEADER),
																  _CompareVariables);
	_CHECK					( lpCurVariable );
	strcpy					( value, lpCurVariable->strvalue );

	_OK;
}

BOOL	__fastcall ReadTrimStringBtx	( CInifile *_ini, char* section, char* string, char* value )
{
	_CHECK( ReadStringBtx( _ini, section, string, value ) );
	_OK;
}




