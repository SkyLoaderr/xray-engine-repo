#include "stdafx.h"
#pragma hdrstop

#include "fs_internal.h"

XRCORE_API CInifile *pSettings	= NULL;

CInifile* CInifile::Create(const char* szFileName, BOOL ReadOnly)
{	return xr_new<CInifile>(szFileName,ReadOnly); }

void CInifile::Destroy(CInifile* ini)
{	xr_delete(ini); }

bool sect_pred(const CInifile::Sect& x, LPCSTR val)
{
	return xr_strcmp(*x.Name,val)<0;
};

bool item_pred(const CInifile::Item& x, LPCSTR val)
{
    if ((!x.first) || (!val))	return x.first<val;
    else				   		return xr_strcmp(*x.first,val)<0;
}

//------------------------------------------------------------------------------
//Тело функций Inifile
//------------------------------------------------------------------------------
XRCORE_API void _parse(LPSTR dest, LPCSTR src)
{
	if (src) {
		BOOL bInsideSTR = false;
		while (*src) {
			if (isspace((u8)*src)) {
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

XRCORE_API void _decorate(LPSTR dest, LPCSTR src)
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
//------------------------------------------------------------------------------

BOOL	CInifile::Sect::line_exist( LPCSTR L, LPCSTR* val )
{
	SectIt A = std::lower_bound(Data.begin(),Data.end(),L,item_pred);
    if (A!=Data.end() && xr_strcmp(*A->first,L)==0){
    	if (val) *val = *A->second;
    	return TRUE;
    }
	return FALSE;
}
//------------------------------------------------------------------------------

CInifile::CInifile(IReader* F ,LPCSTR path)
{
	fName		= 0;
	bReadOnly	= TRUE;
	bSaveAtEnd	= FALSE;
	Load		(F,path);
}

CInifile::CInifile(LPCSTR szFileName, BOOL ReadOnly, BOOL bLoad, BOOL SaveAtEnd)
{
	fName		= szFileName?xr_strdup(szFileName):0;
    bReadOnly	= ReadOnly;
    bSaveAtEnd	= SaveAtEnd;
	if (bLoad)
	{	
    	string256 	path,folder; 
		_splitpath	(fName, path, folder, 0, 0 );
        strcat		(path,folder);
		IReader* R 	= FS.r_open(szFileName);
        if (R){
			Load		(R,path);
			FS.r_close	(R);
        }
	}
}

CInifile::~CInifile( )
{
 	if (!bReadOnly&&bSaveAtEnd) save_as();
	xr_free	(fName);
}

void	CInifile::Load(IReader* F, LPCSTR path)
{
	R_ASSERT(F);
	Sect	Current;	Current.Name = 0;
	char	str			[4096];
	char	str2		[4096];

	while (!F->eof())
	{
		F->r_string		(str);
		_Trim			(str);
		LPSTR semi		= strchr(str,';');
		LPSTR comment	= 0;
		if (semi) {
			*semi		= 0;
			comment		= semi+1;
		}

        if (str[0] && (str[0]=='#') && strstr(str,"#include")){
        	string64	inc_name;	
			R_ASSERT	(path&&path[0]);
        	if (_GetItem	(str,1,inc_name,'"')){
            	string256 	fn;
                strconcat	(fn,path,inc_name);
            	IReader* I 	= FS.r_open(fn); R_ASSERT3(I,"Can't find include file:",inc_name);
            	Load		(I,path);
                FS.r_close	(I);
            }
        }else if (str[0] && (str[0]=='[')){
			if (*Current.Name && Current.Name[0])
			{
				RootIt I		= std::lower_bound(DATA.begin(),DATA.end(),*Current.Name,sect_pred);
				if ((I!=DATA.end())&&(I->Name==Current.Name)) Debug.fatal("Duplicate section '%s' found.",*Current.Name);
				DATA.insert		(I,Current);
				Current.clear	();
			}
//#pragma todo("find real section-name-end ']'")
//			size_t L = xr_strlen(str); str[L-1] = 0;
			R_ASSERT3(strchr(str,']'),"Bad ini section found: ",str);
			*strchr(str,']') 	= 0;
			Current.Name 		= strlwr(str+1);
		} else {
			if (*Current.Name)	{
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
				I.first		= (name[0]?name:NULL);
				I.second	= (str2[0]?str2:NULL);
				I.comment	= bReadOnly?0:comment;

				if (bReadOnly) {
					if (*I.first) {
						SectIt	it	= std::lower_bound(Current.begin(),Current.end(),*I.first,item_pred);
						Current.Data.insert(it,I);
					}
				} else {
					if (*I.first || *I.second || *I.comment) {
						SectIt	it	= std::lower_bound(Current.begin(),Current.end(),*I.first,item_pred);
						Current.Data.insert(it,I);
					}
				}
			}
		}
	}
	if (*Current.Name && Current.Name[0])
	{
		RootIt I		= std::lower_bound(DATA.begin(),DATA.end(),*Current.Name,sect_pred);
		DATA.insert		(I,Current);
		Current.clear	();
	}
}

void	CInifile::save_as( LPCSTR new_fname )
{
	// save if needed
    if (new_fname&&new_fname[0])
	{
        xr_free(fName);
        fName		= xr_strdup(new_fname);
    }
    R_ASSERT		(fName&&fName[0]);
    IWriter* F		= FS.w_open(fName);
    char			temp[512],val[512];
    for (RootIt r_it=DATA.begin(); r_it!=DATA.end(); r_it++)
    {
        sprintf		(temp,"[%s]",*r_it->Name);
        F->w_string	(temp);
        for (SectIt s_it=r_it->begin(); s_it!=r_it->end(); s_it++)
        {
            Item&	I = *s_it;
            if (*I.first) {
                if (*I.second) {
                    _decorate	(val,*I.second);
                    if (*I.comment) {
                        // name, value and comment
                        sprintf	(temp,"%8s%-32s = %-32s ;%s"," ",*I.first,val,*I.comment);
                    } else {
                        // only name and value
                        sprintf	(temp,"%8s%-32s = %-32s"," ",*I.first,val);
                    }
                } else {
                    if (*I.comment) {
                        // name and comment
                        sprintf(temp,"%8s%-32s = ;%s"," ",*I.first,*I.comment);
                    } else {
                        // only name
                        sprintf(temp,"%8s%-32s = "," ",*I.first);
                    }
                }
            } else {
                // no name, so no value
                if (*I.comment)	sprintf		(temp,"%8s;%s"," ",*I.comment);
                else			temp[0] = 0;
            }
            _TrimRight			(temp);
            if (temp[0])		F->w_string	(temp);
        }
        F->w_string		(" ");
    }
    FS.w_close			(F);
}

BOOL	CInifile::section_exist( LPCSTR S )
{
	RootIt I = std::lower_bound(DATA.begin(),DATA.end(),S,sect_pred);
	return (I!=DATA.end() && xr_strcmp(*I->Name,S)==0);
}

BOOL	CInifile::line_exist( LPCSTR S, LPCSTR L )
{
	if (!section_exist(S)) return FALSE;
	Sect&	I = r_section(S);
	SectIt A = std::lower_bound(I.begin(),I.end(),L,item_pred);
	return (A!=I.end() && xr_strcmp(*A->first,L)==0);
}

u32		CInifile::line_count(LPCSTR Sname)
{
	Sect&	S = r_section(Sname);
	SectIt	I = S.begin();
	u32	C = 0;
	for (; I!=S.end(); I++)	if (*I->first) C++;
	return  C;
}


//--------------------------------------------------------------------------------------
CInifile::Sect&	CInifile::r_section		( const shared_str& S	)					{ return	r_section(*S);		}
BOOL			CInifile::line_exist	( const shared_str& S, const shared_str& L )	{ return	line_exist(*S,*L);	}
u32				CInifile::line_count	( const shared_str& S	)					{ return	line_count(*S);		}
BOOL			CInifile::section_exist	( const shared_str& S	)					{ return	section_exist(*S);	}

//--------------------------------------------------------------------------------------
// Read functions
//--------------------------------------------------------------------------------------
CInifile::Sect& CInifile::r_section( LPCSTR S )
{
	char	section[256]; strcpy(section,S); strlwr(section);
	RootIt I = std::lower_bound(DATA.begin(),DATA.end(),section,sect_pred);
	if (!(I!=DATA.end() && xr_strcmp(*I->Name,section)==0))	Debug.fatal("Can't open section '%s'",S);
	return	*I;
}

LPCSTR	CInifile::r_string(LPCSTR S, LPCSTR L)
{
	Sect&	I = r_section(S);
	SectIt	A = std::lower_bound(I.begin(),I.end(),L,item_pred);
	if (A!=I.end() && xr_strcmp(*A->first,L)==0)	return *A->second;
	else										Debug.fatal("Can't find variable %s in [%s]",L,S);
	return 0;
}

shared_str		CInifile::r_string_wb(LPCSTR S, LPCSTR L)	{
	LPCSTR		_base		= r_string(S,L);
	if	(0==_base)					return	shared_str(0);
	string512	_original;	
	u32			_len		= xr_strlen(strcpy(_original,_base));
	if	(0==_len)					return	shared_str("");
	if	('"'==_original[_len-1])	_original[_len-1]=0;				// skip end
	if	('"'==_original[0])			return	shared_str(&_original[0] + 1);	// skip begin
	return									shared_str(_original);
}

u8 CInifile::r_u8(LPCSTR S, LPCSTR L)
{
	LPCSTR		C = r_string(S,L);
	return		u8(atoi(C));
}
u16 CInifile::r_u16(LPCSTR S, LPCSTR L)
{
	LPCSTR		C = r_string(S,L);
	return		u16(atoi(C));
}
u32 CInifile::r_u32(LPCSTR S, LPCSTR L)
{
	LPCSTR		C = r_string(S,L);
	return		u32(atoi(C));
}
s8 CInifile::r_s8(LPCSTR S, LPCSTR L)
{
	LPCSTR		C = r_string(S,L);
	return		s8(atoi(C));
}
s16 CInifile::r_s16(LPCSTR S, LPCSTR L)
{
	LPCSTR		C = r_string(S,L);
	return		s16(atoi(C));
}
s32 CInifile::r_s32(LPCSTR S, LPCSTR L)
{
	LPCSTR		C = r_string(S,L);
	return		s32(atoi(C));
}
float CInifile::r_float(LPCSTR S, LPCSTR L)
{
	LPCSTR		C = r_string(S,L);
	return		float(atof( C ));
}
Fcolor CInifile::r_fcolor( LPCSTR S, LPCSTR L )
{
	LPCSTR		C = r_string(S,L);
	Fcolor		V={0,0,0,0};
	sscanf		(C,"%f,%f,%f,%f",&V.r,&V.g,&V.b,&V.a);
	return V;
}
u32 CInifile::r_color( LPCSTR S, LPCSTR L )
{
	LPCSTR		C = r_string(S,L);
	u32			r=0,g=0,b=0,a=255;
	sscanf		(C,"%d,%d,%d,%d",&r,&g,&b,&a);
	return color_rgba(r,g,b,a);
}

Ivector2 CInifile::r_ivector2( LPCSTR S, LPCSTR L )
{
	LPCSTR		C = r_string(S,L);
	Ivector2	V={0,0};
	sscanf		(C,"%d,%d",&V.x,&V.y);
	return V;
}
Ivector3 CInifile::r_ivector3( LPCSTR S, LPCSTR L )
{
	LPCSTR		C = r_string(S,L);
	Ivector		V={0,0,0};
	sscanf		(C,"%d,%d,%d",&V.x,&V.y,&V.z);
	return V;
}
Ivector4 CInifile::r_ivector4( LPCSTR S, LPCSTR L )
{
	LPCSTR		C = r_string(S,L);
	Ivector4	V={0,0,0,0};
	sscanf		(C,"%d,%d,%d,%d",&V.x,&V.y,&V.z,&V.w);
	return V;
}
Fvector2 CInifile::r_fvector2( LPCSTR S, LPCSTR L )
{
	LPCSTR		C = r_string(S,L);
	Fvector2	V={0.f,0.f};
	sscanf		(C,"%f,%f",&V.x,&V.y);
	return V;
}
Fvector3 CInifile::r_fvector3( LPCSTR S, LPCSTR L )
{
	LPCSTR		C = r_string(S,L);
	Fvector3	V={0.f,0.f,0.f};
	sscanf		(C,"%f,%f,%f",&V.x,&V.y,&V.z);
	return V;
}
Fvector4 CInifile::r_fvector4( LPCSTR S, LPCSTR L )
{
	LPCSTR		C = r_string(S,L);
	Fvector4	V={0.f,0.f,0.f,0.f};
	sscanf		(C,"%f,%f,%f,%f",&V.x,&V.y,&V.z,&V.w);
	return V;
}
BOOL	CInifile::r_bool( LPCSTR S, LPCSTR L )
{
	LPCSTR		C = r_string(S,L);
	char		B[8];
	strncpy		(B,C,7);
	strlwr		(B);
    return 		IsBOOL(B);
}
CLASS_ID CInifile::r_clsid( LPCSTR S, LPCSTR L)
{
	LPCSTR		C = r_string(S,L);
	return		TEXT2CLSID(C);
}
int		CInifile::r_token	( LPCSTR S, LPCSTR L, const xr_token *token_list)
{
	LPCSTR		C = r_string(S,L);
	for( int i=0; token_list[i].name; i++ )
		if( !stricmp(C,token_list[i].name) )
			return token_list[i].id;
	return 0;
}
BOOL	CInifile::r_line( LPCSTR S, int L, const char** N, const char** V )
{
	Sect&	SS = r_section(S);
	if (L>=(int)SS.size() || L<0 ) return FALSE;
	for (SectIt I=SS.begin(); I!=SS.end(); I++)
		if (!(L--)){
			*N = *I->first;
			*V = *I->second;
			return TRUE;
		}
	return FALSE;
}

//--------------------------------------------------------------------------------------------------------
// Write functions
//--------------------------------------------------------------------------------------
void	CInifile::w_string	( LPCSTR S, LPCSTR L, LPCSTR			V, LPCSTR comment)
{
	R_ASSERT	(!bReadOnly);

	// section
	char	sect	[256];
	_parse	(sect,S);
	_strlwr	(sect);
	if (!section_exist(sect))	{
		// create _new_ section
		Sect			NEW;
		NEW.Name		= sect;
		RootIt I		= std::lower_bound(DATA.begin(),DATA.end(),sect,sect_pred);
		DATA.insert		(I,NEW);
	}

	// parse line/value
	char	line	[256];	_parse	(line,L);
	char	value	[256];	_parse	(value,V);

	// duplicate & insert
	Item	I;
	Sect&	data	= r_section	(sect);
	I.first			= (line[0]?line:0);
	I.second		= (value[0]?value:0);
	I.comment		= (comment?comment:0);
	SectIt	it		= std::lower_bound(data.begin(),data.end(),*I.first,item_pred);

    if (it != data.end()) {
	    // Check for "first" matching
    	if (0==xr_strcmp(*it->first,*I.first)) {
            *it  = I;
        } else {
			data.Data.insert(it,I);
        }
    } else {
		data.Data.insert(it,I);
    }
}
void	CInifile::w_u8			( LPCSTR S, LPCSTR L, u8				V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%d",V);
	w_string	(S,L,temp,comment);
}
void	CInifile::w_u16			( LPCSTR S, LPCSTR L, u16				V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%d",V);
	w_string	(S,L,temp,comment);
}
void	CInifile::w_u32			( LPCSTR S, LPCSTR L, u32				V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%d",V);
	w_string	(S,L,temp,comment);
}
void	CInifile::w_s8			( LPCSTR S, LPCSTR L, s8				V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%d",V);
	w_string	(S,L,temp,comment);
}
void	CInifile::w_s16			( LPCSTR S, LPCSTR L, s16				V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%d",V);
	w_string	(S,L,temp,comment);
}
void	CInifile::w_s32			( LPCSTR S, LPCSTR L, s32				V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%d",V);
	w_string	(S,L,temp,comment);
}
void	CInifile::w_float		( LPCSTR S, LPCSTR L, float				V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%f",V);
	w_string	(S,L,temp,comment);
}
void	CInifile::w_fcolor		( LPCSTR S, LPCSTR L, const Fcolor&		V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%f,%f,%f,%f", V.r, V.g, V.b, V.a);
	w_string	(S,L,temp,comment);
}

void	CInifile::w_color		( LPCSTR S, LPCSTR L, u32				V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%d,%d,%d,%d", color_get_R(V), color_get_G(V), color_get_B(V), color_get_A(V));
	w_string	(S,L,temp,comment);
}

void	CInifile::w_ivector2	( LPCSTR S, LPCSTR L, const Ivector2&	V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%d,%d", V.x, V.y);
	w_string	(S,L,temp,comment);
}

void	CInifile::w_ivector3	( LPCSTR S, LPCSTR L, const Ivector3&	V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%d,%d,%d", V.x, V.y, V.z);
	w_string	(S,L,temp,comment);
}

void	CInifile::w_ivector4	( LPCSTR S, LPCSTR L, const Ivector4&	V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%d,%d,%d,%d", V.x, V.y, V.z, V.w);
	w_string	(S,L,temp,comment);
}
void	CInifile::w_fvector2	( LPCSTR S, LPCSTR L, const Fvector2&	V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%f,%f", V.x, V.y);
	w_string	(S,L,temp,comment);
}

void	CInifile::w_fvector3	( LPCSTR S, LPCSTR L, const Fvector3&	V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%f,%f,%f", V.x, V.y, V.z);
	w_string	(S,L,temp,comment);
}

void	CInifile::w_fvector4	( LPCSTR S, LPCSTR L, const Fvector4&	V, LPCSTR comment )
{
	string128 temp; sprintf		(temp,"%f,%f,%f,%f", V.x, V.y, V.z, V.w);
	w_string	(S,L,temp,comment);
}

void	CInifile::w_bool		( LPCSTR S, LPCSTR L, BOOL				V, LPCSTR comment )
{
	w_string	(S,L,V?"on":"off",comment);
}

void	CInifile::remove_line	( LPCSTR S, LPCSTR L )
{
	R_ASSERT	(!bReadOnly);

    if (line_exist(S,L)){
		Sect&	data	= r_section	(S);
		SectIt A = std::lower_bound(data.begin(),data.end(),L,item_pred);
    	R_ASSERT(A!=data.end() && xr_strcmp(*A->first,L)==0);
        data.Data.erase(A);
    }
}

