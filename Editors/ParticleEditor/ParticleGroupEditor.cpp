//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "xr_tokens.h"
#include "xr_trims.h"
#include "ParticleGroup.h"
#include "particles/papi.h"
#include "particles/general.h"
#include "TLSprite.h"

using namespace PAPI;
              
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
	{ "u16",		  		ptWORD				},
	{ "DWORD",		  		ptDWORD				},
	{ "u32",		  		ptDWORD				},
    { "LPSTR",				ptString			},
    { "LPCSTR",				ptString			},
	{ 0,					0				   	},
};

IC bool get_int(LPCSTR a, Variant& val)
{
    for (int k=0; a[k]; k++)
        if (!isdigit(a[k])&&(a[k]!='-')&&(a[k]!='+'))
            return false;
    val = atoi(a);
    return true;
}

IC bool get_float(LPCSTR a, Variant& val)
{
	if (strstr(a,"P_MAXFLOAT"))	{val = P_MAXFLOAT;	return true;}
	if (strstr(a,"P_EPS"))		{val = P_EPS; 		return true;}
    for (int k=0; a[k]; k++){
        if (!isdigit(a[k])&&(a[k]!='.')&&(a[k]!='-')&&(a[k]!='+')&&(a[k]!='f')&&(a[k]!='F'))
            return false;
    }
    val = atof(a);
    return true;
}

IC bool get_bool(LPCSTR a, Variant& val)
{
    bool b0 = (0==stricmp(a,"false"));
    bool b1 = (0==stricmp(a,"true"));
    if (b0==b1) return false;
    val = (BOOL)b1;
    return true;
}

IC bool get_string(LPCSTR src, Variant& val)
{
	AnsiString v;
    _GetItem(src,1,v,'"');
    val = v;
   	return src&&src[0];
}

IC int get_token_ID(xr_token* token_list, LPCSTR name){
	for( int i=0; token_list[i].name; i++ )
		if( !strcmp(name,token_list[i].name) )
			return token_list[i].id;
    return -1;
}

IC bool get_domain_type(LPCSTR src, Variant& val)
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
        Variant			data;
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
                case ptDomain:	bRes=get_domain_type(d.c_str(),P.data);	break;
                case ptBOOL:    bRes=get_bool(d.c_str(),P.data);  		break;
                case ptFloat:	bRes=get_float(d.c_str(),P.data); 		break;
                case ptInt:
                case ptWORD:
                case ptDWORD:	bRes=get_int(d.c_str(),P.data);			break;
                case ptString:	bRes=get_string(d.c_str(),P.data);		break;
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
            case ptDomain:	bRes=get_domain_type(pm.c_str(),P.data);	break;
            case ptBOOL:    bRes=get_bool(pm.c_str(),P.data);			break;
            case ptFloat:	bRes=get_float(pm.c_str(),P.data); 			break;
            case ptInt:
            case ptWORD:
            case ptDWORD:	bRes=get_int(pm.c_str(),P.data); 			break;
            case ptString:	bRes=get_string(pm.c_str(),P.data);			break;
            }
            if (!bRes){ 
            	ErrMsg(BAD_PARAM,l,k,pm.c_str());
            	break;
            }
        }
        return bRes;
    }
    void Execute()
	{   
		#define p(i) (float)params[i].data
        #define P1  p(0)
        #define P2  P1,p(1)
        #define P3  P2,p(2)
        #define P4  P3,p(3)
        #define P5  P4,p(4)
        #define P6  P5,p(5)
        #define P7  P6,p(6)
        #define P8  P7,p(7)
        #define P9  P8,p(8)
        #define P10 P9,p(9)
        #define P11 P10,p(10)
        #define P12 P11,p(11)
        #define P13 P12,p(12)
        #define P14 P13,p(13)

    	// state
		if (command==	  "pColor")				pColor			(P4);
        else if (command=="pColorD")			pColorD			(P11);
        else if (command=="pVelocity")			pVelocity		(P3);
        else if (command=="pVertexB")			pVertexB		(P3);
        else if (command=="pVertexBD")			pVertexBD		(P10);
        else if (command=="pVertexBTracks")		pVertexBTracks	(P1);
        else if (command=="pSize")				pSize			(P3);
        else if (command=="pSizeD")				pSizeD   		(P10);
        else if (command=="pRotate")			pRotate			(P3);
        else if (command=="pRotateD")			pRotateD 		(P10);
        else if (command=="pStartingAge")		pStartingAge   	(P2);
        else if (command=="pTimeStep")			pTimeStep	   	(P1);
        else if (command=="pSetMaxParticles")	pSetMaxParticles(P1);
        else if (command=="pSprite")        	parent->pSprite	(AnsiString(params[0].data).c_str(),AnsiString(params[1].data).c_str());
        else if (command=="pFrame")				parent->pFrame	(P6);
        else if (command=="pParentMotion")		pParentMotion	(P1);
        // actions
        else if (command=="pAnimate")			parent->pAnimate(P2);		
        else if (command=="pAvoid")				pAvoid			(P14);
        else if (command=="pBounce")			pBounce			(P14);
//        else if (command=="pCallActionList")	pCallActionList	();
        else if (command=="pCopyVertexB")		pCopyVertexB	(P2);
        else if (command=="pDamping")			pDamping		(P5);
        else if (command=="pExplosion")			pExplosion		(P9);
        else if (command=="pFollow")			pFollow			(P3);
        else if (command=="pGravitate")			pGravitate		(P3);
        else if (command=="pGravity")			pGravity		(P4);
        else if (command=="pJet")				pJet			(P7);
        else if (command=="pKillOld")			pKillOld		(P2);
        else if (command=="pMatchVelocity")		pMatchVelocity	(P3);
        else if (command=="pMove")				pMove			();
        else if (command=="pOrbitLine")			pOrbitLine		(P10);
        else if (command=="pOrbitPoint")		pOrbitPoint		(P7);
        else if (command=="pRandomAccel")		pRandomAccel	(P11);
        else if (command=="pRandomDisplace")	pRandomDisplace	(P11);
        else if (command=="pRandomVelocity")	pRandomVelocity	(P11);
        else if (command=="pRestore")			pRestore		(P1);
        else if (command=="pSink")				pSink			(P12);
        else if (command=="pSinkVelocity")		pSinkVelocity	(P12);
        else if (command=="pSource")			pSource			(P12);
        else if (command=="pSpeedLimit")		pSpeedLimit		(P2);
        else if (command=="pTargetColor")		pTargetColor	(P5);
        else if (command=="pTargetSize")		pTargetSize		(P6);
        else if (command=="pTargetRotate")		pTargetRotate	(P4);
        else if (command=="pTargetRotateD")		pTargetRotateD	(P11);
        else if (command=="pTargetVelocity")	pTargetVelocity	(P5);
        else if (command=="pTargetVelocityD")	pTargetVelocityD(P12);
        else if (command=="pVortex")			pVortex			(P10);
        else THROW2("Unknown Command.");
		#undef p
        #undef P1
        #undef P2
        #undef P3
        #undef P4
        #undef P5
        #undef P6
        #undef P7
        #undef P8
        #undef P9
        #undef P10
        #undef P11
        #undef P12
        #undef P13
        #undef P14
    }
};

DEFINE_MAP(AnsiString,PFunction,PFuncMap,PFuncPairIt);
DEFINE_VECTOR(PFunction,PFuncVec,PFuncIt);

static PFuncMap CommandTemplates;

static LPCSTR PStateCommands[]={
	"pResetState();",
	"pColor(float red, float green, float blue, float alpha = 1.0f);",
	"pColorD(float alpha, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pParentMotion(float scale);"
	"pRotate(float rot_x, float rot_y=0.f, float rot_z=0.f);",
	"pRotateD(PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pSize(float size_x, float size_y = 1.0f, float size_z = 1.0f);",
	"pSizeD(PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pStartingAge(float age, float sigma = 1.0f);",
	"pTimeStep(float new_dt);",
	"pVelocity(float x, float y, float z);",
	"pVertexB(float x, float y, float z);",
	"pVertexBD(PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pVertexBTracks(BOOL track_vertex = TRUE);",
	"pSetMaxParticles(int max_count);",
// our     
    "pSprite(LPCSTR sh_name, LPCSTR tex_name);",
    "pFrame(BOOL random_frame=TRUE, u32 frame_count=16, u32 texture_width=128, u32 texture_height=128, u32 frame_width=32, u32 frame_height=32);",
    0
};

static LPCSTR PActionCommands[]={
	"pAvoid(float magnitude, float epsilon, float look_ahead,  PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_transform=TRUE);",
	"pBounce(float friction, float resilience, float cutoff, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_transform=TRUE);",
	"pCopyVertexB(BOOL copy_pos = TRUE, BOOL copy_vel = FALSE);",
	"pDamping(float damping_x, float damping_y, float damping_z, float vlow = 0.0f, float vhigh = P_MAXFLOAT);",
	"pExplosion(float center_x, float center_y, float center_z, float velocity, float magnitude, float stdev, float epsilon = P_EPS, float age = 0.0f, BOOL allow_transform=TRUE);",
	"pFollow(float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);",
	"pGravitate(float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);",
	"pGravity(float dir_x, float dir_y, float dir_z, BOOL allow_transform=TRUE);",
	"pJet(float center_x, float center_y, float center_z, float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT, BOOL allow_transform=TRUE);",
	"pKillOld(float age_limit, BOOL kill_less_than = FALSE);",
	"pMatchVelocity(float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT);",
	"pMove();",
	"pOrbitLine(float p_x, float p_y, float p_z, float axis_x, float axis_y, float axis_z, float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT, BOOL allow_transform=TRUE);",
	"pOrbitPoint(float center_x, float center_y, float center_z, float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT, BOOL allow_transform=TRUE);",
	"pRandomAccel(PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_transform=TRUE);",
	"pRandomDisplace(PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_transform=TRUE);",
	"pRandomVelocity(PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_transform=TRUE);",
	"pRestore(float time);",
	"pSink(BOOL kill_inside, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_transform=TRUE);",
	"pSinkVelocity(BOOL kill_inside, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_transform=TRUE);",
	"pSource(float particle_rate, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_transform=TRUE);",
	"pSpeedLimit(float min_speed, float max_speed = P_MAXFLOAT);",
	"pTargetColor(float color_x, float color_y, float color_z, float alpha, float scale);",
	"pTargetColorD(float color_x, float color_y, float color_z, float alpha, float scale);",
	"pTargetSize(float size_x, float size_y, float size_z, float scale_x = 0.0f, float scale_y = 0.0f, float scale_z = 0.0f);",
	"pTargetRotate(float rot_x, float rot_y, float rot_z, float scale = 0.0f);",
	"pTargetRotateD(float scale, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f);",
	"pTargetVelocity(float vel_x, float vel_y, float vel_z, float scale, BOOL allow_transform=TRUE);",
	"pTargetVelocityD(float scale, PDomainEnum dtype, float a0 = 0.0f, float a1 = 0.0f, float a2 = 0.0f, float a3 = 0.0f, float a4 = 0.0f, float a5 = 0.0f, float a6 = 0.0f, float a7 = 0.0f, float a8 = 0.0f, BOOL allow_transform=TRUE);",
	"pVertex(float x, float y, float z);",
	"pVortex(float center_x, float center_y, float center_z, float axis_x, float axis_y, float axis_z, float magnitude = 1.0f, float epsilon = P_EPS, float max_radius = P_MAXFLOAT, BOOL allow_transform=TRUE);",
    // our
	"pAnimate(float speed=24.f, BOOL random_playback=FALSE);",
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

    // reset flags
	m_Flags.zero		();
    
    // parse commands
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
}

//------------------------------------------------------------------------------
// Render part
//------------------------------------------------------------------------------
void CParticleGroup::RenderEditor()
{
	// Get a pointer to the particles in gp memory
	ParticleGroup *pg = _GetGroupPtr(m_HandleGroup);

	if(pg == NULL)		return; // ERROR
	if(pg->p_count < 1)	return;

	Device.SetShader	(m_Shader);
    Device.SetTransform	(D3DTS_WORLD,Fidentity);
    CTLSprite 			m_Sprite;
    for(int i = 0; i < pg->p_count; i++){
        Particle &m = pg->list[i];

        Fvector p;
        Fcolor c;
        p.set(m.pos.x,m.pos.y,m.pos.z);
        c.set(m.color.x,m.color.y,m.color.z,m.alpha);
		if (m_Flags.is(flFramed)){
//        	||m_Flags.test(flAnimated)){
			Fvector2 lt,rb;
        	m_Animation.CalculateTC(m.frame,lt,rb);
			m_Sprite.Render(p,c.get(),m.size.x,m.rot.x,lt,rb);
        }else
			m_Sprite.Render(p,c.get(),m.size.x,m.rot.x);
//		DU::DrawCross(p,m.size.x,m.size.y,m.size.z,m.size.x,m.size.y,m.size.z,c.get(),false);
    }
}

//------------------------------------------------------------------------------
// I/O part
//------------------------------------------------------------------------------
void CParticleGroup::Save(CFS_Base& F)
{
	F.open_chunk	(PG_CHUNK_VERSION);
	F.Wword			(PG_VERSION);
	F.close_chunk	();

	F.open_chunk	(PG_CHUNK_NAME);
	F.WstringZ		(m_Name);
	F.close_chunk	();
    
	ParticleGroup *pg = _GetGroupPtr(m_HandleGroup);
	F.open_chunk	(PG_CHUNK_GROUPDATA);
    F.Wdword		(pg->max_particles); // max particles
	F.close_chunk	();
    
	F.open_chunk	(PG_CHUNK_ACTIONLIST);
	BOOL bRes		= SaveActionList(F); R_ASSERT2(bRes,"Can't save action list.");
	F.close_chunk	();

	F.write_chunk	(PG_CHUNK_FLAGS,&m_Flags,sizeof(m_Flags));

    if (m_Flags.is(flSprite)){
        F.open_chunk	(PG_CHUNK_SPRITE);
        F.WstringZ		(m_ShaderName);
        F.WstringZ		(m_TextureName);
        F.close_chunk	();
    }

    if (m_Flags.is(flFramed)){
        F.open_chunk	(PG_CHUNK_FRAME);
        F.write			(&m_Animation,sizeof(SAnimation));
        F.close_chunk	();
    }

	F.open_chunk	(PG_CHUNK_SOURCETEXT);
    F.WstringZ		(m_SourceText.c_str());
	F.close_chunk	();
}

BOOL CParticleGroup::SaveActionList(CFS_Base& F)
{
	// get pointer to specified action list.
	PAPI::PAHeader *pa	= _GetListPtr(m_HandleActionList);

	if(pa == NULL)
		return FALSE; // ERROR

	int num_actions = pa->count-1;
	PAPI::PAHeader *action= pa+1;

	// save actions
	F.open_chunk	(AL_CHUNK_VERSION);
	F.Wword			(ACTION_LIST_VERSION);
	F.close_chunk	();

	F.open_chunk	(AL_CHUNK_ACTIONS);
	F.write			(action,num_actions*sizeof(PAPI::PAHeader));
	F.close_chunk	();

	return TRUE;
}


