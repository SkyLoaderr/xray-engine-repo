#include "stdafx.h"
#pragma hdrstop

#include "bone.h"
#include "envelope.h"

CBone::~CBone(){
}

void CBone::Save(IWriter& F){
#ifdef _LW_EXPORT
	extern char* ReplaceSpace(char* s);
	ReplaceSpace(name);		strlwr(name);
	ReplaceSpace(parent);	strlwr(parent);
#endif
	F.w_stringZ	(name);
	F.w_stringZ	(parent);
	F.w_stringZ	(wmap);
	F.w_fvector3(rest_offset);
	F.w_fvector3(rest_rotate);
	F.w_float	(rest_length);
}

void CBone::Load(IReader& F){
	F.r_stringZ	(name);
	F.r_stringZ	(parent);
	F.r_stringZ	(wmap);
	F.r_fvector3(rest_offset);
	F.r_fvector3(rest_rotate);
	rest_length	= F.r_float();
    Reset		();
}
