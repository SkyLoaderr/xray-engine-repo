#ifndef __XR_TRIMS_H__
#define __XR_TRIMS_H__

// refs
struct xr_token;

ENGINE_API int		_GetItemCount			( LPCSTR src);
ENGINE_API LPSTR	_GetItem				( LPCSTR src, int itm, LPSTR dest, LPSTR def="" );
ENGINE_API LPCSTR	_SetPos					( LPCSTR src, DWORD pos );
ENGINE_API LPSTR	_CopyVal				( LPCSTR src, LPSTR dst );
ENGINE_API LPSTR	_Trim					( LPSTR str );
ENGINE_API LPSTR	_TrimLeft				( LPSTR str );
ENGINE_API LPSTR	_TrimRight				( LPSTR str );
ENGINE_API LPSTR	_GetFileExt				( LPSTR name );
ENGINE_API LPSTR	_ChangeSymbol			( LPSTR name, char src, char dest );
ENGINE_API DWORD	_ParseItem				( LPSTR src, xr_token* token_list );
ENGINE_API DWORD	_ParseItem				( LPSTR src, int ind, xr_token* token_list );

#endif
