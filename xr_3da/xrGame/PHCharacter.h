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
list<CPHObject*>::iterator m_ident;
CPHInterpolation m_body_interpolation;
dBodyID m_body;
CPhysicsRefObject* m_phys_ref_object;
////////////////////////// geometry
dBodyID m_wheel_body;
dGeomID m_geom_shell;
dGeomID m_wheel;
dGeomID	m_cap;
dGeomID m_hat;
dGeomID m_hat_transform;
dGeomID m_geom_group;
dGeomID m_wheel_transform;
dGeomID m_shell_transform;
dGeomID m_cap_transform;

/////////////////////////////
dJointID m_wheel_joint;
/////////////////////////////

///////////////////////// movement
dVector3 m_control_force;
Fvector	 m_acceleration;	
dVector3 m_wall_contact_normal;
dVector3 m_ground_contact_normal;
dVector3 m_clamb_depart_position;
dVector3 m_depart_position;
dVector3 m_wall_contact_position;
dVector3 m_ground_contact_position;
dReal	 m_jump_depart_hight;
dReal	 jump_up_velocity;//=6.0f;//5.6f;

dReal m_mass;
dReal m_max_velocity;
Fvector m_position;
Fvector m_velocity;
dVector3 m_safe_velocity;
dVector3 m_safe_position;
dVector3 m_jump_depart_position;
dVector3 m_death_position;
Fvector  m_jump_accel;
float m_contact_velocity;
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
dReal m_radius;
dReal m_cyl_hight;
dReal m_friction_factor;
////////////////////////////////////////////////////////////////////////////
/////disable////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
dVector3 previous_p;
dVector3 previous_p1;
dReal previous_v;
u32 dis_count_f;
u32 dis_count_f1;
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
u32* p_lastMaterial;


void Disable										();

public:
	void Enable											(){dBodyEnable(m_body);}
	bool IsEnabled										(){
															if(!b_exist)return false;
															return !!dBodyIsEnabled(m_body);
															}
	bool b_exist;
	float m_update_time;
public:
void		SetPLastMaterial								(u32* p){p_lastMaterial=p;}
void		SetMaximumVelocity								(dReal vel){m_max_velocity=vel; }
void		SetJupmUpVelocity								(dReal velocity){jump_up_velocity=velocity;}
const Fvector&	Position									() {return m_position;}
const Fvector&	Velocity									() {return m_velocity;}
float	ContactVelocity										() 
{
	dReal ret= m_contact_velocity;
	m_contact_velocity=0;
	return ret;
}
Fvector			IPosition									() {
																Fvector pos;
																if(!b_exist){
																	pos.set(m_safe_position[0],
																			m_safe_position[1],
																			m_safe_position[2]);
																	return pos;
																}
																m_body_interpolation.InterpolatePosition(pos);
																pos.y-=m_radius;
																return pos;
																}
Fvector			DeathPosition								(){
																Fvector pos;
																pos.x=m_death_position[0];
																pos.y=m_death_position[1]-m_radius;
																pos.z=m_death_position[2];
																return pos;
																}
void		ApplyImpulse(const Fvector& dir,const dReal P);
virtual		EEnvironment CheckInvironment					()				=0	;
virtual		bool		ContactWas							()				=0	;
virtual		void		Create							(dVector3 sizes)	=0	;
virtual		void		Destroy								(void)			=0	;
virtual		void		SetAcceleration						(Fvector accel)	=0	;
virtual		void		SetPosition							(Fvector pos)	=0	;
virtual		bool		TryPosition							(Fvector pos)	=0	;

virtual		Fvector		GetVelocity							(void)			=0	;
virtual		void		SetVelocity							(Fvector vel)	=0	;
virtual		Fvector		GetPosition							(void)			=0	;
virtual		void		SetMas								(dReal mass)	=0	;
virtual		void		SetPhysicsRefObject					(CPhysicsRefObject* ref_object)=0;

			CPHCharacter									(void)				;
virtual		~CPHCharacter									(void)				;
};

class CPHSimpleCharacter : public CPHCharacter {

/////////////////CPHObject//////////////////////////////////////////////
virtual		void		PhDataUpdate						(dReal step)		;
virtual		void		PhTune								(dReal step)		;
virtual		void		InitContact							(dContact* c)		;
virtual		void		StepFrameUpdate						(dReal step)		;
/////////////////CPHCharacter////////////////////////////////////////////
public:
virtual		bool		ContactWas							(){if(b_meet_control) 
																{b_meet_control=false;return true;}
																else return false	;}
virtual		EEnvironment	 CheckInvironment				()					;
virtual		void		Create								(dVector3 sizes)	;
virtual		void		Destroy								(void)				;
virtual		void		SetAcceleration						(Fvector accel)		;
virtual		void		SetPosition							(Fvector pos)		;
virtual		Fvector		GetVelocity							(void)				;
virtual		void		SetVelocity							(Fvector vel)		;
virtual		Fvector		GetPosition							(void)				;
virtual		void		SetMas								(dReal mass)		;
virtual		bool		TryPosition							(Fvector pos)		;
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
/////////////////CPHObject//////////////////////////////////////////////
virtual		void		PhDataUpdate						(dReal step)	{}	;
virtual		void		PhTune								(dReal step)	{}	;
virtual		void		InitContact							(dContact* c)	{}	;
/////////////////CPHCharacter////////////////////////////////////////////
public:
virtual		EEnvironment CheckInvironment					()					;
virtual		bool		ContactWas							(){return b_meet;}	;
virtual		void		Create							  (dVector3 sizes);
virtual		void		Destroy								(void)				;
virtual		void		SetAcceleration						(Fvector accel)	{}	;
virtual		void		SetPosition							(Fvector pos)	{}	;
virtual		Fvector		GetVelocity							(void)			{}	;
virtual		void		SetVelocity							(Fvector vel)	{}	;
virtual		Fvector		GetPosition							(void)			{}	;
virtual		void		SetMas								(dReal mass)		;
virtual		bool		TryPosition							(Fvector pos)	{}	;
virtual		void		SetPhysicsRefObject					(CPhysicsRefObject* ref_object){};
};


//class CPHStalkerCharacter : public CPHCharacter 