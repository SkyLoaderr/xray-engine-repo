#ifndef __XR_INI_LTX__
#define __XR_INI_LTX__

class CInifile;
BOOL	__fastcall LoadLtxFile			( CInifile*, char* );
BOOL	__fastcall ReadSectionLtx		( CInifile*, char* );
BOOL	__fastcall ReadStringLtx		( CInifile*, char*, char*, char* );
BOOL	__fastcall ReadLineLtx			( CInifile*, char*, int, char*, char* );

#endif
