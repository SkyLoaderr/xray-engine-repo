#ifndef __XR_TRIMS_H__
#define __XR_TRIMS_H__

// refs
struct xr_token;

int							_GetItemCount			( const char* );
char*						_GetItem				( const char*, int, char*, char* ="" );
const char*		  			_SetPos					( const char* src, DWORD pos );
const char*					_CopyVal				( const char* src, char* dst );
char*						_Trim					( char* str );
char*						_TrimLeft				( char* str );
char*						_TrimRight				( char* str );
char*						_GetFileExt				( char* name );
char*						_ChangeSymbol			( char* name, char src, char dest );
DWORD						_ParseItem				( char* src, xr_token* token_list );
DWORD						_ParseItem				( char* src, int ind, xr_token* token_list );
AnsiString&					ListToSequence			(AStringVec& lst);
AnsiString&					ListToSequence2			(AStringVec& lst);
void 						SequenceToList			(AStringVec& lst, LPCSTR in);

#endif
