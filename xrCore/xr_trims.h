#ifndef xr_trimsH
#define xr_trimsH

// refs
struct xr_token;

#ifdef __BORLANDC__
	XRCORE_API 	AnsiString&	_Trim					( AnsiString& str );
	XRCORE_API 	LPCSTR		_GetItem				( LPCSTR src, int, AnsiString& p, char separator=',', LPCSTR ="", bool trim=true);
	XRCORE_API 	LPCSTR		_GetItems 				( LPCSTR src, int idx_start, int idx_end, AnsiString& dst, char separator );
	XRCORE_API 	LPCSTR		_CopyVal 				( LPCSTR src, AnsiString& dst, char separator=',' );
	XRCORE_API 	AnsiString	_ListToSequence			( const AStringVec& lst );
	XRCORE_API 	AnsiString	_ListToSequence2		( const AStringVec& lst );
	XRCORE_API 	void 		_SequenceToList			( AStringVec& lst, LPCSTR in, char separator=',' );
	XRCORE_API 	AnsiString&	_ReplaceItem 			( LPCSTR src, int index, LPCSTR new_item, AnsiString& dst, char separator );
	XRCORE_API 	AnsiString&	_ReplaceItems 			( LPCSTR src, int idx_start, int idx_end, LPCSTR new_items, AnsiString& dst, char separator );
	XRCORE_API 	AnsiString 	FloatTimeToStrTime		(float v, bool h=true, bool m=true, bool s=true, bool ms=false);
	XRCORE_API 	float 		StrTimeToFloatTime		(LPCSTR buf, bool h=true, bool m=true, bool s=true, bool ms=false);
#endif

XRCORE_API int		    __stdcall	_GetItemCount			( LPCSTR , char separator=',');
XRCORE_API LPSTR	    __stdcall	_GetItem				( LPCSTR, int, LPSTR, char separator=',', LPCSTR ="", bool trim=true );
XRCORE_API LPSTR	    __stdcall	_GetItems				( LPCSTR, int, int, LPSTR, char separator=',');
XRCORE_API LPCSTR	    __stdcall	_SetPos					( LPCSTR src, u32 pos, char separator=',' );
XRCORE_API LPCSTR	    __stdcall	_CopyVal				( LPCSTR src, LPSTR dst, char separator=',' );
XRCORE_API LPSTR	    __stdcall	_Trim					( LPSTR str );
XRCORE_API LPSTR	    __stdcall	_TrimLeft				( LPSTR str );
XRCORE_API LPSTR	    __stdcall	_TrimRight				( LPSTR str );
XRCORE_API LPSTR	    __stdcall	_ChangeSymbol			( LPSTR name, char src, char dest );
XRCORE_API u32		    __stdcall	_ParseItem				( LPSTR src, xr_token* token_list );
XRCORE_API u32		    __stdcall	_ParseItem				( LPSTR src, int ind, xr_token* token_list );
XRCORE_API LPSTR 	    __stdcall	_ReplaceItem 			( LPCSTR src, int index, LPCSTR new_item, LPSTR dst, char separator );
XRCORE_API LPSTR 	    __stdcall	_ReplaceItems 			( LPCSTR src, int idx_start, int idx_end, LPCSTR new_items, LPSTR dst, char separator );
XRCORE_API void 	    __stdcall	_SequenceToList			( LPSTRVec& lst, LPCSTR in, char separator=',' );
XRCORE_API void 		__stdcall	_SequenceToList			( RStringVec& lst, LPCSTR in, char separator=',' );

XRCORE_API std::string& __stdcall	_Trim					( std::string& src );
XRCORE_API std::string& __stdcall	_TrimLeft				( std::string& src );
XRCORE_API std::string& __stdcall	_TrimRight				( std::string& src );
XRCORE_API LPCSTR		__stdcall 	_CopyVal 				( LPCSTR src, std::string& dst, char separator=',' );
XRCORE_API LPCSTR		__stdcall	_GetItem				( LPCSTR src, int, std::string& p, char separator=',', LPCSTR ="", bool trim=true );
XRCORE_API std::string	__stdcall	_ListToSequence			( const SStringVec& lst );
XRCORE_API shared_str	__stdcall	_ListToSequence			( const RStringVec& lst );

#endif
