//////////////////////////////////////////////////////////////////////////////////
//	Module			:	Move routines											//
//	File			:	misc.cpp												//
//	Creation date	:	14.02.2001												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "misc.h"

FILE *fVOOutput;

void __cdecl vfDualPrintF(const char *caFormat, ... )
{
	va_list marker;

	va_start(marker,caFormat);
   
	vfprintf(stdout,caFormat,marker);
   
#ifdef WRITE_LOG
	vfprintf(fVOOutput,caFormat,marker);
   
	fflush(fVOOutput);
#endif

	va_end( marker );              
}

bool bfFileExists(char *caFileName)
{
	LPWIN32_FIND_DATA lpFindFileData = new WIN32_FIND_DATA;
	return(FindFirstFile(caFileName,lpFindFileData)!=INVALID_HANDLE_VALUE);
}
