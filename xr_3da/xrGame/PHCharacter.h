#pragma once
#include "PHObject.h"
#include "PHInterpolation.h"

typedef	 void __stdcall ObjectContactCallbackFun(bool& do_colide,	dContact& c);
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
////////////////////////////damage////////////////////////////////////////
float					m_contact_velocity;
////////////////////////// dynamic

CPHInterpolation m_body_interpolation;
dBodyID				m_body;
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

	void	Enable										()										{dBodyEnable(m_body);}
	bool	IsEnabled									()										{ if(!b_exist)return false; return !!dBodyIsEnabled(m_body);}
	dBodyID GetBody										()										{return m_body;}
	float	ContactVelocity								()										{ dReal ret= m_contact_velocity; m_contact_velocity=0; return ret;}
	bool b_exist;

public:
void		SetPLastMaterial								(u32* p)								{p_lastMaterial=p;}

virtual void			SetMaximumVelocity					(dReal vel)								{}
virtual	void			SetJupmUpVelocity					(dReal velocity)						{}
virtual	void			IPosition							(Fvector& pos)							{}
virtual u16				ContactBone							()										{return 0;}
virtual void			DeathPosition						(Fvector& deathPos)						{}
virtual		void		ApplyImpulse						(const Fvector& dir,const dReal P)		{}	;
virtual		void		ApplyForce							(const Fvector& force)					=0 ;
virtual		void		ApplyForce							(const Fvector& dir,float force)		=0 ;
virtual		void		ApplyForce							(float x,float y, float z)				=0 ;
virtual		void		Jump								(const Fvector& jump_velocity)			=0 ;
virtual		EEnvironment CheckInvironment					()				=0	;
virtual		bool		ContactWas							()				=0	;
virtual		void		Create								(dVector3 sizes)=0	;
virtual		void		Destroy								(void)			=0	;
virtual		void		SetAcceleration						(Fvector accel)	=0	;
virtual		void		SetPosition							(Fvector pos)	=0	;

virtual		void		SetObjectContactCallback			(ObjectContactCallbackFun* callback)=0 ;
virtual		void		GetVelocity							(Fvector& vvel)	=0	;
virtual		void		SetVelocity							(Fvector vel)	=0	;
virtual		void		GetPosition							(Fvector& vpos)	=0	;
virtual		void		SetMas								(dReal mass)	=0	;
virtual		void		SetPhysicsRefObject					(CPhysicsRefObject* ref_object)=0;
virtual		CPhysicsRefObject* PhysicsRefObject				(){return m_phys_ref_object;}

//AICharacter
virtual		void		GetDesiredPosition					(Fvector& dpos)	{}
virtual		void		SetDesiredPosition					(const Fvector& pos)	{}
virtual		void		BringToDesired						(float time,float velocity,float force=1.f){}	;
virtual		bool		TryPosition							(Fvector pos)	{return false;}	

			CPHCharacter									(void)				;
virtual		~CPHCharacter									(void)				;
};


