#pragma once
#include "PHObject.h"
#include "PHInterpolation.h"
class CPhysicsRefObject;
 static enum EEnvironment
			{
				peOnGround,
				peAtWall,
				peInAir
			};


class CPHCharacter : public CPHObject
#ifdef DEBUG
	,public pureRender
#endif
{
protected:
////////////////////////// dynamic
xr_list<CPHObject*>::iterator m_ident;
CPHInterpolation m_body_interpolation;
dBodyID m_body;
CPhysicsRefObject* m_phys_ref_object;


dReal m_mass;

////////////////////////////////////////////////////////////////////////////
/////disable////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
dVector3 previous_p;
dVector3 previous_p1;
dReal previous_v;
u32 dis_count_f;
u32 dis_count_f1;
float m_update_time;
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
u32* p_lastMaterial;
///////////////////////////////////////////////////////////////////////////
dVector3 m_safe_velocity;
dVector3 m_safe_position;

void Disabling											();

public:

	void Enable											(){dBodyEnable(m_body);}
	bool IsEnabled										(){ if(!b_exist)return false; return !!dBodyIsEnabled(m_body);}
	bool b_exist;

public:
void		SetPLastMaterial								(u32* p){p_lastMaterial=p;}

virtual void		SetMaximumVelocity								(dReal vel){}
virtual	void		SetJupmUpVelocity								(dReal velocity){}

virtual float		ContactVelocity									()				{return 0.f;}

virtual	void			IPosition							(Fvector& pos)		{}

virtual void			DeathPosition						(Fvector& deathPos){}
virtual		void		ApplyImpulse						(const Fvector& dir,const dReal P)		{}	;
virtual		EEnvironment CheckInvironment					()				=0	;
virtual		bool		ContactWas							()				=0	;
virtual		void		Create								(dVector3 sizes)=0	;
virtual		void		Destroy								(void)			=0	;
virtual		void		SetAcceleration						(Fvector accel)	=0	;
virtual		void		SetPosition							(Fvector pos)	=0	;
virtual		bool		TryPosition							(Fvector pos)	{return false;}	;

virtual		void		GetVelocity							(Fvector& vvel)	=0	;
virtual		void		SetVelocity							(Fvector vel)	=0	;

virtual		void		GetPosition							(Fvector& vpos)	=0	;

virtual		void		SetMas								(dReal mass)	=0	;

virtual		void		SetPhysicsRefObject					(CPhysicsRefObject* ref_object)=0;

			CPHCharacter									(void)				;
virtual		~CPHCharacter									(void)				;
};

class CPHSimpleCharacter : public CPHCharacter {
protected:
////////////////////////// geometry
	dGeomID m_geom_shell;
	dGeomID m_wheel;
	dGeomID	m_cap;
	dGeomID m_hat;
	dGeomID m_hat_transform;
	dGeomID m_geom_group;
	dGeomID m_wheel_transform;
	dGeomID m_shell_transform;
	dGeomID m_cap_transform;
	dReal m_radius;
	dReal m_cyl_hight;
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

	dReal m_max_velocity;



	dVector3 m_jump_depart_position;
	dVector3 m_death_position;
	Fvector  m_jump_accel;
	float	 m_contact_velocity;
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
virtual		void		StepFrameUpdate						(dReal step)		{};
/////////////////CPHCharacter////////////////////////////////////////////
public:
//update
virtual		void		Disabling							()					;

//Check state
virtual		bool			 ContactWas						()					{if(b_meet_control) {b_meet_control=false;return true;} else return false;}
virtual		EEnvironment	 CheckInvironment				()					;

//Creating
virtual		void		Create								(dVector3 sizes)	;
virtual		void		Destroy								(void)				;


virtual		void		SetAcceleration						(Fvector accel)		;

virtual		void		SetPosition							(Fvector pos)		;
virtual		void		GetVelocity							(Fvector& vvel)		;
virtual		void		SetVelocity							(Fvector vel)		;
virtual		void		GetPosition							(Fvector& vpos)		;
virtual		void		DeathPosition						(Fvector& deathPos){ deathPos.set(m_death_position);deathPos.y=m_death_position[1]-m_radius;}
virtual		void		IPosition							(Fvector& pos)		;

virtual		void		ApplyImpulse						(const Fvector& dir,const dReal P);
virtual		void		SetMaximumVelocity					(dReal vel)			{m_max_velocity=vel; }
virtual		void		SetJupmUpVelocity					(dReal velocity)	{jump_up_velocity=velocity;}
virtual		float		ContactVelocity						()					{ dReal ret= m_contact_velocity; m_contact_velocity=0; return ret;}

virtual		void		SetMas								(dReal mass)		;

virtual		void		SetPhysicsRefObject					(CPhysicsRefObject* ref_object);
private:
			void		ApplyAcceleration					()					;
			bool		ValidateWalkOn						()					;
public:	
#ifdef DEBUG
virtual		void		OnRender							()					;
#endif
};


class CPHWheeledCharacter : public CPHCharacter {

dBodyID m_wheel_body;


////////////////////////////
dJointID m_wheel_joint;
/////////////////////////////

CPHWheeledCharacter();

/////////////////CPHObject//////////////////////////////////////////////
virtual		void		PhDataUpdate						(dReal step)	{}	;
virtual		void		PhTune								(dReal step)	{}	;
virtual		void		InitContact							(dContact* c)	{}	;
/////////////////CPHCharacter////////////////////////////////////////////
public:
virtual		EEnvironment CheckInvironment					()				{ return peOnGround; }	;
virtual		bool		ContactWas							(){return false;//b_meet;
																}	;
virtual		void		Create							  (dVector3 sizes);
virtual		void		Destroy								(void)				;
virtual		void		SetAcceleration						(Fvector accel)	{}	;
virtual		void		SetPosition							(Fvector pos)	{}	;
virtual		void		GetVelocity							(Fvector& vel)	{}	;
virtual		void		SetVelocity							(Fvector vel)	{}	;
virtual		void		GetPosition							(Fvector& pos)	{}	;
virtual		void		SetMas								(dReal mass)		;
virtual		bool		TryPosition							(Fvector pos)	{return false;};
virtual		void		SetPhysicsRefObject					(CPhysicsRefObject* ref_object){};
};

class CPHAICharacter : public CPHSimpleCharacter
{
typedef CPHSimpleCharacter	inherited;

Fvector m_vDesiredPosition;
public:
virtual		void		SetPosition							(Fvector pos);
virtual		void		SetDesiredPosition					(const Fvector& pos)									{m_vDesiredPosition.set(pos);}
virtual		void		GetDesiredPosition					(Fvector& dpos)											{dpos.set(m_vDesiredPosition);}
virtual		void		BringToDesired						(float time,float force=1.f)							;
virtual		bool		TryPosition							(Fvector pos)											;

#ifdef DEBUG
virtual		void		OnRender							()														;
#endif

};