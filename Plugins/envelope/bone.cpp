#include "stdafx.h"
#pragma hdrstop

#include "bone.h"
#include "envelope.h"
#include "fs.h"

CBone::~CBone(){
}

void CBone::Save(CFS_Base& F){
#ifdef LWO_EXPORTS
	extern char* ReplaceSpace(char* s);
	ReplaceSpace(name);		strlwr(name);
	ReplaceSpace(parent);	strlwr(parent);
#endif
	F.WstringZ	(name);
	F.WstringZ	(parent);
	F.WstringZ	(wmap);
	F.Wvector	(rest_offset);
	F.Wvector	(rest_rotate);
	F.Wfloat	(rest_length);
}

void CBone::Load(CStream& F){
	F.RstringZ	(name);		
	F.RstringZ	(parent);	
	F.RstringZ	(wmap);
	F.Rvector	(rest_offset);
	F.Rvector	(rest_rotate);
	rest_length	= F.Rfloat();
    Reset		();
}
