#include "stdafx.h"
#pragma hdrstop

#include "xr_trims.h"
#include "xr_ini.h"
#include "xr_tokens.h"

#ifndef ENGINE_BUILD
#include "FileSystem.h"
#endif


CInifile *pSettings	= NULL;

CInifile* CInifile::Create(const char* szFileName, BOOL ReadOnly)
{	return new CInifile(szFileName,ReadOnly); }

void CInifile::Destroy(CInifile* ini)
{	_DELETE(ini); }

#ifdef ENGINE_BUILD
#define FATAL(a,b)	Device.Fatal(a,b)
#else
#define FATAL(a,b)	R_ASSERT(0)
#endif

//-----------------------------------------------------------------------------------------------------------
//Тело функций Inifile
//-----------------------------------------------------------------------------------------------------------
void _parse(LPSTR dest, LPCSTR src)
{
	if (src) {
		BOOL bInsideSTR = false;
		while (*src) {
			if (isspace(*src)) {
				if (bInsideSTR) { *dest++ = *src++; continue; }
				while (*src && isspace(*src)) src++;
				continue;
			} else if (*src=='"') {
				bInsideSTR = !bInsideSTR;
			}
			*dest++ = *src++;
		}
	}
	*dest = 0;
}

void _decorate(LPSTR dest, LPCSTR src)
{
	if (src) {
		BOOL bInsideSTR = false;
		while (*src) {
			if (*src == ',') {
				if (bInsideSTR) { *dest++ = *src++; }
				else			{ *dest++ = *src++; *dest++ = ' '; }
				continue;
			} else if (*src=='"') {
				bInsideSTR = !bInsideSTR;
			}
			*dest++ = *src++;
		}
	}
	*dest = 0;
}

CInifile::CInifile( LPCSTR szFileName, BOOL ReadOnly)
{
	fName		= strdup(szFileName);
    bReadOnly	= ReadOnly;

#ifdef ENGINE_BUILD
	if (!Engine.FS.Exist(szFileName)) 
#else
	if (!FS.Exist(szFileName))
#endif
	{
		R_ASSERT(!ReadOnly);
		return;
	}

	CFileStream file(szFileName);

	Sect	Current;	Current.Name = 0;
	char	str			[1024];
	char	str2		[1024];

	while (!file.Eof())
	{
		file.Rstring	(str);
		_Trim			(str);
		LPSTR semi		= strchr(str,';');
		LPSTR comment	= 0;
		if (semi) {
			*semi		= 0;
			comment		= strdup(semi+1);
		}

		if (str[0] && (str[0]=='['))
		{
			_FREE(comment);
			if (Current.Name && Current.Name[0]) 
			{
				RootIt I		= lower_bound(DATA.begin(),DATA.end(),Current,sect_pred());
				DATA.insert		(I,Current);
				Current.clear	();
			}
			int L = strlen(str); str[L-1] = 0;
			Current.Name = strlwr(strdup(str+1));
		} else {
			if (0==Current.Name)	{
				_FREE(comment);
			} else {
				char*		name	= str;
				char*		t		= strchr(name,'=');
				if (t)		{
					*t		= 0;
					_Trim	(name);
					_parse	(str2,++t);
				} else {
					str2[0]	= 0;
				}
				
				Item		I;
				I.first		= (name[0]?strdup(name):NULL);
				I.second	= (str2[0]?strdup(str2):NULL);
				I.comment	= comment;
				
				if (bReadOnly) {
					if (I.first) {
						_FREE(I.comment);
						SectIt	it	= lower_bound(Current.begin(),Current.end(),I,item_pred());
						Current.Data.insert(it,I);
					} else {
						_FREE(I.second);
						_FREE(I.comment);
					}
				} else {
					if (I.first || I.second || I.comment) {
						SectIt	it	= lower_bound(Current.begin(),Current.end(),I,item_pred());
						Current.Data.insert(it,I);
					}
				}
			}
		}
	}
	if (Current.Name && Current.Name[0]) 
	{
		RootIt I		= lower_bound(DATA.begin(),DATA.end(),Current,sect_pred());
		DATA.insert		(I,Current);
		Current.clear	();
	}
}

CInifile::~CInifile( )
{
	// save if needed
	if (!bReadOnly)
	{
        CFS_Memory	F;
		char		temp[512],val[512];
        for (RootIt r_it=DATA.begin(); r_it!=DATA.end(); r_it++)
		{
        	sprintf		(temp,"[%s]",r_it->Name);
            F.Wstring	(temp);
            for (SectIt s_it=r_it->begin(); s_it!=r_it->end(); s_it++)
			{
				Item&	I = *s_it;
				if (I.first) {
					if (I.second) {
						_decorate	(val,I.second);
						if (I.comment) {
							// name, value and comment
							sprintf	(temp,"%-24s = %-32s ;%s",I.first,val,I.comment);
						} else {
							// only name and value
							sprintf	(temp,"%-24s = %-32s",I.first,val);
						}
					} else {
						if (I.comment) {
							// name and comment
							sprintf(temp,"%-24s   ;%s",I.first,I.comment);
						} else {
							// only name
							sprintf(temp,"%-24s",I.first);
						}
					}
				} else {
					// no name, so no value
					if (I.comment)	sprintf		(temp,";%s",I.comment);
					else			temp[0] = 0;
				}
				_TrimRight			(temp);
                if (temp[0])		F.Wstring	(temp);
            }
            F.Wstring		(" ");
        }
        F.SaveTo			(fName,0);
    }

	// release memory
	_FREE(fName);

	for (RootIt S = DATA.begin(); S!=DATA.end(); S++)
	{
		_FREE(S->Name);
		for (SectIt I=S->begin(); I!=S->end(); I++)
		{
			_FREE(I->first	);
			_FREE(I->second	);
			_FREE(I->comment);
		}
	}
}

BOOL	CInifile::SectionExists( LPCSTR S )
{
	Sect Test; Test.Name = (char*)S; RootIt I = lower_bound(DATA.begin(),DATA.end(),Test,sect_pred());
	return (I!=DATA.end() && strcmp(I->Name,S)==0);
}

BOOL	CInifile::LineExists( LPCSTR S, LPCSTR L )
{
	if (!SectionExists(S)) return FALSE;
	Sect&	I = ReadSection(S);
	Item Test; Test.first=(char*)L; SectIt A = lower_bound(I.begin(),I.end(),Test,item_pred());
	return (A!=I.end() && strcmp(A->first,L)==0);
}
 
DWORD	CInifile::LineCount	(LPCSTR Sname)
{
	Sect&	S = ReadSection(Sname);
	SectIt	I = S.begin();
	DWORD	C = 0;
	for (; I!=S.end(); I++)	if (I->first) C++;
	return  C;
}

CInifile::Sect& CInifile::ReadSection( LPCSTR S )
{
	char	section[256]; strcpy(section,S); strlwr(section);
	Sect Test; Test.Name = section; RootIt I = lower_bound(DATA.begin(),DATA.end(),Test,sect_pred());
	if (I!=DATA.end() && strcmp(I->Name,section)==0)	return *I;
	else												FATAL("Can't open section '%s'",S);
}

LPCSTR	CInifile::ReadSTRING(LPCSTR S, LPCSTR L )
{
	Sect&	I = ReadSection(S);
	Item Test; Test.first=(char*)L; SectIt	A = lower_bound(I.begin(),I.end(),Test,item_pred());
	if (A!=I.end() && strcmp(A->first,L)==0)	return A->second;
	else										FATAL("Can't find variable '%s'",L);
}

int  CInifile::ReadINT(LPCSTR S, LPCSTR L)
{
	LPCSTR		C = ReadSTRING(S,L);
	return		atoi( C );
}

float CInifile::ReadFLOAT(LPCSTR S, LPCSTR L)
{
	LPCSTR		C = ReadSTRING(S,L);
	return		float(atof( C ));
}

DWORD CInifile::ReadCOLOR( LPCSTR S, LPCSTR L )
{
	LPCSTR		C = ReadSTRING(S,L);
	DWORD		r=0,g=0,b=0,a=255;
	sscanf		(C,"%d,%d,%d,%d",&r,&g,&b,&a);
	return D3DCOLOR_RGBA(r,g,b,a);
}

Fvector CInifile::ReadVECTOR( LPCSTR S, LPCSTR L )
{
	LPCSTR		C = ReadSTRING(S,L);
	Fvector		V; V.set(0,0,0);
	sscanf		(C,"%f,%f,%f",&V.x,&V.y,&V.z);
	return V;
}
Fvector2 CInifile::ReadVECTOR2( LPCSTR S, LPCSTR L )
{
	LPCSTR		C = ReadSTRING(S,L);
	Fvector2	V; V.set(0.f,0.f);
	sscanf		(C,"%f,%f",&V.x,&V.y);
	return V;
}
BOOL	CInifile::ReadBOOL( LPCSTR S, LPCSTR L )
{
	LPCSTR		C = ReadSTRING(S,L);
	char		B[8];
	strncpy		(B,C,7);
	strlwr		(B);
	if (strcmp(B,"on")==0 || strcmp(B,"yes")==0 || strcmp(B,"true")==0 || strcmp(B,"1")==0) return TRUE;
	else return FALSE;
}
CLASS_ID CInifile::ReadCLSID( LPCSTR S, LPCSTR L)
{
	LPCSTR		C = ReadSTRING(S,L);
	return		TEXT2CLSID(C);
}
int		CInifile::ReadTOKEN	( LPCSTR S, LPCSTR L, const xr_token *token_list)
{
	LPCSTR		C = ReadSTRING(S,L);
	for( int i=0; token_list[i].name; i++ )
		if( !stricmp(C,token_list[i].name) )
			return token_list[i].id;
	return 0;
}
BOOL	CInifile::ReadLINE( LPCSTR S, int L, const char** N, const char** V )
{
	Sect&	SS = ReadSection(S);
	if (L>=SS.size() || L<0 ) return FALSE;
	for (SectIt I=SS.begin(); I!=SS.end(); I++)
		if (!(L--)){ 
			*N = I->first;
			*V = I->second;
			return TRUE;
		}
	return FALSE;
}
//--------------------------------------------------------------------------------------------------------
void	CInifile::WriteString	( LPCSTR S, LPCSTR L, LPCSTR			V, LPCSTR comment)
{
	R_ASSERT	(!bReadOnly);

	// section
	char	sect	[256];
	_parse	(sect,S);
	_strlwr	(sect);
	if (!SectionExists(sect))	{
		// create new section
		Sect			NEW;
		NEW.Name		= strdup(sect);
		RootIt I		= lower_bound(DATA.begin(),DATA.end(),NEW,sect_pred());
		DATA.insert		(I,NEW);
	}

	// parse line/value
	char	line	[256];	_parse	(line,L);
	char	value	[256];	_parse	(value,V);
	
	// duplicate & insert
	Item	I;
	Sect&	data	= ReadSection	(sect);
	I.first			= (line[0]?strdup(line):0);
	I.second		= (value[0]?strdup(value):0);
	I.comment		= (comment?strdup(comment):0);
	SectIt	it		= lower_bound(data.begin(),data.end(),I,item_pred());
	data.Data.insert(it,I);
}

void	CInifile::WriteFloat	( LPCSTR S, LPCSTR L, float				V, LPCSTR comment )
{
	char temp[128];	sprintf		(temp,"%.3f",V);
	WriteString	(S,L,temp,comment);
}
void	CInifile::WriteInteger	( LPCSTR S, LPCSTR L, int				V, LPCSTR comment )
{
	char temp[128];	sprintf		(temp,"%d",V);
	WriteString	(S,L,temp,comment);
}
void	CInifile::WriteColor	( LPCSTR S, LPCSTR L, const Fcolor&		V, LPCSTR comment )
{
	char temp[128];	sprintf		(temp,"%.3f,%.3f,%.3f", V.r, V.g, V.b);
	WriteString	(S,L,temp,comment);
}

#ifndef RGBA_GETALPHA
#define RGBA_GETALPHA(rgb)      DWORD((rgb) >> 24)
#define RGBA_GETRED(rgb)        DWORD(((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      DWORD(((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       DWORD((rgb) & 0xff)
#endif
void	CInifile::WriteColor	( LPCSTR S, LPCSTR L, DWORD				V, LPCSTR comment )
{
	char temp[128];	sprintf		(temp,"%d,%d,%d", RGBA_GETRED(V), RGBA_GETGREEN(V), RGBA_GETBLUE(V));
	WriteString	(S,L,temp,comment);
}

void	CInifile::WriteVector	( LPCSTR S, LPCSTR L, const Fvector&	V, LPCSTR comment )
{
	char temp[128];	sprintf		(temp,"%.3f,%.3f,%.3f", V.x, V.y, V.z);
	WriteString	(S,L,temp,comment);
}
