#include "stdafx.h"
#include "xrShaderDef.h"

static CLASS_ID signature = MK_CLSID('R','E','D','A','H','S','r','x');

IC bool sort_pred(const SH_ShaderDef& A, const SH_ShaderDef& B)
{	return strcmp(A.cName,B.cName)<0; }

void shLibrary_Sort(std::vector<SH_ShaderDef> &LIB)
{
	std::sort(LIB.begin(),LIB.end(),sort_pred);
}

DWORD	shLibrary_GetCount	(const char *Name)
{
	CLASS_ID	sign; DWORD count;
	int hload = _open(Name,O_BINARY|O_RDONLY);
	_read	(hload,&sign,8	);
	_read	(hload,&count,4	);
	R_ASSERT(sign==signature);
	_close	(hload);
	return count;
}
void shLibrary_Load(const char *Name, std::vector<SH_ShaderDef> &LIB)
{
	LIB.clear	();
	CLASS_ID	sign; DWORD count;
	int hload = _open(Name,O_BINARY|O_RDONLY);
	_read	(hload,&sign,8	);
	_read	(hload,&count,4	);
	R_ASSERT(sign==signature);
	void* ptr = 0;
	_readLZ (hload,ptr,filelength(hload)-8);
	_close	(hload);

	if (ptr) {
		LIB.resize	(count);
		CopyMemory	(LIB.begin(),ptr,count*sizeof(SH_ShaderDef));
		free		(ptr);
	}
}

void shLibrary_Save(const char *Name, std::vector<SH_ShaderDef> &LIB)
{
	DWORD count = LIB.size();
	if (count) shLibrary_Sort(LIB);

	int hsave = _open(Name,O_BINARY|O_CREAT|O_WRONLY|O_TRUNC,S_IREAD|S_IWRITE);
	_write	(hsave,&signature,8);
	_write	(hsave,&count,4);
	_writeLZ(hsave,LIB.begin(),count*sizeof(SH_ShaderDef));
	_close	(hsave);
}

int	shLibrary_Find(const char* Name, std::vector<SH_ShaderDef> &LIB)
{
	SH_ShaderDef	D; strcpy(D.cName,Name);
	SH_ShaderDef*	P = std::lower_bound(LIB.begin(),LIB.end(),D,sort_pred);
	if ((P!=LIB.end()) && (0==strcmp(P->cName,Name)) ) return P-LIB.begin();
	return -1;
}
