//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleEffect.h"

using namespace PAPI;
using namespace PS;
              
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

union var
{
    PDomainEnum		d;
    float   		f;
    int             i;
    BOOL    		b;
    WORD			w;
    DWORD			o;
    string128		s;

    operator PDomainEnum()	{return d;}
    operator float		()	{return f;}
    operator int		()	{return i;}
    operator WORD		()	{return w;}
    operator DWORD		()	{return o;}
    operator string128&	()	{return s;}

    var& operator 		= (PDomainEnum _d) 	{ d=_d; return *this; }
    var& operator 		= (float _f)      	{ f=_f; return *this; }
    var& operator 		= (int _i)        	{ i=_i; return *this; }
    var& operator 		= (WORD _w)       	{ w=_w; return *this; }
    var& operator 		= (DWORD _o)		{ o=_o; return *this; }
    var& operator 		= (string128& _s)	{ strcpy(s,_s); return *this; }

    var					()               	{ }
    var					(PDomainEnum _d): d(_d)   { }
    var					(float _f)		: f(_f)   { }
    var					(int _i)  		: i(_i)   { }
    var					(WORD _w) 		: w(_w)   { }
    var					(DWORD _o)		: o(_o)   { }
    var					(string128& _s)	{ strcpy(s,_s); }
};

IC bool get_int(LPCSTR a, var& val)
{
    for (int k=0; a[k]; k++)
        if (!isdigit(a[k])&&(a[k]!='-')&&(a[k]!='+'))
            return false;
    val = atoi(a);
    return true;
}

IC bool get_float(LPCSTR a, var& val)
{
	if (strstr(a,"P_MAXFLOAT"))	{val = P_MAXFLOAT;	return true;}
	if (strstr(a,"P_EPS"))		{val = P_EPS; 		return true;}
    for (int k=0; a[k]; k++){
        if (!isdigit(a[k])&&(a[k]!='.')&&(a[k]!='-')&&(a[k]!='+')&&(a[k]!='f')&&(a[k]!='F'))
            return false;
    }
    val = (float)atof(a);
    return true;
}

IC bool get_bool(LPCSTR a, var& val)
{
    bool b0 = (0==stricmp(a,"false"));
    bool b1 = (0==stricmp(a,"true"));
    if (b0==b1) return false;
    val = (BOOL)b1;
    return true;
}

IC bool get_string(LPCSTR src, var& val)
{
	string128 v;
    _GetItem(src,1,v,'"');
    val 	= v;
   	return src&&src[0];
}

IC int get_token_ID(xr_token* token_list, LPCSTR name){
	for( int i=0; token_list[i].name; i++ )
		if( !strcmp(name,token_list[i].name) )
			return token_list[i].id;
    return -1;
}

IC bool get_domain_type(LPCSTR src, var& val)
{
	val = (PDomainEnum)get_token_ID(domain_token,src);
    return (int)val>=0;
}

// error constants
static const int BAD_COMMAND	= 0;
static const int FEW_PARAMS 	= 1;
static const int MANY_PARAMS 	= 2;
static const int BAD_PARAM	 	= 3;
static const int STA_MISSING 	= 4;

void ErrMsg(int c, int l, int v, LPCSTR line)
{
    switch (c){
    case BAD_COMMAND: 	ELog.DlgMsg(mtError,"[Error] (%d): Unrecognized command: '%s'",l+1,line); break;
    case FEW_PARAMS: 	ELog.DlgMsg(mtError,"[Error] Too few parameters in call to '%s'",line); break;
    case MANY_PARAMS: 	ELog.DlgMsg(mtError,"[Error] Extra parameters in call to '%s'",line); break;
    case BAD_PARAM: 	ELog.DlgMsg(mtError,"[Error] (%d): Bad parameter '%d' - '%s'",l+1,v,line); break;
    case STA_MISSING:	ELog.DlgMsg(mtError,"[Error] (%d): Statement missing ';'",l+1,line); break;
    }
}

namespace PS{
class PFunction{
public:
	CPEDef*				parent;
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
        AnsiString	    hint;
        AnsiString		s_data;
        var				data;
        Param(){hint="";s_data="";data=0;}
    };
    
    int 				req_params;
    DEFINE_VECTOR(Param,ParamVec,ParamIt);
    ParamVec			params;

    LPCSTR				src;
public:    
						PFunction	(){req_params=-1;parent=0;}
	bool				LoadTemplate(LPCSTR line)
    {
    	src				= line;
        AnsiString 		pms;
        _GetItem		(line,0,command,'(');
        _GetItem		(line,1,pms,'(');
        _GetItem		(pms.c_str(),0,pms,')');
        bool bRes		= true;
        int p_cnt 		= _GetItemCount(pms.c_str());
        for (int k=0; k<p_cnt; k++){
	        AnsiString 	pm,t,v;
        	params.push_back(Param());
            Param& P	= params.back();
        	_GetItem	(pms.c_str(),k,pm);
            _GetItem	(pm.c_str(),0,t,' ');	R_ASSERT(!t.IsEmpty());
            P.type 		= (PParamType)get_token_ID(type_token,t.c_str());	
            R_ASSERT2(P.type!=ptUnknown,"Unknown PParamType!");
            _GetItems	(pm.c_str(),1,1000,v,' ');
            if (_GetItemCount(v.c_str(),'=')==2){
	            _GetItem(v.c_str(),0,P.hint,'=');
    	        _GetItem(v.c_str(),1,P.s_data,'=');
            }else{
	            P.hint	= v;
            }
            if (!P.s_data.IsEmpty()){
                switch(P.type){
                case ptDomain:	bRes=get_domain_type(P.s_data.c_str(),P.data);	break;
                case ptBOOL:    bRes=get_bool(P.s_data.c_str(),P.data);  		break;
                case ptFloat:	bRes=get_float(P.s_data.c_str(),P.data); 		break;
                case ptInt:
                case ptWORD:
                case ptDWORD:	bRes=get_int(P.s_data.c_str(),P.data);			break;
                case ptString:	bRes=get_string(P.s_data.c_str(),P.data);		break;
                }
            }else{
            	req_params = k+1;
            }
            if (!bRes){ 
            	ErrMsg(BAD_PARAM,0,k,P.s_data.c_str());
            	break;
            }
        }
        return bRes;
    }
    bool				Parse		(int l, LPCSTR line)
    {
        AnsiString 		pms,pm;
        _GetItem		(line,1,pms,'(');
        _GetItem		(pms.c_str(),0,pms,')');
        bool bRes		= true;
        int p_cnt 		= _GetItemCount(pms.c_str());
        if (p_cnt>params.size()){
        	ErrMsg		(MANY_PARAMS,l,0,line);
        	return false;
        }
        if (p_cnt<req_params){
        	ErrMsg		(FEW_PARAMS,l,0,line);
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
		#define p(i) params[i].data
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
        #define P15 P14,p(14)

    	// state
		if 		(command=="pColor")	   			pColor			(P4);
        else if (command=="pColorD")			pColorD			(P11);
        else if (command=="pVelocity")	 		pVelocity		(P3);
		else if (command=="pVelocityD")			pVelocityD		(P10);
        else if (command=="pVertexB")			pVertexB		(P3);
        else if (command=="pVertexBD")	  		pVertexBD		(P10);
        else if (command=="pVertexBTracks") 	pVertexBTracks	(P1);
        else if (command=="pSize")		  		pSize			(P3);
        else if (command=="pSizeD")		  		pSizeD   		(P10);
        else if (command=="pRotate")			pRotate			(P3);
        else if (command=="pRotateD")			pRotateD 		(P10);
        else if (command=="pStartingAge")		pStartingAge   	(P2);
        else if (command=="pTimeStep")	  		pTimeStep	   	(P1);
        else if (command=="pSetMaxParticles")	pSetMaxParticles(P1);
//        else if (command=="pObject")       		parent->pObject	(P1);
		else if (command=="pAlignToPath")		parent->pAlignToPath();
		else if (command=="pCollision")			parent->pCollision(P4);
        else if (command=="pSprite")        	parent->pSprite	(P2);
        else if (command=="pFrame")		  		parent->pFrame	(P6);
        else if (command=="pTimeLimit")			parent->pTimeLimit(P1);
        else if (command=="pParentMotion")  	pParentMotion	(P1);
        // actions
        else if (command=="pAnimate")			parent->pAnimate(P2);		
        else if (command=="pAvoid")		  		pAvoid			(P15);
        else if (command=="pBounce")			pBounce			(P15);
        else if (command=="pCopyVertexB")	  	pCopyVertexB	(P2);
        else if (command=="pDamping")			pDamping		(P5);
        else if (command=="pExplosion")			pExplosion		(P10);
        else if (command=="pFollow")			pFollow			(P3);
        else if (command=="pGravitate")			pGravitate		(P3);
        else if (command=="pGravity")			pGravity		(P5);
        else if (command=="pJet")				pJet			(P8);
        else if (command=="pKillOld")			pKillOld		(P2);
        else if (command=="pMatchVelocity")		pMatchVelocity	(P3);
        else if (command=="pMove")				pMove			();
        else if (command=="pOrbitLine")			pOrbitLine		(P11);
        else if (command=="pOrbitPoint")		pOrbitPoint		(P8);
        else if (command=="pRandomAccel")		pRandomAccel	(P12);
        else if (command=="pRandomDisplace")	pRandomDisplace	(P12);
        else if (command=="pRandomVelocity")	pRandomVelocity	(P12);
        else if (command=="pRestore")			pRestore		(P1);
        else if (command=="pSink")				pSink			(P13);
        else if (command=="pSinkVelocity")		pSinkVelocity	(P13);
        else if (command=="pSource")			pSource			(P13);
        else if (command=="pSpeedLimit")		pSpeedLimit		(P2);
        else if (command=="pTargetColor")		pTargetColor	(P5);
        else if (command=="pTargetSize")		pTargetSize		(P6);
        else if (command=="pTargetRotate")		pTargetRotate	(P4);
        else if (command=="pTargetRotateD")		pTargetRotateD	(P11);
        else if (command=="pTargetVelocity")	pTargetVelocity	(P6);
        else if (command=="pTargetVelocityD")	pTargetVelocityD(P13);
        else if (command=="pVortex")			pVortex			(P11);
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
        #undef P15
    }
};
}

DEFINE_MAP(AnsiString,PFunction,PFuncMap,PFuncPairIt);
DEFINE_VECTOR(PFunction,PFuncVec,PFuncIt);

static PFuncMap CommandTemplates;

static LPCSTR PStateCommands[]={
//	"pResetState();",
	"pColor(float red, float green, float blue, float alpha=1.0f);",
	"pColorD(float alpha, PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f);",
	"pParentMotion(float scale);",
	"pRotate(float rot_x, float rot_y=0.f, float rot_z=0.f);",
	"pRotateD(PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f);",
	"pSize(float size_x, float size_y=1.0f, float size_z=1.0f);",
	"pSizeD(PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f);",
	"pStartingAge(float age, float sigma=1.0f);",
	"pTimeStep(float new_dt);",
	"pVelocity(float x, float y, float z);",
	"pVelocityD(PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f);",
	"pVertexB(float x, float y, float z);",
	"pVertexBD(PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f);",
	"pVertexBTracks(BOOL track_vertex=TRUE);",
	"pSetMaxParticles(int max_count);",
// our     
	"pAlignToPath();",
    "pCollision(float friction, float resilience, float cutoff, BOOL destroy_on_contact=FALSE);",
//    "pObject(LPCSTR obj_name);",
    "pSprite(LPCSTR sh_name, LPCSTR tex_name);",
    "pFrame(BOOL random_frame=TRUE, u32 frame_count=16, u32 texture_width=128, u32 texture_height=128, u32 frame_width=32, u32 frame_height=32);",
	"pTimeLimit(float time_limit);",
    0
};

static LPCSTR PActionCommands[]={
	"pAvoid(float magnitude, float epsilon, float look_ahead,  PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pBounce(float friction, float resilience, float cutoff, PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pCopyVertexB(BOOL copy_pos=TRUE, BOOL copy_vel=FALSE);",
	"pDamping(float damping_x, float damping_y, float damping_z, float vlow=0.0f, float vhigh=P_MAXFLOAT);",
	"pExplosion(float center_x, float center_y, float center_z, float velocity, float magnitude, float stdev, float epsilon=P_EPS, float age=0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pFollow(float magnitude=1.0f, float epsilon=P_EPS, float max_radius=P_MAXFLOAT);",
	"pGravitate(float magnitude=1.0f, float epsilon=P_EPS, float max_radius=P_MAXFLOAT);",
	"pGravity(float dir_x, float dir_y, float dir_z, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pJet(float center_x, float center_y, float center_z, float magnitude=1.0f, float epsilon=P_EPS, float max_radius=P_MAXFLOAT, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pKillOld(float age_limit, BOOL kill_less_than=FALSE);",
	"pMatchVelocity(float magnitude=1.0f, float epsilon=P_EPS, float max_radius=P_MAXFLOAT);",
	"pMove();",
	"pOrbitLine(float p_x, float p_y, float p_z, float axis_x, float axis_y, float axis_z, float magnitude=1.0f, float epsilon=P_EPS, float max_radius=P_MAXFLOAT, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pOrbitPoint(float center_x, float center_y, float center_z, float magnitude=1.0f, float epsilon=P_EPS, float max_radius=P_MAXFLOAT, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pRandomAccel(PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pRandomDisplace(PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pRandomVelocity(PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pRestore(float time);",
	"pSink(BOOL kill_inside, PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pSinkVelocity(BOOL kill_inside, PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pSource(float particle_rate, PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pSpeedLimit(float min_speed, float max_speed=P_MAXFLOAT);",
	"pTargetColor(float color_x, float color_y, float color_z, float alpha, float scale);",
	"pTargetSize(float size_x, float size_y, float size_z, float scale_x=0.0f, float scale_y=0.0f, float scale_z=0.0f);",
	"pTargetRotate(float rot_x, float rot_y, float rot_z, float scale=0.0f);",
	"pTargetRotateD(float scale, PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f);",
	"pTargetVelocity(float vel_x, float vel_y, float vel_z, float scale, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pTargetVelocityD(float scale, PDomainEnum dtype, float a0=0.0f, float a1=0.0f, float a2=0.0f, float a3=0.0f, float a4=0.0f, float a5=0.0f, float a6=0.0f, float a7=0.0f, float a8=0.0f, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
	"pVertex(float x, float y, float z);",
	"pVortex(float center_x, float center_y, float center_z, float axis_x, float axis_y, float axis_z, float magnitude=1.0f, float epsilon=P_EPS, float max_radius=P_MAXFLOAT, BOOL allow_translate=TRUE, BOOL allow_rotate=TRUE);",
    // our
	"pAnimate(float speed=24.f, BOOL random_playback=FALSE);",
    0
};

bool InitCommandTemplates()
{
	PFuncMap& ct = CommandTemplates;
    bool bRes=true;
    do{
    	int k;
        // load state commands
        for (k=0; PStateCommands[k]; k++){
            PS::PFunction F; F.type = PFunction::ftState;
            if (!(bRes=F.LoadTemplate(PStateCommands[k]))) break;
            ct.insert(mk_pair(F.command,F));
        }
	    if (!bRes) break;
		// load action commands
        for (k=0; PActionCommands[k]; k++){
            PFunction F; F.type = PS::PFunction::ftAction;
            if (!(bRes=F.LoadTemplate(PActionCommands[k]))) break;
            ct.insert(mk_pair(F.command,F));
        }
    }while(0);
    if (!bRes) ct.clear();
    return bRes;
}

#include <Menus.hpp>
const AnsiString GetFunctionTemplate(const AnsiString& command)
{
	LPCSTR dest=0;
	PFunction* F 	= CPEDef::FindCommandPrototype(command.c_str(),dest);
	AnsiString text	= "";
    if (F){
        text.sprintf("%-16s(",F->command);
        if (!F->params.empty()){
            for (PFunction::ParamIt it=F->params.begin(); it!=F->params.end(); it++){
                text	+= it->s_data;
                text	+= ", ";
            }
            text.Delete	(text.Length()-1,2);
        }
        text		+= ");";
    }
    return text;
}

void FillStateMenu(TMenuItem* root, TNotifyEvent on_click)
{
	// load templates
	if (CommandTemplates.empty()) R_ASSERT(InitCommandTemplates());

    for (PFuncPairIt b=CommandTemplates.begin(); b!=CommandTemplates.end(); b++){
    	if (b->second.type==PFunction::ftState){
	        TMenuItem* mi 	= xr_new<TMenuItem>((TComponent*)0);
    	    mi->Caption 	= b->second.command;
        	mi->OnClick 	= on_click;
			root->Insert	(root->Count,mi);
        }
    }
}
void FillActionMenu(TMenuItem* root, TNotifyEvent on_click)
{
	// load templates
	if (CommandTemplates.empty()) R_ASSERT(InitCommandTemplates());

    for (PFuncPairIt b=CommandTemplates.begin(); b!=CommandTemplates.end(); b++){
    	if (b->second.type==PFunction::ftAction){
	        TMenuItem* mi 	= xr_new<TMenuItem>((TComponent*)0);
    	    mi->Caption 	= b->second.command;
        	mi->OnClick 	= on_click;
			root->Insert	(root->Count,mi);
        }
    }
}

PFunction* CPEDef::FindCommandPrototype(LPCSTR src, LPCSTR& dest)
{
	// load templates
	if (CommandTemplates.empty()) R_ASSERT(InitCommandTemplates());

    AnsiString		command;
    _GetItem		(src,0,command,'(');
    // comment
    if (command==strstr(command.c_str(),"//")){ dest=0; return 0; }
    // find command in templates
    PFuncPairIt pfp_it = CommandTemplates.find(command);
    if (pfp_it==CommandTemplates.end())	return 0;
    else{								
    	dest = pfp_it->second.src;
    	return &pfp_it->second;
    }
}

void CPEDef::Compile()
{
	// load templates
	if (CommandTemplates.empty()) R_ASSERT(InitCommandTemplates());

    // parse
    LPSTRVec 			lst;
    _SequenceToList		(lst,m_SourceText.c_str(),'\r');  // 0x0d 0x0a \n \r

    // reset flags
	m_Flags.zero		();
    
    // parse commands
	static PFuncVec 	Commands;
    Commands.clear		();
    bool bRes=true;
	for (int i_line=0; i_line<(int)lst.size(); i_line++){
		LPSTRIt it		= lst.begin() + i_line;
        AnsiString		command,line;
        _GetItem		(*it,0,command,'(');
        // comment
        if (command==strstr(command.c_str(),"//")) continue;
        // check ';' found
        _GetItem		(*it,1,line,')');
        if (line!=strstr(line.c_str(),";")){
        	ErrMsg		(STA_MISSING,i_line,0,*it);
            bRes		= false;
            break;
        }
        // find command in templates
    	PFuncPairIt pfp_it = CommandTemplates.find(command.c_str());
        if (pfp_it==CommandTemplates.end()){
        	ErrMsg		(BAD_COMMAND,i_line,0,*it);
            bRes		= false;
            break;
        }
        Commands.push_back(PFunction());
		PFunction& F	= Commands.back();
        F				= pfp_it->second;
        F.parent		= this;
        // parse params
        if (!(bRes=F.Parse(i_line,*it))) break;
    }
    // free list
    for (LPSTRIt it=lst.begin(); it!=lst.end(); it++)
    	xr_free(*it);

    if (!bRes) return;

    // destroy shader (may be changed)
    m_CachedShader.destroy	();
    
    // create temporary handles
	int effect_handle 		= pGenParticleEffects(1, 1);
    int action_list_handle	= pGenActionLists();
	pCurrentEffect			(effect_handle);

    // reset state (одинаковые начальные условия)
    pResetState				();
    
    // execute commands
    PFuncIt pf_it;
    // at first state commands
    for (pf_it=Commands.begin(); pf_it!=Commands.end(); pf_it++)
    	if (PFunction::ftState==pf_it->type) pf_it->Execute();             
    // at next action commands
	pNewActionList			(action_list_handle);
    for (pf_it=Commands.begin(); pf_it!=Commands.end(); pf_it++)
    	if (PFunction::ftAction==pf_it->type) pf_it->Execute();             
	pEndActionList			();

    // save effect data
	ParticleEffect *pe 		= _GetEffectPtr(effect_handle); R_ASSERT(pe);
    m_MaxParticles			= pe->max_particles;
    
    // save action list
	PAPI::PAHeader *pa		= _GetListPtr(action_list_handle);	R_ASSERT(pa);
	m_ActionCount	 		= pa->count-1;
	PAPI::PAHeader *action	= pa+1;
    xr_free					(m_ActionList);
    m_ActionList			= xr_alloc<PAPI::PAHeader>(m_ActionCount);
    Memory.mem_copy			(m_ActionList,action,m_ActionCount*sizeof(PAPI::PAHeader));

    // destroy temporary handls
	pDeleteParticleEffects	(effect_handle);
	pDeleteActionLists		(action_list_handle);

    if (m_ShaderName&&m_ShaderName[0]&&m_TextureName&&m_TextureName[0])
	    m_CachedShader.create(m_ShaderName,m_TextureName);
}


