//----------------------------------------------------
// file: FileSystem.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

//#ifdef M_BORLAND
#include "cderr.h"
#include "commdlg.h"

EFS_Utils EFS;
//----------------------------------------------------
EFS_Utils::EFS_Utils( )
{
}

EFS_Utils::~EFS_Utils()
{
}

void EFS_Utils::OnCreate()
{
#ifdef M_BORLAND
	FS.update_path			(m_LastAccessFN,"$server_data_root$","access.ini");
    FS.update_path			(m_AccessLog,	"$server_data_root$","access.log");
#endif
}                                               
//----------------------------------------------------
/*
#ifdef _MSC_VER
#define utimbuf _utimbuf
LPCSTR ExtractFileExt(const string& fn)
{
	return strext(fn.c_str());
}
#endif
*/
LPCSTR MakeFilter(string1024& dest, LPCSTR info, LPCSTR ext)
{
	ZeroMemory(dest,sizeof(dest));
    if (ext){
        int icnt=_GetItemCount(ext,';');
		LPSTR dst=dest;
        if (icnt>1){
            strconcat(dst,info," (",ext,")");
            dst+=(xr_strlen(dst)+1);
            strcpy(dst,ext);
            dst+=(xr_strlen(ext)+1);
        }
        for (int i=0; i<icnt; i++){
            string64 buf;
            _GetItem(ext,i,buf,';');
            strconcat(dst,info," (",buf,")");
            dst+=(xr_strlen(dst)+1);
            strcpy(dst,buf);
            dst+=(xr_strlen(buf)+1);
        }
    }
	return dest;
}

//------------------------------------------------------------------------------
// start_flt_ext = -1-all 0..n-indices
//------------------------------------------------------------------------------
bool EFS_Utils::GetOpenName( LPCSTR initial, char *buffer, int sz_buf, bool bMulti, LPCSTR offset, int start_flt_ext )
{
	VERIFY(buffer&&(sz_buf>0));
	FS_Path& P			= *FS.get_path(initial);
	string1024 flt;
	MakeFilter(flt,P.m_FilterCaption,P.m_DefExt);

	OPENFILENAME ofn;
	Memory.mem_fill		( &ofn, 0, sizeof(ofn) );
    ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner 		= GetForegroundWindow();
	ofn.lpstrDefExt 	= P.m_DefExt;
	ofn.lpstrFile 		= buffer;
	ofn.nMaxFile 		= sz_buf;
	ofn.lpstrFilter 	= flt;
	ofn.nFilterIndex 	= start_flt_ext+2;
    ofn.lpstrTitle      = "Open a File";

    string512 path; strcpy(path,(offset&&offset[0])?offset:P.m_Path);
	ofn.lpstrInitialDir = path;
	ofn.Flags =
    	OFN_PATHMUSTEXIST|
    	OFN_FILEMUSTEXIST|
		OFN_HIDEREADONLY|
		OFN_FILEMUSTEXIST|
		OFN_NOCHANGEDIR|(bMulti?OFN_ALLOWMULTISELECT|OFN_EXPLORER:0);
	bool bRes = !!GetOpenFileName( &ofn );
    if (!bRes){
	    u32 err = CommDlgExtendedError();
	    switch(err){
        case FNERR_BUFFERTOOSMALL: 	Log("Too many file selected."); break;
        }
	}
    if (bRes&&bMulti){
		int cnt		= _GetItemCount(buffer,0x0);
        if (cnt>1){
            string64  	buf;
            string64  	dir;
            string4096 	fns;
            strcpy(dir, buffer);

            strcpy		(fns,dir);
            strcat		(fns,"\\");
            strcat		(fns,_GetItem(buffer,1,buf,0x0));
            for (int i=2; i<cnt; i++){
                strcat	(fns,",");
                strcat	(fns,dir);
                strcat	(fns,"\\");
                strcat	(fns,_GetItem(buffer,i,buf,0x0));
            }
            strcpy		(buffer,fns);
        }
    }
    strlwr(buffer);
    return bRes;
}

bool EFS_Utils::GetSaveName( LPCSTR initial, char *buffer, int sz_buf, LPCSTR offset, int start_flt_ext )
{
	VERIFY(buffer&&(sz_buf>0));
	FS_Path& P			= *FS.get_path(initial);
	string1024 flt;
	MakeFilter(flt,P.m_FilterCaption,P.m_DefExt);
	OPENFILENAME ofn;
	Memory.mem_fill		( &ofn, 0, sizeof(ofn) );
	ofn.hwndOwner 		= GetForegroundWindow();
	ofn.lpstrDefExt 	= P.m_DefExt;
	ofn.lpstrFile 		= buffer;
	ofn.lpstrFilter 	= flt;
	ofn.lStructSize 	= sizeof(ofn);
	ofn.nMaxFile 		= sz_buf;
	ofn.nFilterIndex 	= start_flt_ext+2;
    ofn.lpstrTitle      = "Save a File";
    string512 path; strcpy(path,(offset&&offset[0])?offset:P.m_Path);
	ofn.lpstrInitialDir = path;
	ofn.Flags 			= OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;

	bool bRes = !!GetSaveFileName( &ofn );
    if (!bRes){
	    u32 err = CommDlgExtendedError();
	    switch(err){
        case FNERR_BUFFERTOOSMALL: 	Log("Too many file selected."); break;
        }
	}
    strlwr(buffer);
	return bRes;
}
//----------------------------------------------------
LPSTR EFS_Utils::AppendFolderToName(LPSTR tex_name)
{
	string256 _fn;
	strcpy(tex_name,AppendFolderToName(tex_name, _fn));
	return tex_name;
}

LPSTR EFS_Utils::AppendFolderToName(LPCSTR src_name, LPSTR dest_name)
{
	ref_str tmp = dest_name;
    LPCSTR s 	= src_name;
    LPSTR d 	= dest_name;
	for (; *s; s++, d++) 
    	*d		=(*s=='_')?'\\':*s; 
    *d			= 0;
	strcat		(dest_name,*tmp);
    return dest_name;
/*    
	if (_GetItemCount(src_name,'_')>1){
		string256 fld;
		_GetItem(src_name,0,fld,'_');
		sprintf(dest_name,"%s\\%s",fld,src_name);
	}else{
		strcpy(dest_name,src_name);
	}
	return dest_name;
*/
}

LPCSTR EFS_Utils::GenerateName(LPCSTR base_path, LPCSTR base_name, LPCSTR def_ext, LPSTR out_name)
{
    int cnt = 0;
	string256 fn;
    if (base_name)	strconcat	(fn,base_path,base_name,def_ext);
	else 			sprintf		(fn,"%s%02d%s",base_path,cnt++,def_ext);
	while (FS.exist(fn))
	    if (base_name)	sprintf(fn,"%s%s%02d%s",base_path,base_name,cnt++,def_ext);
        else 			sprintf(fn,"%s%02d%s",base_path,cnt++,def_ext);
    strcpy(out_name,fn);
	return out_name;
}

//#endif
