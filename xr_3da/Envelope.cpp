#include "stdafx.h"
#pragma hdrstop

#include "envelope.h"

CEnvelope::~CEnvelope()
{
	for (KeyIt k_it=keys.begin(); k_it!=keys.end(); k_it++)
    	xr_delete(*k_it);
}
CEnvelope::CEnvelope(CEnvelope* source)
{
	*this 		= *source;
	for (u32 i=0; i<source->keys.size(); i++)
    	keys[i]	= xr_new<st_Key> (*source->keys[i]);
}

void CEnvelope::RotateKeys(float angle)
{
	for (u32 i=0; i<keys.size(); i++)
    	keys[i]->value += angle;
}


extern float evalEnvelope( CEnvelope *env, float time );
float CEnvelope::Evaluate(float time){
	return evalEnvelope(this, time);
}

void CEnvelope::Save(IWriter& F){
	F.w			(behavior,sizeof(int)*2);
	F.w_u32		(keys.size());
	for (KeyIt k_it=keys.begin(); k_it!=keys.end(); k_it++)
		F.w		(*k_it,sizeof(st_Key));
}

void CEnvelope::Load(IReader& F){
	F.r			(behavior,sizeof(int)*2);
	keys.resize	(F.r_u32());
	for (u32 i=0; i<keys.size(); i++){
    	keys[i]	= xr_new<st_Key> ();
		F.r		(keys[i],sizeof(st_Key));
    }
}

void CEnvelope::SaveA(IWriter& F){
}

void CEnvelope::LoadA(IReader& F){
	string512 	buf;
    float		f[9];
	F.r_string(buf);
	if (strstr(buf,"{ Envelope")){
		F.r_string(buf);
        int nkeys=atoi(buf);
		keys.resize(nkeys);
		for (u32 i=0; i<keys.size(); i++){
    		keys[i]	= xr_new<st_Key> ();
            st_Key& K=*keys[i];
			F.r_string(buf);
            int cnt = sscanf	(buf,"Key %f %f %f %f %f %f %f %f %f", f+0, f+1, f+2, f+3, f+4, f+5, f+6, f+7, f+8);
            R_ASSERT(cnt==9);
			K.value = f[ 0 ];
      		K.time  = f[ 1 ];
      		K.shape = ( int ) f[ 2 ];
			if ( K.shape == SHAPE_TCB ) {
         		K.tension    = f[ 3 ];
         		K.continuity = f[ 4 ];
         		K.bias       = f[ 5 ];
      		}
            if ( K.shape == SHAPE_BEZ2 ) {
         		K.param[ 0 ] = f[ 3 ];
         		K.param[ 1 ] = f[ 4 ];
         		K.param[ 2 ] = f[ 5 ];
         		K.param[ 3 ] = f[ 6 ];
      		}else{
         		K.param[ 0 ] = f[ 6 ];
                K.param[ 1 ] = f[ 7 ];
      		}
	    }
        // behavior <pre> <post>
		F.r_string(buf);
		int cnt = sscanf(buf,"Behaviors %d %d", behavior[0], behavior[1] );
        R_ASSERT(cnt==2);
    }
}

