// ScriptAPI.cpp: implementation of the CScriptAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lzhuf.h"
#include "ScriptAPI.h"
#include "scripting\script.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScriptAPI::CScriptAPI()
{

}

CScriptAPI::~CScriptAPI()
{
#ifndef DEBUG
	vector<SScriptDef>::iterator i = Scripts.begin();
	for (;i<Scripts.end();i++)	_FREE(i->S);
#endif
}

extern void		SAPI_Export(void);

struct FTIME {
	FILETIME	t_create;
	FILETIME	t_access;
	FILETIME	t_modify;
};

BOOL FT_Get(const char *FN, FTIME &T)
{
	int hfile	= _open(FN,O_BINARY|O_RDONLY);
	if	(hfile<=0) return FALSE;

	int handle	= _get_osfhandle(hfile);

	BOOL res	= GetFileTime(HANDLE(handle),&T.t_create,&T.t_access,&T.t_modify);

	_close		(hfile);

	return		res;
}
BOOL FT_ModifyEqual	(FTIME &T1, FTIME &T2)
{
	return (0==CompareFileTime(&T1.t_modify,&T2.t_modify));
}
BOOL FT_ModifySet	(const char *FN, FTIME &T)
{
	int hfile	= _open(FN,O_BINARY|O_RDWR);
	if	(hfile<=0) return FALSE;

	int handle	= _get_osfhandle(hfile);
	BOOL res	= SetFileTime(HANDLE(handle),0,0,&T.t_modify);
	return		res;
}

void CScriptAPI::Initialize()
{
	Log("Compiling scripts...");

	SAPI_Export();

	_finddata_t F;
	int hFile =_findfirst("scripts\\*.c",&F);
	int res   = 0;
	while (res==0 && hFile>=0) {
		char		fname[_MAX_PATH];
		char		cname[_MAX_PATH];
		SScriptDef	S;

		// Make names
		strcpy	(fname,"scripts\\");
		strcat	(fname,F.name);

		strcpy	(S.name,F.name);
		if (strchr(S.name,'.')) *(strchr(S.name,'.'))=0;
		strlwr	(S.name);

		strcpy	(cname,"scripts\\");
		strcat	(cname,S.name);
		strcat	(cname,".cs");

		// Check timestamp
		FTIME	tS,tC;
		BOOL	bExist;

		FT_Get(fname,tS);
		bExist = FT_Get(cname,tC);
 
		CLASS_ID signature = MK_CLSID('T','P','I','R','C','S','r','x');
		if ((!bExist) || (!FT_ModifyEqual(tS,tC))) {
			Msg("   compiling '%s'",S.name);
			DWORD size;
			char *file_text = (char *)FileDownload(fname, &size);
			file_text[size-1]=0;
			S.S=scCompile_Text(file_text);
			_FREE(file_text);

			// Save compiled code to disk
			size = scGet_Script_Size(S.S);
			int hsave = _open(cname,O_BINARY|O_CREAT|O_WRONLY,S_IREAD|S_IWRITE);
			_write	(hsave,&signature,8);
			_writeLZ(hsave,S.S,size);
			_close(hsave);
			FT_ModifySet(cname,tS);
		} else {
			Msg("   loading '%s'",S.name);
			int hload = _open(cname,O_BINARY|O_RDONLY);
			CLASS_ID	sign;
			_read	(hload,&sign,8);
			R_ASSERT(sign==signature);
			void* ptr = 0;
			_readLZ (hload,ptr,filelength(hload)-8);
			S.S = (char*)ptr;
			_close	(hload);
		}

		if (scErrorNo) {
			Msg("    %s\n> %s\n",scErrorMsg,scErrorLine);
			THROW;
		} else {
			Scripts.push_back(S);
		}

		res=_findnext(hFile,&F);
	}
	_findclose(hFile);
}

void CScriptAPI::Destroy()
{
	for (DWORD i=0; i<Scripts.size(); i++)
		_FREE(Scripts[i].S);
	Scripts.clear	();
}

CScript*	CScriptAPI::CreateInstance(const char *name)
{
	char N[256];
	strcpy(N,name);
	strlwr(N);

	vector<SScriptDef>::iterator i = Scripts.begin();
	for (;i<Scripts.end();i++)
		if (strcmp(i->name,N)==0) {
			// script found
			CScript*	pS = new CScript(i->S);
			return pS;
		}
	R_ASSERT(0=="Engine can't find required script");
	return NULL;
}
