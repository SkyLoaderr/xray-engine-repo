#include "stdafx.h"
#include "xr_trims.h"
#include "xr_tokens.h"

char* _TrimRight( char* str )
{
	DWORD	num = strlen( str ) - 1;
	while ( (num > 0)&&(isspace(BYTE(str[num]))))
	{
		num--;
	}
	str[num+1] = 0;
	return str;
}

char* _Trim( char* str )
{
	_TrimLeft	( str );
	_TrimRight	( str );
	return str;
}

char* _GetFileExt ( char* name )
{
	char *point = strchr(name,'.');
	if (point) return point+1;
	return NULL;
}

const char* _SetPos (const char* src, DWORD pos )
{
	const char*	res			= src;
	DWORD		p			= 0;
	while( p<pos && (res=strchr(res,',')) )
	{
		res		++;
		p		++;
	}
	return		res;
}

char* _CopyVal ( const char* src, char* dst )
{
	char*		p;
	DWORD		n;
	p			= strchr	( src, ',' );
	n			= (p>0) ? (p-src) : strlen(src);
	strncpy		( dst, src, n );
	dst[n]		= 0;
	return		dst;
}

int				_GetItemCount ( const char* src )
{
	const char*	res			= src;
	DWORD		p			= 0;
	while( res=strchr(res,',') )
	{
		res		++;
		p		++;
	}
	return		++p;
}

char* _GetItem ( const char* src, int index, char* dst, char* def )
{
	const char*	ptr;
	ptr			= _SetPos	( src, index );
	if( ptr )	_CopyVal	( ptr, dst );
		else	strcpy		( dst, def );
	_Trim( dst );
	return		dst;
}

DWORD _ParseItem ( char* src, xr_token* token_list )
{
	for( int i=0; token_list[i].name; i++ )
		if( !stricmp(src,token_list[i].name) )
			return token_list[i].id;
	return -1;
}

DWORD _ParseItem ( char* src, int ind, xr_token* token_list )
{
	char dst[128];
	_GetItem(src, ind, dst);
	return _ParseItem(dst, token_list);
}

char* _ChangeSymbol ( char* name, char src, char dest )
{
    char						*sTmpName = name;
    while(sTmpName[0] ){
		if (sTmpName[0] == src) sTmpName[0] = dest;
		sTmpName ++;
	}
	return						name;
}

char* _TrimLeft( char* str )
{
	char* p = str;
	while( *p && isspace(*p) ) p++;
	DWORD	num1 = strlen( str );
	DWORD	num2 = strlen( p );
	if (num1 == num2) return str;
	for (DWORD	i = 0; i < num1; i++)
	{
		if (i < num2) str[i] = p[i];
			else str[i] = 0;
	}
	return str;
}




