//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleEffectActions.h"
#include "folderlib.h"
#include "PropertiesListHelper.h"
#include "particle_actions_collection.h"

using namespace PAPI; 
#define PARTICLE_ACTION_VERSION		0x0000
//---------------------------------------------------------------------------
xr_token					actions_token 			[ ]={
    { "Avoid",				PAAvoidID				},        
    { "Bounce",				PABounceID				},        
    { "Copy VertexB",		PACopyVertexBID			},        
    { "Damping",			PADampingID				},        
    { "Explosion",			PAExplosionID			},        
    { "Follow",				PAFollowID				},        
    { "Gravitate",			PAGravitateID			},        
    { "Gravity",			PAGravityID				},        
    { "Jet",				PAJetID					},        
    { "Kill Old",			PAKillOldID				},        
    { "Match Velocity",		PAMatchVelocityID		},        
    { "Move",				PAMoveID				},        
    { "Orbit Line",			PAOrbitLineID			},        
    { "Orbit Point",		PAOrbitPointID			},        
    { "Random Accel",		PARandomAccelID			},        
    { "Random Displace",	PARandomDisplaceID		},        
    { "Random Velocity",	PARandomVelocityID		},        
    { "Restore",			PARestoreID				},        
    { "Sink",				PASinkID				},        
    { "Sink Velocity",		PASinkVelocityID		},        
    { "Source",				PASourceID				},        
    { "Speed Limit",		PASpeedLimitID			},        
    { "Target Color",		PATargetColorID			},        
    { "Target Size",		PATargetSizeID			},        
    { "Target Rotate",		PATargetRotateID		},        
    { "Target Velocity",	PATargetVelocityID		},        
    { "Vortex",				PAVortexID				},        
    { 0,					0				  	 	}
};

EParticleAction* pCreateEAction(PAPI::PActionEnum type)
{
	EParticleAction* pa	= 0;
    switch(type){
    case PAPI::PAAvoidID:			pa = xr_new<EPAAvoid>			();	break;
    case PAPI::PABounceID:    		pa = xr_new<EPABounce>			();	break;
    case PAPI::PACopyVertexBID:    	pa = xr_new<EPACopyVertexB>		();	break;
    case PAPI::PADampingID:    		pa = xr_new<EPADamping>			();	break;
    case PAPI::PAExplosionID:    	pa = xr_new<EPAExplosion>		();	break;
    case PAPI::PAFollowID:    		pa = xr_new<EPAFollow>			();	break;
    case PAPI::PAGravitateID:    	pa = xr_new<EPAGravitate>		();	break;
    case PAPI::PAGravityID:    		pa = xr_new<EPAGravity>			();	break;
    case PAPI::PAJetID:    			pa = xr_new<EPAJet>				();	break;
    case PAPI::PAKillOldID:    		pa = xr_new<EPAKillOld>			();	break;
    case PAPI::PAMatchVelocityID:   pa = xr_new<EPAMatchVelocity>	();	break;
    case PAPI::PAMoveID:    		pa = xr_new<EPAMove>		   	();	break;
    case PAPI::PAOrbitLineID:    	pa = xr_new<EPAOrbitLine>		();	break;
    case PAPI::PAOrbitPointID:    	pa = xr_new<EPAOrbitPoint>		();	break;
    case PAPI::PARandomAccelID:    	pa = xr_new<EPARandomAccel>		();	break;
    case PAPI::PARandomDisplaceID:  pa = xr_new<EPARandomDisplace>	();	break;
    case PAPI::PARandomVelocityID:  pa = xr_new<EPARandomVelocity>	();	break;
    case PAPI::PARestoreID:    		pa = xr_new<EPARestore>			();	break;
    case PAPI::PASinkID:    		pa = xr_new<EPASink>		   	();	break;
    case PAPI::PASinkVelocityID:    pa = xr_new<EPASinkVelocity>   	();	break;
    case PAPI::PASourceID:    		pa = xr_new<EPASource>			();	break;
    case PAPI::PASpeedLimitID:    	pa = xr_new<EPASpeedLimit>		();	break;
    case PAPI::PATargetColorID:    	pa = xr_new<EPATargetColor>		();	break;
    case PAPI::PATargetSizeID:    	pa = xr_new<EPATargetSize>		();	break;
    case PAPI::PATargetRotateID:    pa = xr_new<EPATargetRotate> 	();	break;
    case PAPI::PATargetRotateDID:   pa = xr_new<EPATargetRotate> 	();	break;
    case PAPI::PATargetVelocityID:	pa = xr_new<EPATargetVelocity>	();	break;
    case PAPI::PATargetVelocityDID: pa = xr_new<EPATargetVelocity>	();	break;
    case PAPI::PAVortexID:    		pa = xr_new<EPAVortex>			();	break;
    default: NODEFAULT;
    }
    pa->type						= type;
	return pa;
}
//---------------------------------------------------------------------------
void 	EParticleAction::Render		()
{
	if (flags.is(flDrawDomain)&&flags.is(flEnabled)){
    	u32 clr						= 0xffffffff;
		for (PDomainMapIt it=domains.begin(); it!=domains.end(); it++)
        	it->second.Render		(clr);
    }
}
void 	EParticleAction::Load		(IReader& F)
{
	u32 vers		= F.r_u32();
    R_ASSERT		(vers==PARTICLE_ACTION_VERSION);
	flags.set		(F.r_u32());
    for (PFloatMapIt 	f_it=floats.begin(); 	f_it!=floats.end(); 	f_it++)	f_it->second.val	= F.r_float();
    for (PVectorMapIt 	v_it=vectors.begin();	v_it!=vectors.end(); 	v_it++)	F.r_fvector3(v_it->second.val);
    for (PDomainMapIt 	d_it=domains.begin(); 	d_it!=domains.end(); 	d_it++)	F.r			(&(d_it->second),sizeof(PDomain));
    for (PBoolMapIt 	b_it=bools.begin();  	b_it!=bools.end(); 		b_it++)	b_it->second.val	= F.r_u8();
}
void 	EParticleAction::Save		(IWriter& F)
{
	F.w_u32			(PARTICLE_ACTION_VERSION);
	F.w_u32			(flags.get());
    for (PFloatMapIt 	f_it=floats.begin(); 	f_it!=floats.end(); 	f_it++)	F.w_float	(f_it->second.val);
    for (PVectorMapIt 	v_it=vectors.begin(); 	v_it!=vectors.end(); 	v_it++)	F.w_fvector3(v_it->second.val);
    for (PDomainMapIt 	d_it=domains.begin(); 	d_it!=domains.end(); 	d_it++)	F.w			(&(d_it->second),sizeof(PDomain));
    for (PBoolMapIt 	b_it=bools.begin(); 	b_it!=bools.end(); 		b_it++)	F.w_u8		(b_it->second.val);
}
void 	EParticleAction::FillProp	(PropItemVec& items, LPCSTR pref)
{
    for (OrderVecIt o_it=orders.begin(); o_it!=orders.end(); o_it++)
    {
    	LPCSTR name 				= o_it->name.c_str();
		switch (o_it->type){
        case tpDomain: 
            domains[o_it->name].FillProp(items, FHelper.PrepareKey(pref,name).c_str());
        break;
        case tpVector:{ 
        	PVector& vect = vectors[o_it->name];
        	switch (vect.type){
            case PVector::vNum: 	
				PHelper.CreateVector	(items,	FHelper.PrepareKey(pref,name).c_str(), &vect.val, vect.mn, vect.mx, 0.001f, 3);            
			break;
            case PVector::vAngle: 	
				PHelper.CreateAngle3	(items,	FHelper.PrepareKey(pref,name).c_str(), &vect.val, vect.mn, vect.mx, 0.001f, 3);            
            break;
            case PVector::vColor: 	
				PHelper.CreateVColor	(items,	FHelper.PrepareKey(pref,name).c_str(), &vect.val);
            break;
            }
        }break;
        case tpFloat:{
        	PFloat& flt	= floats[o_it->name];
            PHelper.CreateFloat			(items,	FHelper.PrepareKey(pref,name).c_str(), &flt.val, flt.mn, flt.mx, 0.001f, 3);
        }break;
        case tpBool: 
            PHelper.CreateBOOL			(items,	FHelper.PrepareKey(pref,name).c_str(), &bools[o_it->name].val);
        break;
        }
    }
    PHelper.CreateFlag<Flags32>		(items,	FHelper.PrepareKey(pref,"Enabled").c_str(), 		&flags, flEnabled);
    if (!domains.empty())
    	PHelper.CreateFlag<Flags32>	(items,	FHelper.PrepareKey(pref,"Draw Domains").c_str(), 	&flags, flDrawDomain);
}
void EParticleAction::appendFloat	(LPCSTR name, float v, float mn, float mx)
{
	orders.push_back				(SOrder(tpFloat,name));
	floats[name]					= PFloat(v,mn,mx);
}
void EParticleAction::appendVector	(LPCSTR name, PVector::EType type, float vx, float vy, float vz, float mn, float mx)
{
	orders.push_back				(SOrder(tpVector,name));
	vectors[name]					= PVector(type,Fvector().set(vx,vy,vz),mn,mx);
}
void EParticleAction::appendDomain	(LPCSTR name, PDomain v)
{
	orders.push_back				(SOrder(tpDomain,name));
	domains[name]					= v;
}

void EParticleAction::appendBool	(LPCSTR name, BOOL v)
{
	orders.push_back				(SOrder(tpBool,name));
	bools[name]						= PBool(v);
}

//---------------------------------------------------------------------------
void pAvoid(IWriter& F, float magnitude, float epsilon, float look_ahead, pDomain D, BOOL allow_rotate)
{
	PAAvoid 		S;

	S.positionL		= D;
	S.position		= S.positionL;
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.look_ahead	= look_ahead;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);

    S.Save			(F);
}

void pBounce(IWriter& F, float friction, float resilience, float cutoff, pDomain D, BOOL allow_rotate)
{
	PABounce 		S;
	
	S.positionL		= D;
	S.position		= S.positionL;
	S.oneMinusFriction = 1.0f - friction;
	S.resilience	= resilience;
	S.cutoffSqr		= _sqr(cutoff);
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	S.Save			(F);
}

void pCopyVertexB(IWriter& F, BOOL copy_pos)
{
	PACopyVertexB 	S;

	S.copy_pos		= copy_pos;
	
	S.Save			(F);
}

void pDamping(IWriter& F, const Fvector& damping,
			 float vlow, float vhigh)
{
	PADamping 	S;
	
	S.damping		= pVector(damping.x, damping.y, damping.z);
	S.vlowSqr		= _sqr(vlow);
	S.vhighSqr		= _sqr(vhigh);
	
	S.Save			(F);
}

void pExplosion(IWriter& F, const Fvector& center, float velocity,
				float magnitude, float stdev, float epsilon, float age, BOOL allow_rotate)
{
	PAExplosion 	S;

	S.centerL		= pVector(center.x, center.y, center.z);
	S.center		= S.centerL;
	S.velocity		= velocity;
	S.magnitude		= magnitude;
	S.stdev			= stdev;
	S.epsilon		= epsilon;
	S.age			= age;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	if(S.epsilon < 0.0f)
		S.epsilon 	= EPS_L;
	
	S.Save			(F);
}

void pFollow(IWriter& F, float magnitude, float epsilon, float max_radius)
{
	PAFollow 	S;
	
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	
	S.Save			(F);
}

void pGravitate(IWriter& F, float magnitude, float epsilon, float max_radius)
{
	PAGravitate 	S;
	
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	
	S.Save			(F);
}

void pGravity(IWriter& F, const Fvector& dir, BOOL allow_rotate)
{
	PAGravity 	S;
	
	S.directionL	= pVector(dir.x, dir.y, dir.z);
	S.direction		= S.directionL;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);

	S.Save			(F);
}

void pJet(IWriter& F, pDomain acc, const Fvector& center,
		 float magnitude, float epsilon, float max_radius, BOOL allow_rotate)
{
	PAJet 	S;
	
	S.centerL		= pVector(center.x, center.y, center.z);
	S.center		= S.centerL;
	S.accL			= acc;
	S.acc			= S.accL;
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	S.Save			(F);
}

void pKillOld(IWriter& F, float age_limit, BOOL kill_less_than)
{
	PAKillOld 	S;
	
	S.age_limit		= age_limit;
	S.kill_less_than = kill_less_than;
	
	S.Save			(F);
}

void pMove(IWriter& F)
{
	PAMove 		S;
    S.Save			(F);
}

void pMatchVelocity(IWriter& F, float magnitude, float epsilon, float max_radius)
{
	PAMatchVelocity 	S;
	
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	
	S.Save			(F);
}

void pOrbitLine(IWriter& F, const Fvector& p, const Fvector& axis,
				float magnitude, float epsilon, float max_radius, BOOL allow_rotate)
{
	PAOrbitLine 	S;
	
	S.pL			= pVector(p.x, p.y, p.z);
	S.p			= S.pL;
	S.axisL		= pVector(axis.x, axis.y, axis.z);
	S.axisL.normalize();
	S.axis			= S.axisL;
	S.magnitude	= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	S.Save			(F);
}

void pOrbitPoint(IWriter& F, const Fvector& center,
				 float magnitude, float epsilon, float max_radius, BOOL allow_rotate)
{
	PAOrbitPoint 	S;
	
	S.centerL		= pVector(center.x, center.y, center.z);
	S.center		= S.centerL;
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	S.Save			(F);
}

void pRandomAccel(IWriter& F, pDomain D, BOOL allow_rotate)
{
	PARandomAccel 	S;
	
	S.gen_accL		= D;
	S.gen_acc		= S.gen_accL;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	S.Save			(F);
}

void pRandomDisplace(IWriter& F, pDomain D, BOOL allow_rotate)
{
	PARandomDisplace 	S;
	
	S.gen_dispL		= D;
	S.gen_disp		= S.gen_dispL;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	S.Save			(F);
}

void pRandomVelocity(IWriter& F, pDomain D, BOOL allow_rotate)
{
	PARandomVelocity 	S;
	
	S.gen_velL		= D;
	S.gen_vel		= S.gen_velL;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	S.Save			(F);
}

void pRestore(IWriter& F, float time_left)
{
	PARestore 	S;
	
	S.time_left		= time_left;
	
	S.Save			(F);
}

void pSink(IWriter& F, BOOL kill_inside, pDomain D, BOOL allow_rotate)
{
	PASink 	S;
	
	S.kill_inside	= kill_inside;
	S.positionL		= D;
	S.position		= S.positionL;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	S.Save			(F);
}

void pSinkVelocity(IWriter& F, BOOL kill_inside, pDomain D, BOOL allow_rotate)
{
	PASinkVelocity 	S;
	
	S.kill_inside	= kill_inside;
	S.velocityL		= D;
	S.velocity		= S.velocityL;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	S.Save			(F);
}

void pSource(IWriter& F, float particle_rate, pDomain pos, pDomain vel, pDomain rot, pDomain size, BOOL single_size,
			pDomain color, float alpha, float age, float age_sigma, float parent_motion,
            BOOL allow_rotate)
{
	PASource 	S;

	S.type			= PASourceID;
    S.m_Flags.zero	();
    
	S.particle_rate = particle_rate;
	S.positionL		= pos;
	S.position		= S.positionL;
	S.velocityL		= vel;
	S.velocity		= S.velocityL;
	S.size			= size;
	S.rot			= rot;
	S.color			= color;
	S.alpha			= alpha;
	S.age			= age;
	S.age_sigma		= age_sigma;
	S.m_Flags.set	((single_size?PASource::flSingleSize:0)|PASource::flVertexB_tracks);
	S.parent_vel	= pVector(0,0,0);
	S.parent_motion	= parent_motion;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);

	S.Save			(F);
}

void pSpeedLimit(IWriter& F, float min_speed, float max_speed)
{
	PASpeedLimit 	S;

	S.min_speed = min_speed;
	S.max_speed = max_speed;

	S.Save			(F);
}

void pTargetColor(IWriter& F, const Fvector& color, float alpha, float scale)
{
	PATargetColor 	S;
	
	S.color = pVector(color.x, color.y, color.z);
	S.alpha = alpha;
	S.scale = scale;
	
	S.Save			(F);
}

void pTargetSize(IWriter& F, const Fvector& size, const Fvector& scale)
{
	PATargetSize 	S;
	
	S.size = pVector(size.x, size.y, size.z);
	S.scale = pVector(scale.x, scale.y, scale.z);
	
	S.Save			(F);
}

void pTargetRotate(IWriter& F, const Fvector& rot, float scale)
{
	PATargetRotate 	S;

	S.rot = pVector(rot.x, rot.y, rot.z);
	S.scale = scale;

	S.Save			(F);
}

void pTargetVelocity(IWriter& F, const Fvector& vel, float scale, BOOL allow_rotate)
{
	PATargetVelocity 	S;
	
	S.velocityL		= pVector(vel.x, vel.y, vel.z);
	S.velocity		= S.velocityL;
	S.scale			= scale;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	S.Save			(F);
}

void pVortex(IWriter& F, const Fvector& center, const Fvector& axis,
			float magnitude, float epsilon, float max_radius, BOOL allow_rotate)
{
	PAVortex 	S;
	
	S.centerL		= pVector(center.x, center.y, center.z);
	S.center		= S.centerL;
	S.axisL			= pVector(axis.x, axis.y, axis.z);
	S.axisL.normalize();
	S.axis			= S.axisL;
	S.magnitude		= magnitude;
	S.epsilon		= epsilon;
	S.max_radius	= max_radius;
	S.m_Flags.set	(ParticleAction::ALLOW_ROTATE,allow_rotate);
	
	S.Save			(F);
}
//------------------------------------------------------------------------------
#define EXPAND_DOMAIN(D)			D.type,\
									D.f[0], D.f[1], D.f[2],\
									D.f[3], D.f[4], D.f[5],\
						            D.f[6], D.f[7], D.f[9]
                                    
EPAAvoid::EPAAvoid					():EParticleAction(PAPI::PAAvoidID)
{
	actionType						= "Avoid";
	actionName						= actionType;
    appendDomain					("Position",	PDomain(PDomain::vNum));
    appendFloat						("Magnitude",	0.f);
    appendFloat						("Epsilon",		0.f);
    appendFloat						("Look Ahead",	0.f);
    appendBool						("Allow Rotate",	TRUE);
}
void	EPAAvoid::Compile			(IWriter& F)
{
	if (flags.is(flEnabled))
		pAvoid(F, _float("Magnitude").val, _float("Epsilon").val, _float("Look Ahead").val, pDomain(EXPAND_DOMAIN(_domain("Position"))), _bool("Allow Rotate").val);
}

EPABounce::EPABounce				():EParticleAction(PAPI::PABounceID)
{
	actionType						= "Bounce";
	actionName						= actionType;
    appendDomain					("Position",PDomain(PDomain::vNum));
    appendFloat						("Friction",0.f);
    appendFloat						("Resilience",0.f);
    appendFloat						("Cutoff",0.f);
    appendBool						("Allow Rotate",	TRUE);
}
void	EPABounce::Compile			(IWriter& F)
{
	if (flags.is(flEnabled))
	 	pBounce(F,_float("Friction").val, _float("Resilience").val, _float("Cutoff").val, pDomain(EXPAND_DOMAIN(_domain("Position"))), _bool("Allow Rotate").val);
}

EPACopyVertexB::EPACopyVertexB  	():EParticleAction(PAPI::PACopyVertexBID)
{
	actionType						= "CopyVertexB";
	actionName						= actionType;
    appendBool						("Copy Position", TRUE);
}
void	EPACopyVertexB::Compile	   	(IWriter& F)
{
	if (flags.is(flEnabled))
		pCopyVertexB(F,_bool("Copy Position").val);
}

EPADamping::EPADamping				():EParticleAction(PAPI::PADampingID)
{
	actionType						= "Damping";
	actionName						= actionType;
    appendVector					("Damping", PVector::vNum, 0.f,0.f,0.f);
    appendFloat						("V Low",0.f);
    appendFloat						("V High",P_MAXFLOAT);
}
void	EPADamping::Compile			(IWriter& F)
{
	if (flags.is(flEnabled))
		pDamping(F,_vector("Damping").val, _float("V Low").val, _float("V High").val);
}

EPAExplosion::EPAExplosion			():EParticleAction(PAPI::PAExplosionID)
{
	actionType						= "Explosion";
	actionName						= actionType;
    appendVector					("Center",PVector::vNum, 0.f,0.f,0.f);
    appendFloat						("Velocity",0.f);
    appendFloat						("Magnitude",0.f);
    appendFloat						("Standart Dev",0.f);
    appendFloat						("Epsilon",EPS_L);
    appendFloat						("Age",0.f);
    appendBool						("Allow Rotate",	TRUE);
}
void	EPAExplosion::Compile	  	(IWriter& F)
{
	if (flags.is(flEnabled))
    	pExplosion(F,_vector("Center").val, _float("Velocity").val, _float("Magnitude").val, _float("Standart Dev").val, _float("Epsilon").val, _float("Age").val, _bool("Allow Rotate").val);
}

EPAFollow::EPAFollow				():EParticleAction(PAPI::PAFollowID)
{
	actionType						= "Follow";
	actionName						= actionType;
    appendFloat						("Magnitude",0.f);
    appendFloat						("Epsilon",EPS_L);
    appendFloat						("Max Radius",P_MAXFLOAT);
}
void	EPAFollow::Compile			(IWriter& F)
{
	if (flags.is(flEnabled))
		pFollow(F,_float("Magnitude").val, _float("Epsilon").val, _float("Max Radius").val);
}

EPAGravitate::EPAGravitate			():EParticleAction(PAPI::PAGravitateID)
{
	actionType						= "Gravitate";
	actionName						= actionType;
    appendFloat						("Magnitude",0.f);
    appendFloat						("Epsilon",EPS_L);
    appendFloat						("Max Radius",P_MAXFLOAT);
}
void	EPAGravitate::Compile	   	(IWriter& F)
{
	if (flags.is(flEnabled))
		pGravitate(F,_float("Magnitude").val, _float("Epsilon").val, _float("Max Radius").val);
}

EPAGravity::EPAGravity				():EParticleAction(PAPI::PAGravityID)
{
	actionType						= "Gravity";
	actionName						= actionType;
    appendVector					("Direction",PVector::vNum,0.f,0.f,0.f);
    appendBool						("Allow Rotate",	TRUE);
}
void	EPAGravity::Compile			(IWriter& F)
{
	if (flags.is(flEnabled))
		pGravity(F,_vector("Direction").val, _bool("Allow Rotate").val);
}

EPAJet::EPAJet						():EParticleAction(PAPI::PAJetID)
{
	actionType						= "Jet";
	actionName						= actionType;
    appendDomain					("Accelerate",PDomain(PDomain::vNum));
    appendVector					("Center",PVector::vNum, 0.f,0.f,0.f);
    appendFloat						("Magnitude",0.f);
    appendFloat						("Epsilon",EPS_L);
    appendFloat						("Max Radius",P_MAXFLOAT);
    appendBool						("Allow Rotate",	TRUE);
}
void	EPAJet::Compile				(IWriter& F)
{
	if (flags.is(flEnabled))
	 	pJet(F,pDomain(EXPAND_DOMAIN(_domain("Accelerate"))),_vector("Center").val, _float("Magnitude").val, _float("Epsilon").val, _float("Max Radius").val, _bool("Allow Rotate").val);
}

EPAKillOld::EPAKillOld				():EParticleAction(PAPI::PAKillOldID)
{
	actionType						= "KillOld";
	actionName						= actionType;
    appendFloat						("Age Limit",		0.f);
    appendBool						("Kill Less Than",	FALSE);
}
void	EPAKillOld::Compile			(IWriter& F)
{
	if (flags.is(flEnabled))
		pKillOld(F,_float("Age Limit").val, _bool("Kill Less Than").val);
}

EPAMatchVelocity::EPAMatchVelocity	():EParticleAction(PAPI::PAMatchVelocityID)
{
	actionType						= "MatchVelocity";
	actionName						= actionType;
    appendFloat						("Magnitude",0.f);
    appendFloat						("Epsilon",EPS_L);
    appendFloat						("Max Radius",P_MAXFLOAT);
}
void	EPAMatchVelocity::Compile 	(IWriter& F)
{
	if (flags.is(flEnabled))
        pMatchVelocity(F,_float("Magnitude").val, _float("Epsilon").val, _float("Max Radius").val);
}

EPAMove::EPAMove					():EParticleAction(PAPI::PAMoveID)
{
	actionType						= "Move";
	actionName						= actionType;
}
void	EPAMove::Compile			(IWriter& F)
{
	if (flags.is(flEnabled))
		pMove(F);
}

EPAOrbitLine::EPAOrbitLine			():EParticleAction(PAPI::PAOrbitLineID)
{
	actionType						= "OrbitLine";
	actionName						= actionType;
    appendVector					("Position",		PVector::vNum, 0.f,0.f,0.f);
    appendVector					("Axis",			PVector::vNum, 0.f,0.f,0.f);
    appendFloat						("Magnitude",		1.f);
    appendFloat						("Epsilon",			EPS_L);
    appendFloat						("Max Radius",		P_MAXFLOAT);
    appendBool						("Allow Rotate",	TRUE);
}
void	EPAOrbitLine::Compile	 	(IWriter& F)
{
	if (flags.is(flEnabled))
		pOrbitLine(F,_vector("Position").val, _vector("Axis").val, _float("Magnitude").val, _float("Epsilon").val, _float("Max Radius").val, _bool("Allow Rotate").val);
}

EPAOrbitPoint::EPAOrbitPoint		():EParticleAction(PAPI::PAOrbitPointID)
{
	actionType						= "OrbitPoint";
	actionName						= actionType;
    appendVector					("Center",			PVector::vNum, 0.f,0.f,0.f);
    appendFloat						("Magnitude",		1.f);
    appendFloat						("Epsilon",			EPS_L);
    appendFloat						("Max Radius",		P_MAXFLOAT);
    appendBool						("Allow Rotate",	TRUE);
}
void	EPAOrbitPoint::Compile	   	(IWriter& F)
{
	if (flags.is(flEnabled))
		pOrbitPoint(F,_vector("Center").val, _float("Magnitude").val, _float("Epsilon").val, _float("Max Radius").val, _bool("Allow Rotate").val);
}

EPARandomAccel::EPARandomAccel		():EParticleAction(PAPI::PARandomAccelID)
{
	actionType						= "RandomAccel";
	actionName						= actionType;
    appendDomain					("Accelerate",PDomain(PDomain::vNum));
    appendBool						("Allow Rotate",	TRUE);
}
void	EPARandomAccel::Compile	   	(IWriter& F)
{
	if (flags.is(flEnabled))
		pRandomAccel(F,pDomain(EXPAND_DOMAIN(_domain("Accelerate"))), _bool("Allow Rotate").val);
}

EPARandomDisplace::EPARandomDisplace():EParticleAction(PAPI::PARandomDisplaceID)
{
	actionType						= "RandomDisplace";
	actionName						= actionType;
    appendDomain					("Displace",PDomain(PDomain::vNum));
    appendBool						("Allow Rotate",	TRUE);
}
void	EPARandomDisplace::Compile 	(IWriter& F)
{
	if (flags.is(flEnabled))
		pRandomDisplace(F,pDomain(EXPAND_DOMAIN(_domain("Displace"))), _bool("Allow Rotate").val);

}

EPARandomVelocity::EPARandomVelocity():EParticleAction(PAPI::PARandomVelocityID)
{
	actionType						= "RandomVelocity";
	actionName						= actionType;
    appendDomain					("Velocity",PDomain(PDomain::vNum));
    appendBool						("Allow Rotate",	TRUE);
}
void	EPARandomVelocity::Compile 	(IWriter& F)
{
	if (flags.is(flEnabled))
		pRandomVelocity(F,pDomain(EXPAND_DOMAIN(_domain("Velocity"))), _bool("Allow Rotate").val);

}

EPARestore::EPARestore				():EParticleAction(PAPI::PARestoreID)
{
	actionType						= "Restore";
	actionName						= actionType;
    appendFloat						("Time",			0.f);
}
void	EPARestore::Compile			(IWriter& F)
{
	if (flags.is(flEnabled))
		pRestore(F,_float("Time").val);
}

EPASink::EPASink					():EParticleAction(PAPI::PASinkID)
{
	actionType						= "Sink";
	actionName						= actionType;
    appendBool						("Kill Inside",		TRUE);
    appendDomain					("Domain",			PDomain(PDomain::vNum));
    appendBool						("Allow Rotate",	TRUE);
}
void	EPASink::Compile			(IWriter& F)
{
	if (flags.is(flEnabled))
		pSink(F,_bool("Kill Inside").val, pDomain(EXPAND_DOMAIN(_domain("Domain"))), _bool("Allow Rotate").val);
}

EPASinkVelocity::EPASinkVelocity	():EParticleAction(PAPI::PASinkVelocityID)
{
	actionType						= "SinkVelocity";
	actionName						= actionType;
    appendBool						("Kill Inside",		TRUE);
    appendDomain					("Domain",PDomain(PDomain::vNum));
    appendBool						("Allow Rotate",	TRUE);
}
void	EPASinkVelocity::Compile   	(IWriter& F)
{
	if (flags.is(flEnabled))
		pSinkVelocity(F,_bool("Kill Inside").val, pDomain(EXPAND_DOMAIN(_domain("Domain"))), _bool("Allow Rotate").val);
}

EPASource::EPASource				():EParticleAction(PAPI::PASourceID)
{
	actionType						= "Source";
	actionName						= actionType;
	appendFloat						("Rate",			10.f);
	appendDomain					("Domain",			PDomain(PDomain::vNum));
	appendDomain					("Velocity",		PDomain(PDomain::vNum));
	appendDomain					("Rotation",		PDomain(PDomain::vAngle));
	appendDomain					("Size",			PDomain(PDomain::vNum));
	appendBool						("Single Size",		TRUE);
	appendDomain					("Color",			PDomain(PDomain::vColor, PAPI::PDPoint,1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f,1.f));
	appendFloat						("Alpha",			0.f, 0.f, 1.f);
	appendFloat						("Starting Age",	0.f);
	appendFloat						("Age Sigma",		0.f);
	appendFloat						("Parent Motion",	0.f);
    appendBool						("Allow Rotate",	TRUE);
}
void	EPASource::Compile			(IWriter& F)
{
	if (flags.is(flEnabled))
		pSource(F,_float("Rate").val, 	pDomain(EXPAND_DOMAIN(_domain("Domain"))), 
        							pDomain(EXPAND_DOMAIN(_domain("Velocity"))),
        							pDomain(EXPAND_DOMAIN(_domain("Rotation"))),
        							pDomain(EXPAND_DOMAIN(_domain("Size"))), _bool("Single Size").val,
        							pDomain(EXPAND_DOMAIN(_domain("Color"))), _float("Alpha").val,
                                    _float("Starting Age").val, _float("Age Sigma").val, _float("Parent Motion").val,
        							_bool("Allow Rotate").val);
}

EPASpeedLimit::EPASpeedLimit		():EParticleAction(PAPI::PASpeedLimitID)
{
	actionType						= "SpeedLimit";
	actionName						= actionType;
    appendFloat						("Min Speed",			0.f);
    appendFloat						("Max Speed",			P_MAXFLOAT);
}
void	EPASpeedLimit::Compile	 	(IWriter& F)
{
	if (flags.is(flEnabled))
		pSpeedLimit(F,_float("Min Speed").val, _float("Max Speed").val);
}

EPATargetColor::EPATargetColor		():EParticleAction(PAPI::PATargetColorID)
{
	actionType						= "TargetColor";
	actionName						= actionType;
    appendVector					("Color",			PVector::vColor, 1.f,1.f,1.f, 0.f,1.f);
    appendFloat						("Alpha",			1.f);
    appendFloat						("Scale",			1.f);
}
void	EPATargetColor::Compile	  	(IWriter& F)
{
	if (flags.is(flEnabled))
		pTargetColor(F,_vector("Color").val, _float("Alpha").val, _float("Scale").val);
}

EPATargetSize::EPATargetSize		():EParticleAction(PAPI::PATargetSizeID)
{
	actionType						= "TargetSize";
	actionName						= actionType;
    appendVector					("Size",			PVector::vNum, 1.f,1.f,1.f, EPS_L);
    appendVector					("Scale",			PVector::vNum, 0.f,0.f,0.f);
}
void	EPATargetSize::Compile	  	(IWriter& F)
{
	if (flags.is(flEnabled))
		pTargetSize(F,_vector("Size").val, _vector("Scale").val);
}

EPATargetRotate::EPATargetRotate	():EParticleAction(PAPI::PATargetRotateID)
{
	actionType						= "TargetRotate";
	actionName						= actionType;
    appendVector					("Rotation",		PVector::vAngle, 0.f,0.f,0.f);
    appendFloat						("Scale",			0.f);
}
void	EPATargetRotate::Compile   	(IWriter& F)
{
	if (flags.is(flEnabled))
		pTargetRotate(F,_vector("Rotation").val, _float("Scale").val);
}

EPATargetVelocity::EPATargetVelocity():EParticleAction(PAPI::PATargetVelocityID)
{
	actionType						= "TargetVelocity";
	actionName						= actionType;
    appendVector					("Velocity",		PVector::vNum, 0.f,0.f,0.f);
    appendFloat						("Scale",			0.f);
    appendBool						("Allow Rotate",	TRUE);
}
void	EPATargetVelocity::Compile	(IWriter& F)
{
	if (flags.is(flEnabled))
		pTargetVelocity(F,_vector("Velocity").val, _float("Scale").val, _bool("Allow Rotate").val);
}

EPAVortex::EPAVortex				():EParticleAction(PAPI::PAVortexID)
{
	actionType						= "Vortex";
	actionName						= actionType;
    appendVector					("Center",			PVector::vNum, 0.f,0.f,0.f);
    appendVector					("Axis",			PVector::vNum, 0.f,0.f,0.f);
    appendFloat						("Magnitude",		1.f);
    appendFloat						("Epsilon",			EPS_L);
    appendFloat						("Max Radius",		P_MAXFLOAT);
    appendBool						("Allow Rotate",	TRUE);
}
void	EPAVortex::Compile			(IWriter& F)
{
	if (flags.is(flEnabled))
         pVortex(F,_vector("Center").val, _vector("Axis").val, _float("Magnitude").val, _float("Epsilon").val, _float("Max Radius").val, _bool("Allow Rotate").val);
}

