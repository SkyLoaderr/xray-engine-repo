#pragma once
#include "PHCharacter.h"
#include "Physics.h"
#include "MathUtils.h"
#define DRAW_BOXES
class CPHSimpleCharacter : public CPHCharacter

{
#ifdef DRAW_BOXES
	Fvector m_bcenter;
	Fvector m_bcenter_forbid;
	Fvector m_AABB;
	Fvector m_AABB_forbid;
#endif


protected:
	////////////////////////////damage////////////////////////////////////////
	dContact				m_damege_contact;
	float					m_dmc_signum;
	enum{ctStatic,ctObject}	m_dmc_type;
	/////////////////////////// callback
	ObjectContactCallbackFun*	m_object_contact_callback;
	////////////////////////// geometry
	dGeomID m_geom_shell;
	dGeomID m_wheel;
	dGeomID m_hat;
	dGeomID	m_cap;

	dGeomID m_hat_transform;
	dGeomID m_wheel_transform;
	dGeomID m_shell_transform;
	dGeomID m_cap_transform;

	dSpaceID m_space;
	
	dReal m_radius;
	dReal m_cyl_hight;
	///////////////////////////////////
	dJointID m_capture_joint;
	dJointFeedback m_capture_joint_feedback;
	////////////////////////// movement
	dVector3 m_control_force;
	Fvector	 m_acceleration;	
	dVector3 m_wall_contact_normal;
	dVector3 m_ground_contact_normal;
	dVector3 m_clamb_depart_position;
	dVector3 m_depart_position;
	dVector3 m_wall_contact_position;
	dVector3 m_ground_contact_position;
	dReal	 jump_up_velocity;//=6.0f;//5.6f;

	dReal	 m_max_velocity;

	float   m_air_control_factor;

	dVector3 m_jump_depart_position;
	dVector3 m_death_position;
	Fvector  m_jump_accel;

	//movement state
	bool is_contact;
	bool was_contact;
	bool is_control;
	bool b_depart;
	bool b_meet;
	bool b_meet_control;
	bool b_on_ground;
	bool b_lose_control;
	bool b_lose_ground;
	bool b_depart_control;
	bool b_jump;
	bool b_clamb_jump;
	bool b_side_contact;
	bool b_any_contacts;
	bool b_air_contact_state;
	bool b_valide_ground_contact;
	bool b_valide_wall_contact;
	bool b_saved_contact_velocity;
	bool b_jumping;
	bool b_climb;
	bool b_pure_climb;
	bool b_at_wall;
	bool was_control;
	bool b_stop_control;
	bool b_on_object;
	bool b_was_on_object;
	bool b_external_impulse;
	bool b_death_pos;

	bool b_climb_getup;
	bool b_block_climb_getup;
	dReal m_start_climb_getup_height;

	u32  m_contact_count;

	dReal m_friction_factor;
public:
	CPHSimpleCharacter										()					;
	virtual					~CPHSimpleCharacter					()						{Destroy();}

	/////////////////CPHObject//////////////////////////////////////////////
	virtual		void		PhDataUpdate						(dReal step)						;
	virtual		void		PhTune								(dReal step)						;
	virtual		void		InitContact							(dContact* c,bool &do_collide)		;
	virtual		dSpaceID	dSpace								()									{return m_space;}
	virtual		dGeomID		dSpacedGeom							()									{return (dGeomID)m_space;}
	virtual		void		get_spatial_params					()									;
	/////////////////CPHCharacter////////////////////////////////////////////
public:
	//update


	//Check state
	virtual		bool			 ContactWas						()					{if(b_meet_control) {b_meet_control=false;return true;} else return false;}
	virtual		EEnvironment	 CheckInvironment				()					;

	//Creating
	virtual		void		Create								(dVector3 sizes)	;
	virtual		void		Destroy								(void)				;

	//get-set
	virtual		void		SetObjectContactCallback			(ObjectContactCallbackFun* callback);
	virtual		void		SetAcceleration						(Fvector accel)		;
	virtual		Fvector		GetAcceleration						()					{ return m_acceleration; };
	virtual		void		SetMaterial							(u16 material)		;
	virtual		void		SetPosition							(Fvector pos)		;
	virtual		void		GetVelocity							(Fvector& vvel)		;
	virtual		void		SetVelocity							(Fvector vel)		;
	virtual		void		SetAirControlFactor					(float factor)		{m_air_control_factor=factor;}
	virtual		void		GetPosition							(Fvector& vpos)		;
	virtual		float		FootRadius							()					;
	virtual		void		DeathPosition						(Fvector& deathPos){ deathPos.set(m_death_position);deathPos.y=m_death_position[1]-m_radius;}
	virtual		void		IPosition							(Fvector& pos)		;
	virtual		u16			ContactBone							();
	virtual		void		ApplyImpulse						(const Fvector& dir,const dReal P);
	virtual		void		ApplyForce							(const Fvector& force);
	virtual		void		ApplyForce							(const Fvector& dir,float force);
	virtual		void		ApplyForce							(float x,float y, float z);
	virtual		void		AddControlVel						(const Fvector& vel);
	virtual		void		SetMaximumVelocity					(dReal vel)			{m_max_velocity=vel;}
	virtual		dReal		GetMaximumVelocity					()					{ return m_max_velocity;}
	virtual		void		SetJupmUpVelocity					(dReal velocity)	{jump_up_velocity=velocity;}
	virtual		bool		JumpState							()					{
																					return b_jumping||b_jump;
																					};
	virtual		void		SetMas								(dReal mass)		;

	virtual		void		SetPhysicsRefObject					(CGameObject* ref_object);

	virtual		void		CaptureObject						(dBodyID body,const dReal* anchor);
	virtual		void		CapturedSetPosition					(const dReal* position);
	virtual		void		doCaptureExist						(bool&	do_exist);

	virtual		void		get_State							(		SPHNetState&	state)								;
	virtual		void		set_State							(const	SPHNetState&	state)								;

private:
	void		CheckCaptureJoint					()					;
	void		ApplyAcceleration					()					;
	bool		ValidateWalkOn						()					;
	u16			RetriveContactBone					()					;
IC	void		SafeAndLimitVelocity						()
	{
		
		const float		*linear_velocity		=dBodyGetLinearVel(m_body);
		//limit velocity
		dReal l_limit;
		if(is_control&&!b_lose_control) 
			l_limit = m_max_velocity/phTimefactor;
		else			
			l_limit=10.f/fixed_step;

		dReal mag;

		if(dV_valid(linear_velocity))
		{	
			mag=_sqrt(linear_velocity[0]*linear_velocity[0]+linear_velocity[1]*linear_velocity[1]+linear_velocity[2]*linear_velocity[2]);//
			if(mag>l_limit)
			{
				dReal f=mag/l_limit;
				if(b_lose_ground&&linear_velocity[1]<0.f)
					dBodySetLinearVel(m_body,linear_velocity[0]/f,linear_velocity[1],linear_velocity[2]/f);///f
				else			 
					dBodySetLinearVel(m_body,linear_velocity[0]/f,linear_velocity[1]/f,linear_velocity[2]/f);///f
				if(is_control&&!b_lose_control)
										dBodySetPosition(m_body,
														m_safe_position[0]+linear_velocity[0]*fixed_step,
														m_safe_position[1]+linear_velocity[1]*fixed_step,
														m_safe_position[2]+linear_velocity[2]*fixed_step);
			}
		}
		else
		{
					dBodySetLinearVel(m_body,m_safe_velocity[0],m_safe_velocity[1],m_safe_velocity[2]);
		}

		if(!dV_valid(dBodyGetPosition(m_body)))
			dBodySetPosition(m_body,m_safe_position[0]-m_safe_velocity[0]*fixed_step,
			m_safe_position[1]-m_safe_velocity[1]*fixed_step,
			m_safe_position[2]-m_safe_velocity[2]*fixed_step);


		dVectorSet(m_safe_position,dBodyGetPosition(m_body));
		dVectorSet(m_safe_velocity,linear_velocity);

	}
	
public:	
#ifdef DEBUG
	virtual		void		OnRender							()					;
#endif
};