#include "stdafx.h"
#pragma hdrstop

#include "xr_trims.h"
#include "xr_tokens.h"

AnsiString& ListToSequence(AStringVec& lst)
{
	static AnsiString out;
    out = "";
    if (lst.size()){
        out			= lst.front();
        for (AStringIt s_it=lst.begin()+1; s_it!=lst.end(); s_it++)
            out		+= AnsiString(",")+(*s_it);
    }
    return out;
}

AnsiString& ListToSequence2(AStringVec& lst)
{
	static AnsiString out;
    out = "";
    if (lst.size()){
        out			= lst.front();
        for (AStringIt s_it=lst.begin()+1; s_it!=lst.end(); s_it++){
            out		+= AnsiString("\n")+(*s_it);
        }
    }
    return out;
}

void SequenceToList(AStringVec& lst, LPCSTR in)
{
	lst.clear();
    int t_cnt=_GetItemCount(in);
    string256 T;
    for (int i=0; i<t_cnt; i++){
        _GetItem(in,i,T,',',0);
        lst.push_back(T);
    }
}

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
	_TrimLeft( str );
	_TrimRight( str );
	return str;
}

char* _GetFileExt ( char* name )
{
	char *point = strchr(name,'.');
	if (point) return point+1;
	return NULL;
}

const char* _SetPos (const char* src, DWORD pos, char separator )
{
	const char*	res			= src;
	DWORD		p			= 0;
	while( p<pos && (res=strchr(res,separator)) )
	{
		res		++;
		p		++;
	}
	return		res;
}

const char* _CopyVal ( const char* src, char* dst, char separator )
{
	const char*	p;
	DWORD		n;
	p			= strchr	( src, separator );
	n			= (p>0) ? (p-src) : strlen(src);
	strncpy		( dst, src, n );
	dst[n]		= 0;
	return		dst;
}

const char* _CopyVal ( const char* src, AnsiString& dst, char separator )
{
	const char*	p;
	DWORD		n;
	p			= strchr	( src, separator );
	n			= (p>0) ? (p-src) : strlen(src);
    dst			= src;
    dst			= dst.Delete(n+1,dst.Length());
	return		dst.c_str();
}

int				_GetItemCount ( const char* src, char separator )
{
	const char*	res			= src;
	const char*	last_res	= res;
	DWORD		cnt			= 0;
	while( res=strchr(res,separator) )
	{
		res		++;
        last_res=res;
		cnt		++;
        if (res[0]==separator) break;
	}
    if (strlen(last_res)) cnt++;
	return		cnt;
}

LPCSTR _GetItem ( const char* src, int index, char* dst, char separator, char* def )
{
	const char*	ptr;
	ptr			= _SetPos	( src, index, separator );
	if( ptr )	_CopyVal	( ptr, dst, separator );
		else	strcpy		( dst, def );
	_Trim( dst );
	return		dst;
}

LPCSTR _GetItem ( const char* src, int index, AnsiString& dst, char separator, char* def )
{
	const char*	ptr;
	ptr			= _SetPos	( src, index, separator );
	if( ptr )	_CopyVal	( ptr, dst, separator );
		else	dst = def;
    dst 		= dst.Trim();
	return		dst.c_str();
}

LPCSTR _GetItems ( LPCSTR src, int idx_start, int idx_end, LPSTR dst, char separator )
{
	char* n = dst;
    int level = 0;
 	for (LPCSTR p=src; *p!=0; p++){
    	if ((level>=idx_start)&&(level<idx_end))
			*n++ = *p;
    	if (*p==separator) level++;
        if (level>=idx_end) break;
    }
    *n++ = '\0';
	return dst;
}

LPCSTR _GetItems ( LPCSTR src, int idx_start, int idx_end, AnsiString& dst, char separator )
{
    int level = 0;
 	for (LPCSTR p=src; *p!=0; p++){
    	if ((level>=idx_start)&&(level<idx_end))
        	dst += *p;
    	if (*p==separator) level++;
        if (level>=idx_end) break;
    }
	return dst.c_str();
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

char* _ReplaceItems( LPCSTR src, int idx_start, int idx_end, LPCSTR new_items, LPSTR dst, char separator ){
	char* n = dst;
    int level = 0;
    bool bCopy = true;
	for (LPCSTR p=src; *p!=0; p++){
    	if ((level>=idx_start)&&(level<idx_end)){
        	if (bCopy){
            	for (LPCSTR itm = new_items; *itm!=0;) *n++ = *itm++;
                bCopy=false;
            }
	    	if (*p==separator) *n++ = separator;
        }else{
			*n++ = *p;
        }
    	if (*p==separator) level++;
    }
    *n++ = '\0';
	return dst;
}

char* _ReplaceItem ( LPCSTR src, int index, LPCSTR new_item, LPSTR dst, char separator ){
	char* n = dst;
    int level = 0;
    bool bCopy = true;
	for (LPCSTR p=src; *p!=0; p++){
    	if (level==index){
        	if (bCopy){
            	for (LPCSTR itm = new_item; *itm!=0;) *n++ = *itm++;
                bCopy=false;
            }
	    	if (*p==separator) *n++ = separator;
        }else{
			*n++ = *p;
        }
    	if (*p==separator) level++;
    }
    *n++ = '\0';
	return dst;
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



