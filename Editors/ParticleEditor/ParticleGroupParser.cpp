//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "xr_tokens.h"
#include "xr_trims.h"
#include "ParticleGroup.h"
#include "engine/particles/papi.h"
#include "engine/particles/general.h"

xr_token					al_command_token	[ ]={
	{ "pAvoid",				PAAvoidID			},			
	{ "pBounce",			PABounceID			},			
	{ "pCallActionList",	PACallActionListID	},	
	{ "pCopyVertexB",		PACopyVertexBID		},	
	{ "pDamping",			PADampingID			},		
	{ "pExplosion",			PAExplosionID		},		
	{ "pFollow",			PAFollowID			},			
    { "pFrame",				PAFrameID			},
	{ "pGravitate",			PAGravitateID		},		
	{ "pGravity",			PAGravityID			},		
	{ "pJet",				PAJetID				},			
	{ "pKillOld",			PAKillOldID			},		
	{ "pMatchVelocity",		PAMatchVelocityID	},	
	{ "pMove",				PAMoveID			},			
	{ "pOrbitLine",			PAOrbitLineID		},		
	{ "pOrbitPoint",		PAOrbitPointID		},		
	{ "pRandomAccel",		PARandomAccelID		},	
	{ "pRandomDisplace",	PARandomDisplaceID	},	
	{ "pRandomVelocity",	PARandomVelocityID	},	
	{ "pRestore",			PARestoreID			},		
	{ "pSink",				PASinkID			},			
	{ "pSinkVelocity",		PASinkVelocityID	},	
	{ "pSource",			PASourceID			},			
	{ "pSpeedLimit",		PASpeedLimitID		},		
	{ "pTargetColor",		PATargetColorID		},	
	{ "pTargetSize",		PATargetSizeID		},
    { "pTargetRotate",		PATargetRotateID	},
    { "pTargetRotateD",		PATargetRotateDID	},
	{ "pTargetVelocity",	PATargetVelocityID	},	
	{ "pTargetVelocityD",	PATargetVelocityDID	},	
	{ "pVortex",			PAVortexID			},			
	{ 0,					0				   	},
};

xr_token					domain_token		[ ]={
	{ "PDPoint",	  		PDPoint				},
	{ "PDLine",	  			PDLine				},
	{ "PDTriangle",	  		PDTriangle			},
	{ "PDPlane",	  		PDPlane				},
	{ "PDBox",	  			PDBox				},
	{ "PDSphere",	  		PDSphere			},
	{ "PDCylinder",	  		PDCylinder			},
	{ "PDCone",	  			PDCone				},
	{ "PDBlob",	  			PDBlob				},
	{ "PDDisc",	  			PDDisc				},
	{ "PDRectangle",	  	PDRectangle			},
	{ 0,					0				   	},
};

IC int get_int(LPCSTR src, int idx, int p_cnt, u32& err, int def=0)
{
	if (idx<p_cnt){
		string128 a;
	    _GetItem(src,idx,a,',');
    	int res = atoi(a);
        for (int k=0; a[k]; k++){
        	if (!isdigit(a[k])&&(a[k]!='-')&&(a[k]!='+'))
        		err |= (1<<idx);
        }
        return res;
    }else return def;
}

IC float get_float(LPCSTR src, int idx, int p_cnt, u32& err, float def=0)
{
	if (idx<p_cnt){
		string128 a;
	    _GetItem(src,idx,a,',');
    	float res = atof(a);
        for (int k=0; a[k]; k++){
        	if (!isdigit(a[k])&&(a[k]!='.')&&(a[k]!='-')&&(a[k]!='+'))
        		err |= (1<<idx);
        }
        return res;
    }else return def;
}

IC BOOL get_bool(LPCSTR src, int idx, int p_cnt, u32& err, BOOL def=TRUE)
{
	if (idx<p_cnt){
		string128 a;
	    _GetItem(src,idx,a,',');
        bool b0 = (0==stricmp(a,"false"));
        bool b1 = (0==stricmp(a,"true"));
        if (b0==b1) 
        	err |= (1<<idx);
    	return b1;
    }else return def;
}

IC AnsiString get_string(LPCSTR src, int idx, int p_cnt, u32& err, AnsiString def="")
{
	if (idx<p_cnt){
		AnsiString a,b;
	    _GetItem(src,idx,a,',');
        _GetItem(a.c_str(),1,b,'"');
    	return b;
    }else return def;
}

IC _get_token_ID(xr_token* token_list, LPCSTR name){
	for( int i=0; token_list[i].name; i++ )
		if( !strcmp(name,token_list[i].name) )
			return token_list[i].id;
    return -1;
}

struct SDomain{
	PDomainEnum	dtype;
    float 		a[9];
    SDomain		(){a[0]=0.f;a[1]=0.f;a[2]=0.f;a[3]=0.f;a[4]=0.f;a[5]=0.f;a[6]=0.f;a[7]=0.f;a[8]=0.f;dtype=PDPoint;}
};

IC bool ParseDomainType(LPCSTR src, SDomain& s, int start_p, u32& err)
{
	string128 dtype,a;
    _GetItem(src,start_p,dtype,',');
    int itm_cnt = _GetItemCount(src);
    if (itm_cnt<start_p) return false;
    s.dtype = (PDomainEnum)_get_token_ID(domain_token,dtype);
    if (-1==(int)s.dtype) err |= (1<<start_p);
    for (int k=start_p+1; k<itm_cnt; k++)
	    s.a[k-start_p-1] = get_float(src,k,itm_cnt,err);
    return true;
}

#define EXPAND_DOMAIN(D) D.dtype,D.a[0],D.a[1],D.a[2],D.a[3],D.a[4],D.a[5],D.a[6],D.a[7],D.a[8]


class PFunction{
	// command
	AnsiString 			command;
    // type
	enum Type{
    	ftState,
        ftAction
    };
    Type				type
    // params
	struct Param{
    	enum Type{
        	ptBOOL,
            ptFloat,
            ptInt,
            ptDomain
        };
        Type	type;
        union{
        	struct{BOOL 		_BOOL;	};
        	struct{float 		_float; };
        	struct{int 			_int;   };
        	struct{PDomainEnum 	_domain;};
        };
    };
    int 				req_params;
    DEFINE_VECTOR(Param,ParamVec,ParamIt);
    ParamVec			params;
public:    
	int					LoadTemplate(const AnsiString& line);
    int					Parse		(const AnsiString& line); // err_code
    const AnsiString&	Command		(){return command;}
    const Param&		Param		(int num){R_ASSERT(num<params.size()); return params[num];}
};

DEFINE_MAP(AnsiString,PFunction,CTMap,CTPairIt);

static CTMap CommandTemplates;

// error constants
static const int BAD_COMMAND	= (1<<31);
static const int FEW_PARAMS 	= (1<<30);

static LPCSTR PStateCommands[]={
	"pRotate(float rot_x, float rot_y=0.f, float rot_z=0.f);",
	"pRotateD(PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pColor(float red, float green, float blue, float alpha = 1.0f);",
	"pColorD(float alpha, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pSize(float size_x, float size_y = 1.0f, float size_z = 1.0f);",
	"pSizeD(PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pStartingAge(float age, float sigma = 1.0f);",
	"pTimeStep(float new_dt);",
	"pVelocity(float x, float y, float z);",
	"pVelocityD(PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pVertexB(float x, float y, float z);",
	"pVertexBD(PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pVertexBTracks(BOOL track_vertex = TRUE);",
	"pSetMaxParticles(int max_count);",
};

static LPCSTR PActionCommnds[]={
	"pFrame(BOOL animated=FALSE, BOOL random_frame=FALSE, BOOL random_playback=FALSE, WORD frame_count=16, float speed=24.f);",
	"pAvoid(float magnitude, float epsilon, float look_ahead,  PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pBounce(float friction, float resilience, float cutoff, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pCopyVertexB(BOOL copy_pos = TRUE, BOOL copy_vel = FALSE);",
	"pDamping(float damping_x, float damping_y, float damping_z, float vlow = 0.0f, float vhigh = P_MAXFLOAT);",
	"pExplosion(float center_x, float center_y, float center_z, float velocity, float magnitude, float stdev, float epsilon = P_EPS, float age = 0.0f);",
	"pFollow(float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);",
	"pGravitate(float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);",
	"pGravity(float dir_x, float dir_y, float dir_z);",
	"pJet(float center_x, float center_y, float center_z, float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);",
	"pKillOld(float age_limit, BOOL kill_less_than = FALSE);",
	"pMatchVelocity(float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);",
	"pMove();",
	"pOrbitLine(float p_x, float p_y, float p_z, float axis_x, float axis_y, float axis_z, float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);",
	"pOrbitPoint(float center_x, float center_y, float center_z, float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);",
	"pRandomAccel(PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pRandomDisplace(PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pRandomVelocity(PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pRestore(float time);",
	"pSink(BOOL kill_inside, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pSinkVelocity(BOOL kill_inside, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pSource(float particle_rate, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pSpeedLimit(float min_speed, float max_speed = P_MAXFLOAT);",
	"pTargetColor(float color_x, float color_y, float color_z, float alpha, float scale);",
	"pTargetColorD(float color_x, float color_y, float color_z, float alpha, float scale);",
	"pTargetSize(float size_x, float size_y, float size_z, float scale_x = 0.0f, float scale_y = 0.0f, float scale_z = 0.0f);",
	"pTargetRotate(float rot_x, float rot_y, float rot_z, float scale = 0.0f);",
	"pTargetRotateD(float scale, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pTargetVelocity(float vel_x, float vel_y, float vel_z, float scale);",
	"pTargetVelocityD(float scale, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pVertex(float x, float y, float z);",
	"pVortex(float center_x, float center_y, float center_z, float axis_x, float axis_y, float axis_z, float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);"
};

bool InitCommandTemplates(CTMap& CommandTemplates)
{
}

void CParticleGroup::ParseCommandList(LPCSTR txt)
{
	// load templates
	if (CommandTemplates.empty()) R_ASSERT(InitCommandTemplates(CommandTemplates)));
    
    // parse
    AStringVec 			lst;
    SequenceToList		();//lst,txt,';');

    int err_code		= 0;
	bool bBadCommand	= false;
    bool bFewParams		= false;
    
	AnsiString 			cmd,param;
    u32	err				= 0;
    int i_line;
    LPCSTR p			= 0;
    do{                                             
        // parse state command
        for (i_line=0; i_line<(int)lst.size(); i_line++){
            AStringIt it= lst.begin() + i_line;
            // parse command
            _GetItem	(it->c_str(),0,cmd,'(');
            _GetItem	(it->c_str(),1,param,'(');
            _GetItem	(param.c_str(),0,param,')');
                       
            if (1==cmd.Pos("//")){
                lst.erase(it);
                i_line--;
            	continue;
            }
            SDomain		D;
            int p_cnt	= _GetItemCount	(param.c_str());
            p			= param.c_str();
            bool bExec	= true;

            if (cmd==	   "pColor"){          		
                if (bRes=(p_cnt>=3)) 						pColor			(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err), get_float(p,2,p_cnt,err), get_float(p,3,p_cnt,err,1.0f));
            }else if (cmd=="pColorD"){     		
                if (bRes=((p_cnt>=2)&&ParseDomainType(p,D,1,err)))pColorD  	(get_float(p,0,p_cnt,err), EXPAND_DOMAIN(D));
            }else if (cmd=="pVelocity"){   		
                if (bRes=(p_cnt==3)) 						pVelocity		(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err), get_float(p,2,p_cnt,err));
            }else if (cmd=="pVelocityD"){  		
                if (bRes=((p_cnt>=1)&&ParseDomainType(p,D,0,err)))pVelocityD(EXPAND_DOMAIN(D));
            }else if (cmd=="pVertexB"){   		
                if (bRes=(p_cnt==3)) 						pVertexB		(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err), get_float(p,2,p_cnt,err));
            }else if (cmd=="pVertexBD"){  		
                if (bRes=((p_cnt>=1)&&ParseDomainType(p,D,0,err)))pVertexBD	(EXPAND_DOMAIN(D));
            }else if (cmd=="pVertexBTracks"){	
                if (bRes=(p_cnt==1)) 						pVertexBTracks	(get_bool(p,0,p_cnt,err));
            }else if (cmd=="pSize"){       		
                if (bRes=(p_cnt>=1)) 						pSize			(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err,0.0f), get_float(p,1,p_cnt,err,0.0f));
            }else if (cmd=="pSizeD"){      		
                if (bRes=((p_cnt>=1)&&ParseDomainType(p,D,0,err)))pSizeD   	(EXPAND_DOMAIN(D));
            }else if (cmd=="pRotate"){       		
                if (bRes=(p_cnt>=1)) 						pRotate			(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err,0.0f), get_float(p,1,p_cnt,err,0.0f));
            }else if (cmd=="pRotateD"){      		
                if (bRes=((p_cnt>=1)&&ParseDomainType(p,D,0,err)))pRotateD 	(EXPAND_DOMAIN(D));
            }else if (cmd=="pStartingAge"){		
                if (bRes=(p_cnt>=1)) 						pStartingAge   	(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err,0.0f));
            }else if (cmd=="pTimeStep"){   		
                if (bRes=(p_cnt==1)) 						pTimeStep	   	(get_float(p,0,p_cnt,err));
            }else if (cmd=="pSetMaxParticles"){	
                if (bRes=(p_cnt==1)) 						pSetMaxParticles(get_float(p,0,p_cnt,err));
            }else if (cmd=="pSprite"){	
                if (bRes=(p_cnt==2)) 						pSprite			(get_string(p,0,p_cnt,err).c_str(), get_string(p,1,p_cnt,err).c_str());
            }else bExec = false;

            if (!bRes||err) break;
            if (bExec){
	            lst.erase(it);
    	        i_line--;
            }
        }

        if (!bRes||err) break;

        // parse action list command
		pNewActionList	(m_HandleActionList);
        for (i_line=0; i_line<(int)lst.size(); i_line++){//AStringIt it=lst.begin(); it!=lst.end(); it++){
            AStringIt it= lst.begin() + i_line;
            _GetItem	(it->c_str(),0,cmd,'(');
            _GetItem	(it->c_str(),1,param,'(');
            _GetItem	(param.c_str(),0,param,')');
                       
            PActionEnum action 			= (PActionEnum)_get_token_ID(al_command_token,cmd.c_str());
            SDomain		D;
            int p_cnt	= _GetItemCount	(param.c_str());
            p			= param.c_str();
            switch(action){
            case PAAvoidID: 		
                if (bRes=((p_cnt>=4)&&ParseDomainType(p,D,3,err))) 
                    pAvoid(get_float(p,0,p_cnt,err),get_float(p,1,p_cnt,err),get_float(p,2,p_cnt,err),EXPAND_DOMAIN(D)); 	
            break;
            case PABounceID: 		
                if (bRes=((p_cnt>=4)&&ParseDomainType(p,D,3,err))) 
                    pBounce(get_float(p,0,p_cnt,err),get_float(p,1,p_cnt,err),get_float(p,2,p_cnt,err),EXPAND_DOMAIN(D));	
            break;
            case PACallActionListID:   
            break;
            case PACopyVertexBID:	
                pCopyVertexB(get_bool(p,0,p_cnt,err),get_bool(p,1,p_cnt,err));         
            break;
            case PADampingID:		
                if (bRes=(p_cnt>=3))            
                    pDamping(get_float(p,0,p_cnt,err),get_float(p,1,p_cnt,err),get_float(p,2,p_cnt,err),get_float(p,3,p_cnt,err),get_float(p,4,p_cnt,err)); 
            break;
            case PAExplosionID:		
                if (bRes=(p_cnt>=6)) 
                    pExplosion(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err), get_float(p,2,p_cnt,err), get_float(p,3,p_cnt,err), get_float(p,4,p_cnt,err), get_float(p,5,p_cnt,err), get_float(p,6,p_cnt,err,P_EPS), get_float(p,0,p_cnt,err,0.0f));
            break;
            case PAFollowID: 		   
                pFollow(get_float(p,0,p_cnt,err,1.0f), get_float(p,1,p_cnt,err,P_EPS), get_float(p,2,p_cnt,err,P_MAXFLOAT));
            break;
            case PAFrameID:
//                pFrame(get_bool(p,0,p_cnt,err,FALSE), get_bool(p,1,p_cnt,err,FALSE), get_bool(p,2,p_cnt,err,FALSE), get_int(p,3,p_cnt,16), get_float(p,4,p_cnt,err,24.f));
                pFrame( get_float(p,0,p_cnt,err,24.f), get_bool(p,1,p_cnt,err,FALSE), get_bool(p,2,p_cnt,err,FALSE), get_bool(p,3,p_cnt,err,FALSE), 
                		get_int(p,4,p_cnt,err,32), get_int(p,5,p_cnt,err,32), get_int(p,6,p_cnt,err,16), 
                		get_int(p,7,p_cnt,err,128), get_int(p,8,p_cnt,err,128));
			break;
            case PAGravitateID: 	   
                pGravitate(get_float(p,0,p_cnt,err,1.0f), get_float(p,1,p_cnt,err,P_EPS), get_float(p,2,p_cnt,err,P_MAXFLOAT));
            break;
            case PAGravityID: 		   
                if (bRes=(p_cnt==3))
                    pGravity(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err), get_float(p,2,p_cnt,err));
            break;
            case PAJetID: 			   
                if (bRes=(p_cnt>=3))
                    pJet(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err), get_float(p,2,p_cnt,err), get_float(p,3,p_cnt,err,1.0f), get_float(p,4,p_cnt,err,P_EPS), get_float(p,5,p_cnt,err,P_MAXFLOAT));
            break;
            case PAKillOldID: 		   
                if (bRes=(p_cnt>=1))
                    pKillOld(get_float(p,0,p_cnt,err), get_bool(p,1,p_cnt,err,false));
            break;
            case PAMatchVelocityID:    
                pMatchVelocity(get_float(p,0,p_cnt,err,1.0f), get_float(p,1,p_cnt,err,P_EPS), get_float(p,2,p_cnt,err,P_MAXFLOAT));
            break;
            case PAMoveID: 			   
                pMove(); 
            break;
            case PAOrbitLineID: 	   
                if (bRes=(p_cnt>=6))
                    pOrbitLine(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err), get_float(p,2,p_cnt,err), get_float(p,3,p_cnt,err), get_float(p,4,p_cnt,err), get_float(p,5,p_cnt,err), get_float(p,6,p_cnt,err,1.0f), get_float(p,7,p_cnt,err,P_EPS), get_float(p,0,p_cnt,err,P_MAXFLOAT));
            break;
            case PAOrbitPointID: 	   
                if (bRes=(p_cnt>=3))
                    pOrbitPoint(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err), get_float(p,2,p_cnt,err), get_float(p,3,p_cnt,err,1.0f), get_float(p,4,p_cnt,err,P_EPS), get_float(p,5,p_cnt,err,P_MAXFLOAT));
            break;
            case PARandomAccelID: 	   
                if (bRes=((p_cnt>=1)&&ParseDomainType(p,D,0,err)))
                    pRandomAccel(EXPAND_DOMAIN(D));
            break;
            case PARandomDisplaceID:   
                if (bRes=((p_cnt>=1)&&ParseDomainType(p,D,0,err)))
                    pRandomDisplace(EXPAND_DOMAIN(D));
            break;
            case PARandomVelocityID:   
                if (bRes=((p_cnt>=1)&&ParseDomainType(p,D,0,err)))
                    pRandomVelocity(EXPAND_DOMAIN(D));
            break;
            case PARestoreID: 		   
                if (bRes=(p_cnt==1))
                    pRestore(get_float(p,0,p_cnt,err));
            break;
            case PASinkID: 			   
                if (bRes=((p_cnt>=2)&&ParseDomainType(p,D,1,err)))
                    pSink(get_bool(p,0,p_cnt,err), EXPAND_DOMAIN(D));
            break;
            case PASinkVelocityID: 	   
                if (bRes=((p_cnt>=2)&&ParseDomainType(p,D,1,err)))
                    pSinkVelocity(get_bool(p,0,p_cnt,err), EXPAND_DOMAIN(D));
            break;
            case PASourceID: 		   
                if (bRes=((p_cnt>=2)&&ParseDomainType(p,D,1,err)))
                    pSource(get_float(p,0,p_cnt,err), EXPAND_DOMAIN(D));
            break;
            case PASpeedLimitID: 	   
                if (bRes=(p_cnt>=1))
                    pSpeedLimit(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err,P_MAXFLOAT));
            break;
            case PATargetColorID: 	   
                if (bRes=(p_cnt==5))
                    pTargetColor(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err), get_float(p,2,p_cnt,err), get_float(p,3,p_cnt,err), get_float(p,4,p_cnt,err));
            break;
            case PATargetSizeID: 	   
                if (bRes=(p_cnt>=3))
                    pTargetSize(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err), get_float(p,2,p_cnt,err), get_float(p,3,p_cnt,err,0.0), get_float(p,4,p_cnt,err,0.0), get_float(p,5,p_cnt,err,0.0));
            break;
            case PATargetRotateID: 	   
                if (bRes=(p_cnt>=3))
                    pTargetRotate(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err), get_float(p,2,p_cnt,err), get_float(p,3,p_cnt,err,0.0));
            break;
            case PATargetRotateDID: 	   
                if (bRes=((p_cnt>=2)&&ParseDomainType(p,D,1,err)))
                    pTargetRotateD(get_float(p,0,p_cnt,err), EXPAND_DOMAIN(D));
            break;
            case PATargetVelocityID:   
                if (bRes=(p_cnt==4))
                    pTargetVelocity(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err), get_float(p,2,p_cnt,err), get_float(p,3,p_cnt,err));
            break;
            case PATargetVelocityDID:   
                if (bRes=((p_cnt>=2)&&ParseDomainType(p,D,1,err)))
                    pTargetVelocityD(get_float(p,0,p_cnt,err), EXPAND_DOMAIN(D));
            break;
            case PAVortexID: 		   
                if (bRes=(p_cnt>=6))
                    pVortex(get_float(p,0,p_cnt,err), get_float(p,1,p_cnt,err), get_float(p,2,p_cnt,err), get_float(p,3,p_cnt,err), get_float(p,4,p_cnt,err), get_float(p,5,p_cnt,err), get_float(p,6,p_cnt,err,1.0f), get_float(p,7,p_cnt,err,P_EPS), get_float(p,8,p_cnt,err,P_MAXFLOAT));
            break;
            case -1:
            	bRes = false;
		        ELog.DlgMsg(mtError,"[Error] Bad command: '%s'",cmd.c_str());
            break;
            }
            if (!bRes||err) break;
        }
        pEndActionList();
    }while (0);
    if (!bRes){ 
        ELog.DlgMsg(mtError,"[Error] Line: '%s'. \nSyntax error: '%s'.",lst[i_line].c_str(),cmd.c_str());
    }
    if (err){ 
    	bRes = false;
        int err_p = -1;
        for (int k=0; k<20; k++){
            if (err&(1<<k)){
                err_p = k;
                break;
            }
        }
        AnsiString temp;
        ELog.DlgMsg(mtError,"[ActionList Error] Line: '%s'. \nUndefined symbol #%d - '%s'.",lst[i_line].c_str(),err_p,_GetItem(p,err_p,temp));
    }
    if (!bRes){
		pNewActionList(m_HandleActionList);
        pEndActionList();
    }
}

