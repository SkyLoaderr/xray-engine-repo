//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "xr_tokens.h"
#include "xr_trims.h"
#include "ParticleGroup.h"
#include "engine/particles/papi.h"
#include "engine/particles/general.h"

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

enum PParamType{
	ptUnknown=-1,
    ptDomain=0,
    ptBOOL,
    ptFloat,
    ptInt,
    ptWORD,
    ptDWORD,
    ptString,
};
xr_token					type_token		[ ]={
	{ "PDomainEnum",		ptDomain			},
	{ "BOOL",		  		ptBOOL				},
	{ "float",		  		ptFloat				},
	{ "int",		  		ptInt				},
	{ "WORD",		  		ptWORD				},
	{ "DWORD",		  		ptDWORD				},
    { "LPSTR",				ptString			},
    { "LPCSTR",				ptString			},
	{ 0,					0				   	},
};

IC bool get_int(LPCSTR a, int& val)
{
    for (int k=0; a[k]; k++)
        if (!isdigit(a[k])&&(a[k]!='-')&&(a[k]!='+'))
            return false;
    val = atoi(a);
    return true;
}

IC bool get_float(LPCSTR a, float& val)
{
	if (strstr(a,"P_MAXFLOAT")) return P_MAXFLOAT;
	if (strstr(a,"P_EPS")) return P_EPS;
    for (int k=0; a[k]; k++){
        if (!isdigit(a[k])&&(a[k]!='.')&&(a[k]!='-')&&(a[k]!='+')&&(a[k]!='f')&&(a[k]!='F'))
            return false;
    }
    val = atof(a);
    return true;
}

IC bool get_bool(LPCSTR a, BOOL& val)
{
    bool b0 = (0==stricmp(a,"false"));
    bool b1 = (0==stricmp(a,"true"));
    if (b0==b1) return false;
    val = b1;
    return true;
}

IC bool get_string(LPCSTR src, LPSTR val)
{
    _GetItem(src,1,val,'"');
   	return src&&src[0];
}

IC int get_token_ID(xr_token* token_list, LPCSTR name){
	for( int i=0; token_list[i].name; i++ )
		if( !strcmp(name,token_list[i].name) )
			return token_list[i].id;
    return -1;
}

IC bool get_domain_type(LPCSTR src, PDomainEnum& val)
{
    val = (PDomainEnum)get_token_ID(domain_token,src);
    return val>=0;
}

// error constants
static const int BAD_COMMAND	= 0;
static const int FEW_PARAMS 	= 1;
static const int BAD_PARAM	 	= 2;

void ErrMsg(int c, int l, int v, LPCSTR line)
{
    switch (c){
    case BAD_COMMAND: 	ELog.DlgMsg(mtError,"[Error] (%d): Unrecognized command: '%s'",l+1,line); break;
    case FEW_PARAMS: 	ELog.DlgMsg(mtError,"[Error] Too few parameters in call to '%s'",line); break;
    case BAD_PARAM: 	ELog.DlgMsg(mtError,"[Error] (%d): Bad parameter '%d' - '%s'",l+1,v,line); break;
    }
}

class PFunction{
public:
	CParticleGroup*		parent;
	// command
	AnsiString 			command;
    // type
	enum Type{
    	ftState,
        ftAction
    };
    Type				type;
    // params
	struct Param{
        PParamType		type;
        AnsiString      hint;
        union{
        	struct{BOOL 		_BOOL;	};
        	struct{float 		_float; };
        	struct{int 			_int;   };
        	struct{PDomainEnum	_domain;};
        	struct{string128	_string;};
            LPVOID		data;
        };
    };
    int 				req_params;
    DEFINE_VECTOR(Param,ParamVec,ParamIt);
    ParamVec			params;
public:    
						PFunction	(){req_params=-1;parent=0;}
	bool				LoadTemplate(const AnsiString& line)
    {
        AnsiString 		pms;
        _GetItem		(line.c_str(),0,command,'(');
        _GetItem		(line.c_str(),1,pms,'(');
        _GetItem		(pms.c_str(),0,pms,')');
        bool bRes		= true;
        int p_cnt 		= _GetItemCount(pms.c_str());
        for (int k=0; k<p_cnt; k++){
	        AnsiString 	pm,t,v,d;
        	params.push_back(Param());
            Param& P	= params.back();
        	_GetItem	(pms.c_str(),k,pm);
            _GetItem	(pm.c_str(),0,t,' ');								R_ASSERT(!t.IsEmpty());
            P.type 		= (PParamType)get_token_ID(type_token,t.c_str());	
            R_ASSERT2(P.type!=ptUnknown,"Unknown PParamType!");
            _GetItems	(pm.c_str(),1,1000,v,' ');
            if (_GetItemCount(v.c_str(),'=')==2){
	            _GetItem	(v.c_str(),0,P.hint,'=');
    	        _GetItem	(v.c_str(),1,d,'=');
            }else{
	            P.hint	= v;
            }
            if (!d.IsEmpty()){
                switch(P.type){
                case ptDomain:	bRes=get_domain_type(d.c_str(),P._domain);	break;
                case ptBOOL:    bRes=get_bool(d.c_str(),P._BOOL);			break;
                case ptFloat:	bRes=get_float(d.c_str(),P._float); 		break;
                case ptInt:
                case ptWORD:
                case ptDWORD:	bRes=get_int(d.c_str(),P._int); 			break;
                case ptString:	bRes=get_string(d.c_str(),P._string);		break;
                }
            }else{
            	req_params = k+1;
            }
            if (!bRes){ 
            	ErrMsg(BAD_PARAM,0,k,d.c_str());
            	break;
            }
        }
        return bRes;
    }
    bool				Parse		(int l, const AnsiString& line)
    {
        AnsiString 		pms,pm;
        _GetItem		(line.c_str(),1,pms,'(');
        _GetItem		(pms.c_str(),0,pms,')');
        bool bRes		= true;
        int p_cnt 		= _GetItemCount(pms.c_str());
        if (p_cnt<req_params){
        	ErrMsg		(FEW_PARAMS,l,0,line.c_str());
        	return false;
        }
        for (int k=0; k<p_cnt; k++){
            Param& P	= params[k];
        	_GetItem	(pms.c_str(),k,pm);
            switch(P.type){
            case ptDomain:	bRes=get_domain_type(pm.c_str(),P._domain);	break;
            case ptBOOL:    bRes=get_bool(pm.c_str(),P._BOOL);			break;
            case ptFloat:	bRes=get_float(pm.c_str(),P._float); 		break;
            case ptInt:
            case ptWORD:
            case ptDWORD:	bRes=get_int(pm.c_str(),P._int); 			break;
            case ptString:	bRes=get_string(pm.c_str(),P._string);		break;
            }
            if (!bRes){ 
            	ErrMsg(BAD_PARAM,l,k,pm.c_str());
            	break;
            }
        }
        return bRes;
    }
    int p_num;
    #define B params[p_num=0]._BOOL
    #define F params[p_num=0]._float
    #define I params[p_num=0]._int
    #define D params[p_num=0]._domain,F,F,F,F,F,F,F,F,F
    #define S params[p_num=0]._string
    #define b params[p_num++]._BOOL
    #define f params[p_num++]._float
    #define i params[p_num++]._int
    #define d params[p_num++]._domain,F,F,F,F,F,F,F,F,F
    #define s params[p_num++]._string
    void Execute()
    {
    	// state
		if (command==	  "pColor")				pColor			(F,f,f,f);
        else if (command=="pColorD")			pColorD			(F,d);
        else if (command=="pVelocity")			pVelocity		(F,f,f);
        else if (command=="pVelocityD")			pVelocityD		(D);
        else if (command=="pVertexB")			pVertexB		(F,f,f);
        else if (command=="pVertexBD")			pVertexBD		(D);
        else if (command=="pVertexBTracks")		pVertexBTracks	(B);
        else if (command=="pSize")				pSize			(F,f,f);
        else if (command=="pSizeD")				pSizeD   		(D);
        else if (command=="pRotate")			pRotate			(F,f,f);
        else if (command=="pRotateD")			pRotateD 		(D);
        else if (command=="pStartingAge")		pStartingAge   	(F,f);
        else if (command=="pTimeStep")			pTimeStep	   	(F);
        else if (command=="pSetMaxParticles")	pSetMaxParticles(F);
        else if (command=="pSprite")			parent->pSprite	(S,s);
        else if (command=="pFrameSet")			parent->pFrameSet(I,i,i,i);
        // actions
        else if (command=="pAnimate")			pAnimate		(B,b,b,i,f);		
        else if (command=="pAvoid")				pAvoid			(F,f,f,d);
        else if (command=="pBounce")			pBounce			(F,f,f,d);
//        else if (command=="pCallActionList")	pCallActionList	();
        else if (command=="pCopyVertexB")		pCopyVertexB	(B,b);
        else if (command=="pDamping")			pDamping		(F,f,f,f,f);
        else if (command=="pExplosion")			pExplosion		(F,f,f,f,f,f,f,f);
        else if (command=="pFollow")			pFollow			(F,f,f);
        else if (command=="pGravitate")			pGravitate		(F,f,f);
        else if (command=="pGravity")			pGravity		(F,f,f);
        else if (command=="pJet")				pJet			(F,f,f,f,f,f);
        else if (command=="pKillOld")			pKillOld		(F,b);
        else if (command=="pMatchVelocity")		pMatchVelocity	(F,f,f);
        else if (command=="pMove")				pMove			();
        else if (command=="pOrbitLine")			pOrbitLine		(F,f,f,f,f,f,f,f,f);
        else if (command=="pOrbitPoint")		pOrbitPoint		(F,f,f,f,f,f);
        else if (command=="pRandomAccel")		pRandomAccel	(D);
        else if (command=="pRandomDisplace")	pRandomDisplace	(D);
        else if (command=="pRandomVelocity")	pRandomVelocity	(D);
        else if (command=="pRestore")			pRestore		(F);
        else if (command=="pSink")				pSink			(B,d);
        else if (command=="pSinkVelocity")		pSinkVelocity	(B,d);
        else if (command=="pSource")			pSource			(F,d);
        else if (command=="pSpeedLimit")		pSpeedLimit		(F,f);
        else if (command=="pTargetColor")		pTargetColor	(F,f,f,f,f);
        else if (command=="pTargetSize")		pTargetSize		(F,f,f,f,f,f);
        else if (command=="pTargetRotate")		pTargetRotate	(F,f,f,f);
        else if (command=="pTargetRotateD")		pTargetRotateD	(F,d);
        else if (command=="pTargetVelocity")	pTargetVelocity	(F,f,f,f);
        else if (command=="pTargetVelocityD")	pTargetVelocityD(F,d);
        else if (command=="pVortex")			pVortex			(F,f,f,f,f,f,f,f,f);
    }
    #undef B
    #undef F
    #undef I
    #undef D
    #undef S
    #undef b
    #undef f
    #undef i
    #undef d
    #undef s
};

DEFINE_MAP(AnsiString,PFunction,PFuncMap,PFuncPairIt);
DEFINE_VECTOR(PFunction,PFuncVec,PFuncIt);

static PFuncMap CommandTemplates;

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
// our     
    "pSprite(LPCSTR sh_name, LPCSTR tex_name);",
    "pFrameSet(WORD texture_width=128, WORD texture_height=128, WORD frame_width=32, WORD frame_height=32);",
    0
};

static LPCSTR PActionCommands[]={
	"pAnimate(BOOL animated=FALSE, BOOL random_frame=FALSE, BOOL random_playback=FALSE, WORD frame_count=16, float speed=24.f);",
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
	"pVortex(float center_x, float center_y, float center_z, float axis_x, float axis_y, float axis_z, float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);",
    0
};

bool InitCommandTemplates(PFuncMap& ct)
{
    bool bRes=true;
    do{
    	int k;
        // load state commands
        for (k=0; PStateCommands[k]; k++){
            PFunction F; F.type = PFunction::ftState;
            if (!(bRes=F.LoadTemplate(PStateCommands[k]))) break;
            ct[F.command] = F;
        }
	    if (!bRes) break;
		// load action commands
        for (k=0; PActionCommands[k]; k++){
            PFunction F; F.type = PFunction::ftAction;
            if (!(bRes=F.LoadTemplate(PActionCommands[k]))) break;
            ct[F.command] = F;
        }
    }while(0);
    if (!bRes) ct.clear();
    return bRes;
}

void CParticleGroup::ParseCommandList(LPCSTR txt)
{
	// load templates
	if (CommandTemplates.empty()) R_ASSERT(InitCommandTemplates(CommandTemplates));

    // parse
    AStringVec 			lst;
    SequenceToList		(lst,txt,';');
    
	static PFuncVec 	Commands;
    Commands.clear		();
    bool bRes=true;
	for (int i_line=0; i_line<(int)lst.size(); i_line++){
		AStringIt it	= lst.begin() + i_line;
        AnsiString		command;
        _GetItem		(it->c_str(),0,command,'(');
        // comment
        if (1==command.Pos("//")) continue;
        // find command in templates
    	PFuncPairIt pfp_it = CommandTemplates.find(command);
        if (pfp_it==CommandTemplates.end()){
        	ErrMsg		(BAD_COMMAND,i_line,0,it->c_str());
            bRes		= false;
            break;
        }
        Commands.push_back(PFunction());
		PFunction& F	= Commands.back();
        F				= pfp_it->second;
        F.parent		= this;
        // parse params
        if (!(bRes=F.Parse(i_line,it->c_str()))) break;
    }
    if (!bRes) return;
    // execute commands
    PFuncIt pf_it;
    // at first state commands
    for (pf_it=Commands.begin(); pf_it!=Commands.end(); pf_it++)
    	if (PFunction::ftState==pf_it->type) pf_it->Execute();             
    // at next action commands
	pNewActionList	(m_HandleActionList);
    for (pf_it=Commands.begin(); pf_it!=Commands.end(); pf_it++)
    	if (PFunction::ftAction==pf_it->type) pf_it->Execute();             
	pEndActionList	();
/*    

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
*/
}

