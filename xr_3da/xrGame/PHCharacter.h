#pragma once
#include "PHObject.h"

class CPHCharacter : public CPHObject
{
protected:
dBodyID m_body;
dBodyID m_wheel_body;
dGeomID m_geom_shell;
dGeomID m_wheel;
dGeomID	m_cap;
dGeomID m_geom_group;
dGeomID m_wheel_transform;
dGeomID m_shell_transform;
dGeomID m_cap_transform;
dJointID m_wheel_joint;
dVector3 m_control_accel;
dVector3 m_contact_normal;

bool is_contact;
bool was_contact;
bool is_control;
bool b_depart;
bool b_meet;


public:

virtual		void		Create								(void)			=0	;
virtual		void		Destroy								(void)			=0	;
virtual		void		SetAcceleration						(Fvector accel)	=0	;
virtual		void		SetPosition							(Fvector pos)	=0	;


virtual		Fvector		GetVelocity							(void)			=0	;
virtual		void		SetVelocity							(Fvector vel)	=0	;
virtual		Fvector		GetPosition							(void)			=0	;

			CPHCharacter									(void)				;
			~CPHCharacter									(void)				;
};

class CPHSimpleCharacter : public CPHCharacter {

/////////////////CPHObject//////////////////////////////////////////////
virtual		void		PhDataUpdate						(dReal step)		;
virtual		void		PhTune								(dReal step)		;
virtual		void		InitContact							(dContact* c)		;
/////////////////CPHCharacter////////////////////////////////////////////
public:
virtual		void		Create								(void)				;
virtual		void		Destroy								(void)				;
virtual		void		SetAcceleration						(Fvector accel)		;
virtual		void		SetPosition							(Fvector pos)		;
virtual		Fvector		GetVelocity							(void)				;
virtual		void		SetVelocity							(Fvector vel)		;
virtual		Fvector		GetPosition							(void)				;
};


class CPHWheeledCharacter : public CPHCharacter {
/////////////////CPHObject//////////////////////////////////////////////
virtual		void		PhDataUpdate						(dReal step)	{}	;
virtual		void		PhTune								(dReal step)	{}	;
virtual		void		InitContact							(dContact* c)	{}	;
/////////////////CPHCharacter////////////////////////////////////////////
public:
virtual		void		Create								(void)				;
virtual		void		Destroy								(void)				;
virtual		void		SetAcceleration						(Fvector accel)	{}	;
virtual		void		SetPosition							(Fvector pos)	{}	;
virtual		Fvector		GetVelocity							(void)			{}	;
virtual		void		SetVelocity							(Fvector vel)	{}	;
virtual		Fvector		GetPosition							(void)			{}	;
};