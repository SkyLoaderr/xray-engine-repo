#ifndef __XR_INI_BTX__
#define __XR_INI_BTX__

#pragma pack(4)

typedef struct	XR_BTXVARIABLEHEADER{
	char*						variable_name;
	char*						strvalue;
} XR_BTXVARIABLEHEADER, *LPXR_BTXVARIABLEHEADER;

typedef struct	XR_BTXSECTIONHEADER	{
	char*						section_name;
	LPXR_BTXVARIABLEHEADER		lpVariables;
	DWORD						variables_count;
} XR_BTXSECTIONHEADER, *LPXR_BTXSECTIONHEADER;

typedef struct	XR_BTXFILEHEADER	{
	DWORD						section_count;
	LPXR_BTXSECTIONHEADER		lpSections;
} XR_BTXFILEHEADER, *LPXR_BTXFILEHEADER;

#pragma pack()


BOOL	__fastcall LoadBtxFile			( CInifile*, char* );
BOOL	__fastcall ReadSectionBtx		( CInifile*, char* );
BOOL	__fastcall ReadStringBtx		( CInifile*, char*, char*, char* );
BOOL	__fastcall ReadTrimStringBtx	( CInifile*, char*, char*, char* );
BOOL	__fastcall ReadLineBtx			( CInifile*, char*, int, char*, char* );

#endif
