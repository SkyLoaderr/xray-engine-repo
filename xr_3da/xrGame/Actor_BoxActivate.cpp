#include "stdafx.h"
#include "Actor.h"
#include "ExtendedGeom.h"
#include "MathUtils.h"
#include "Physics.h"
#include "Level.h"

ObjectContactCallbackFun* saved_callback		=	0	;
static float max_depth							=	0.f	;

struct STestCallbackPars
{
	static float calback_friction_factor			;
	static float depth_to_use_force					;
	static float callback_force_factor				;
	static float depth_to_change_softness_pars		;
	static float callback_cfm_factor				;
	static float callback_erp_factor				;
	static float decrement_depth					;
};


float 	STestCallbackPars::calback_friction_factor					=	0.1f	;
float 	STestCallbackPars::depth_to_use_force						=	0.3f	;
float 	STestCallbackPars::callback_force_factor					=	10.f	;
float 	STestCallbackPars::depth_to_change_softness_pars			=	0.00f	;
float 	STestCallbackPars::callback_cfm_factor						=	100.f	;
float 	STestCallbackPars::callback_erp_factor						=	1.f		;
float	STestCallbackPars::decrement_depth							=	0.f		;
struct STestFootCallbackPars
{
	static float calback_friction_factor			;
	static float depth_to_use_force					;
	static float callback_force_factor				;
	static float depth_to_change_softness_pars		;
	static float callback_cfm_factor				;
	static float callback_erp_factor				;
	static float decrement_depth						;
};


float 	STestFootCallbackPars::calback_friction_factor					=	0.7f	;
float 	STestFootCallbackPars::depth_to_use_force						=	0.3f	;
float 	STestFootCallbackPars::callback_force_factor					=	10.f	;
float 	STestFootCallbackPars::depth_to_change_softness_pars			=	0.01f	;
float 	STestFootCallbackPars::callback_cfm_factor						=	1.f		;
float 	STestFootCallbackPars::callback_erp_factor						=	1.f		;
float	STestFootCallbackPars::decrement_depth							=	0.00f	;

template<class Pars>
void __stdcall TTestDepthCallback (bool& do_colide,dContact& c,SGameMtl* material_1,SGameMtl* material_2)
{
	if(saved_callback)saved_callback(do_colide,c,material_1,material_2);

	if(do_colide&&!material_1->Flags.test(SGameMtl::flPassable) &&!material_2->Flags.test(SGameMtl::flPassable))
	{
		const float& depth=c.geom.depth;
		float test_depth=depth-Pars.decrement_depth;
		if(test_depth>max_depth)
						max_depth=test_depth;
		c.surface.mu*=Pars.calback_friction_factor;
		if(test_depth>Pars.depth_to_use_force)
		{
			float force = Pars.callback_force_factor*world_gravity;
			dBodyID b1=dGeomGetBody(c.geom.g1);
			dBodyID b2=dGeomGetBody(c.geom.g2);
			if(b1)dBodyAddForce(b1,c.geom.normal[0]*force,c.geom.normal[1]*force,c.geom.normal[2]*force);
			if(b2)dBodyAddForce(b2,-c.geom.normal[0]*force,-c.geom.normal[1]*force,-c.geom.normal[2]*force);
			dxGeomUserData* ud1=retrieveGeomUserData(c.geom.g1);
			dxGeomUserData* ud2=retrieveGeomUserData(c.geom.g2);

			if(ud1)
			{
				CPhysicsShell* phsl=ud1->ph_ref_object->PPhysicsShell();
				if(phsl) phsl->Enable();
			}

			if(ud2)
			{
				CPhysicsShell* phsl=ud2->ph_ref_object->PPhysicsShell();
				if(phsl) phsl->Enable();
			}


			do_colide=false;
		}
		else if(test_depth>Pars.depth_to_change_softness_pars)
		{
			c.surface.soft_cfm=Pars.callback_cfm_factor;
			c.surface.soft_erp=Pars.callback_erp_factor;
		}	
	}

}

ObjectContactCallbackFun* TestDepthCallback = &TTestDepthCallback<STestCallbackPars>;
ObjectContactCallbackFun* TestFootDepthCallback = &TTestDepthCallback<STestFootCallbackPars>;

class CVelocityLimiter :
	public CPHUpdateObject
{
	dBodyID m_body;
	float l_limit;
	dVector3 m_safe_velocity;
	dVector3 m_safe_position;
public:
	CVelocityLimiter(dBodyID b,float l)
	{
		R_ASSERT(b);
		m_body=b;
		dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));
		dVectorSet(m_safe_position,dBodyGetPosition(m_body));
		l_limit=l;
	}
	virtual ~CVelocityLimiter()
	{
		Deactivate();
		m_body =0;
	}


	bool VelocityLimit()
	{
		const float		*linear_velocity		=dBodyGetLinearVel(m_body);
		//limit velocity

		if(dV_valid(linear_velocity))
		{
			dReal mag;
			mag=_sqrt(linear_velocity[0]*linear_velocity[0]+linear_velocity[1]*linear_velocity[1]+linear_velocity[2]*linear_velocity[2]);//
			if(mag>l_limit)
			{
				dReal f=mag/l_limit;
				dBodySetLinearVel(m_body,linear_velocity[0]/f,linear_velocity[1]/f,linear_velocity[2]/f);///f
				return true;
			}
			else return false;
		}
		else
		{
			dBodySetLinearVel(m_body,m_safe_velocity[0],m_safe_velocity[1],m_safe_velocity[2]);
			return true;
		}
	}
	virtual void PhDataUpdate(dReal step)
	{
		const float		*linear_velocity		=dBodyGetLinearVel(m_body);

		if(VelocityLimit())
		{
			dBodySetPosition(m_body,
				m_safe_position[0]+linear_velocity[0]*fixed_step,
				m_safe_position[1]+linear_velocity[1]*fixed_step,
				m_safe_position[2]+linear_velocity[2]*fixed_step);
		}

		if(!dV_valid(dBodyGetPosition(m_body)))
			dBodySetPosition(m_body,m_safe_position[0]-m_safe_velocity[0]*fixed_step,
			m_safe_position[1]-m_safe_velocity[1]*fixed_step,
			m_safe_position[2]-m_safe_velocity[2]*fixed_step);


		dVectorSet(m_safe_position,dBodyGetPosition(m_body));
		dVectorSet(m_safe_velocity,linear_velocity);
	}

	virtual void PhTune(dReal step)
	{
		VelocityLimit();
	}

};

bool CActor:: ActivateBox(DWORD id)
{
	if(PPhysicsShell())return false;
	DWORD old_id=m_PhysicMovementControl->BoxID();
	bool  character_exist=m_PhysicMovementControl->CharacterExist();
	if(character_exist&&id==old_id)return true;

	if(!character_exist)
	{
		m_PhysicMovementControl->CreateCharacter();
	}

	//m_PhysicMovementControl->ActivateBox(id);
	ph_world->Freeze();
	m_PhysicMovementControl->UnFreeze();

	saved_callback=m_PhysicMovementControl->ObjectContactCallback();
	m_PhysicMovementControl->SetOjectContactCallback(TestDepthCallback);
	m_PhysicMovementControl->SetFootCallBack(TestFootDepthCallback);
	max_depth=0.f;



//////////////////////////////////pars///////////////////////////////////////////
	int		num_it=2;
	int		num_steps=10;
	float	resolve_depth=0.01f;
	STestCallbackPars::callback_cfm_factor=1.f;//300000000.f;
	STestFootCallbackPars::callback_erp_factor=1.f;//0.00000001f;
	if(!character_exist)
	{
		num_it=20;
		num_steps=1;		
		STestCallbackPars::callback_cfm_factor=1.f;		
		STestFootCallbackPars::callback_erp_factor=1.f;
		resolve_depth=0.1f;
	}
///////////////////////////////////////////////////////////////////////
	float	fnum_it=float(num_it);
	float	fnum_steps=float(num_steps);
	float	fnum_steps_r=1.f/fnum_steps;

	Fvector vel;
	Fvector pos;
	m_PhysicMovementControl->GetCharacterVelocity(vel);
	m_PhysicMovementControl->GetCharacterPosition(pos);
	const Fbox& box =m_PhysicMovementControl->Box();
	float pass=box.x2-box.x1;
	float max_vel=pass/fnum_it/fnum_steps/fixed_step; 
	CVelocityLimiter vl(m_PhysicMovementControl->GetBody(),max_vel);

	vl.Activate();
	bool	ret=false;
	for(int m=0;num_steps>m;++m)
	{
		float param =fnum_steps_r*(1+m);
		//float fun_param=param*param;
		//float fun_param1=fun_param*fun_param;
		//float fun_param2=fun_param1*fun_param1;
		//float fun_param3=fun_param2*fun_param2;
		//float fun_param4=fun_param3*fun_param3;
		STestCallbackPars::callback_cfm_factor=0.001f*world_cfm;//(0.0000000000001f+fun_param4*3000000000.f);
		STestCallbackPars::callback_erp_factor=1.f;//1.f/(0.7f+fun_param4*3000000000.f);
		STestFootCallbackPars::callback_cfm_factor=0.001f*world_cfm;//(0.0000000000001f+fun_param4*3000000000.f);
		STestFootCallbackPars::callback_erp_factor=1.f;//1.f/(0.7f+fun_param4*3000000000.f);

		m_PhysicMovementControl->InterpolateBox(id,param);
		ret=false;
		for(int i=0;num_it>i;++i){
			max_depth=0.f;
			m_PhysicMovementControl->EnableCharacter();
			ph_world->Step();
			if(max_depth<resolve_depth) 
			{
				ret=true;
				break;
			}	
		}
		if(!ret) break;
	}
	vl.Deactivate();

	if(!ret)
	{	
		if(!character_exist)m_PhysicMovementControl->DestroyCharacter();
		m_PhysicMovementControl->ActivateBox(old_id);
		m_PhysicMovementControl->SetPosition(pos);
	}
	else
	{
		m_PhysicMovementControl->ActivateBox(id);
	}
	ph_world->UnFreeze();
	m_PhysicMovementControl->SetOjectContactCallback(saved_callback);
	m_PhysicMovementControl->SetVelocity(vel);
	saved_callback=0;
	return ret;
}
