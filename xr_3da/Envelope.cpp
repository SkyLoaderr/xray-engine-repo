#include "stdafx.h"
#include "envelope.h"

#ifdef LWO_EXPORTS
	#include "fs.h"
#else 
#ifdef _EDITOR
	#include "fs.h"
#else
	#include "..\fs.h"
#endif
#endif

CEnvelope::~CEnvelope(){
	for (KeyIt k_it=keys.begin(); k_it!=keys.end(); k_it++)
    	_DELETE(*k_it);
}
CEnvelope::CEnvelope(CEnvelope* source){
	*this 		= *source;
	for (DWORD i=0; i<source->keys.size(); i++)
    	keys[i]	= new st_Key(*source->keys[i]);
}

extern float evalEnvelope( CEnvelope *env, float time );
float CEnvelope::Evaluate(float time){
	return evalEnvelope(this, time);
}

void CEnvelope::Save(CFS_Base& F){
	F.write		(behavior,sizeof(int)*2);
	F.Wdword	(keys.size());
	for (KeyIt k_it=keys.begin(); k_it!=keys.end(); k_it++)
		F.write	(*k_it,sizeof(st_Key));
}

void CEnvelope::Load(CStream& F){
	F.Read		(behavior,sizeof(int)*2);
	keys.resize	(F.Rdword());
	for (DWORD i=0; i<keys.size(); i++){
    	keys[i]	= new st_Key();
		F.Read	(keys[i],sizeof(st_Key));
    }
}

