#ifndef __XR_TRIMS_H__
#define __XR_TRIMS_H__

// refs
struct xr_token;

#ifdef M_BORLAND
	LPCSTR					_GetItem				( LPCSTR src, int, AnsiString& p, char separator=',', LPCSTR ="" );
	LPCSTR					_GetItems 				( LPCSTR src, int idx_start, int idx_end, AnsiString& dst, char separator );
	LPCSTR					_CopyVal 				( LPCSTR src, AnsiString& dst, char separator=',' );
	AnsiString&				ListToSequence			( AStringVec& lst );
	AnsiString&				ListToSequence2			( AStringVec& lst );
	void 					SequenceToList			( AStringVec& lst, LPCSTR in );
#endif

ENGINE_API int				_GetItemCount			( LPCSTR , char separator=',');
ENGINE_API LPCSTR			_GetItem				( LPCSTR, int, LPSTR, char separator=',', LPCSTR ="" );
ENGINE_API LPCSTR			_GetItems				( LPCSTR, int, int, LPSTR, char separator=',');
ENGINE_API LPCSTR			_SetPos					( LPCSTR src, u32 pos, char separator=',' );
ENGINE_API LPCSTR			_CopyVal				( LPCSTR src, LPSTR dst, char separator=',' );
ENGINE_API LPSTR			_Trim					( LPSTR str );
ENGINE_API LPSTR			_TrimLeft				( LPSTR str );
ENGINE_API LPSTR			_TrimRight				( LPSTR str );
ENGINE_API LPSTR			_GetFileExt				( LPSTR name );
ENGINE_API LPSTR			_ChangeSymbol			( LPSTR name, char src, char dest );
ENGINE_API u32				_ParseItem				( LPSTR src, xr_token* token_list );
ENGINE_API u32				_ParseItem				( LPSTR src, int ind, xr_token* token_list );
ENGINE_API LPSTR 			_ReplaceItem 			( LPCSTR src, int index, LPCSTR new_item, LPSTR dst, char separator );
ENGINE_API LPSTR 			_ReplaceItems 			( LPCSTR src, int idx_start, int idx_end, LPCSTR new_items, LPSTR dst, char separator );

#endif
