#ifndef xr_trimsH
#define xr_trimsH

// refs
struct xr_token;

#ifdef __BORLANDC__
	AnsiString&				_Trim					( AnsiString& str );
	LPCSTR					_GetItem				( LPCSTR src, int, AnsiString& p, char separator=',', LPCSTR ="" );
	LPCSTR					_GetItems 				( LPCSTR src, int idx_start, int idx_end, AnsiString& dst, char separator );
	LPCSTR					_CopyVal 				( LPCSTR src, AnsiString& dst, char separator=',' );
	AnsiString&				_ListToSequence			( const AStringVec& lst );
	AnsiString&				_ListToSequence2		( const AStringVec& lst );
	void 					_SequenceToList			( AStringVec& lst, LPCSTR in, char separator=',' );
#endif

XRCORE_API int				_GetItemCount			( LPCSTR , char separator=',');
XRCORE_API LPSTR			_GetItem				( LPCSTR, int, LPSTR, char separator=',', LPCSTR ="" );
XRCORE_API LPSTR			_GetItems				( LPCSTR, int, int, LPSTR, char separator=',');
XRCORE_API LPCSTR			_SetPos					( LPCSTR src, u32 pos, char separator=',' );
XRCORE_API LPCSTR			_CopyVal				( LPCSTR src, LPSTR dst, char separator=',' );
XRCORE_API LPSTR			_Trim					( LPSTR str );
XRCORE_API LPSTR			_TrimLeft				( LPSTR str );
XRCORE_API LPSTR			_TrimRight				( LPSTR str );
XRCORE_API LPSTR			_GetFileExt				( LPSTR name );
XRCORE_API LPSTR			_ChangeSymbol			( LPSTR name, char src, char dest );
XRCORE_API u32				_ParseItem				( LPSTR src, xr_token* token_list );
XRCORE_API u32				_ParseItem				( LPSTR src, int ind, xr_token* token_list );
XRCORE_API LPSTR 			_ReplaceItem 			( LPCSTR src, int index, LPCSTR new_item, LPSTR dst, char separator );
XRCORE_API LPSTR 			_ReplaceItems 			( LPCSTR src, int idx_start, int idx_end, LPCSTR new_items, LPSTR dst, char separator );
XRCORE_API void 			_SequenceToList			( LPSTRVec& lst, LPCSTR in, char separator=',' );

#endif
