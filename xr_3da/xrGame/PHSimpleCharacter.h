#pragma once
#include "PHCharacter.h"

extern float object_demage_factor;

class CPHSimpleCharacter : public CPHCharacter
{
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

	dGeomID m_geom_group;
	
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
	bool b_depart_control;
	bool b_jump;
	bool b_clamb_jump;
	bool b_side_contact;
	bool b_any_contacts;
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
	u32  m_contact_count;

	dReal m_friction_factor;
public:
	CPHSimpleCharacter										()					;
	virtual					~CPHSimpleCharacter										()					{Destroy();};

	/////////////////CPHObject//////////////////////////////////////////////
	virtual		void		PhDataUpdate						(dReal step)		;
	virtual		void		PhTune								(dReal step)		;
	virtual		void		InitContact							(dContact* c)		;
	virtual		void		StepFrameUpdate						(dReal /**step/**/)		{};
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

	virtual		void		SetPosition							(Fvector pos)		;
	virtual		void		GetVelocity							(Fvector& vvel)		;
	virtual		void		SetVelocity							(Fvector vel)		;
	virtual		void		GetPosition							(Fvector& vpos)		;
	virtual		void		DeathPosition						(Fvector& deathPos){ deathPos.set(m_death_position);deathPos.y=m_death_position[1]-m_radius;}
	virtual		void		IPosition							(Fvector& pos)		;
	virtual		u16			ContactBone							();
	virtual		void		ApplyImpulse						(const Fvector& dir,const dReal P);
	virtual		void		ApplyForce							(const Fvector& force);
	virtual		void		ApplyForce							(const Fvector& dir,float force);
	virtual		void		ApplyForce							(float x,float y, float z);
	virtual		void		SetMaximumVelocity					(dReal vel)			{m_max_velocity=vel;}
	virtual		dReal		GetMaximumVelocity					()					{ return m_max_velocity;}
	virtual		void		SetJupmUpVelocity					(dReal velocity)	{jump_up_velocity=velocity;}
	virtual		bool		JumpState							()					{
																					return b_jumping||b_jump;
																					};
	virtual		void		SetMas								(dReal mass)		;

	virtual		void		SetPhysicsRefObject					(CPhysicsRefObject* ref_object);

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
public:	
#ifdef DEBUG
	virtual		void		OnRender							()					;
#endif
};