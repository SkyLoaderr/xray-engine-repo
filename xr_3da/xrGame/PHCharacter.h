#pragma once
#include "PHObject.h"
#include "PHInterpolation.h"
#include "PHSynchronize.h"
#include "PHDisabling.h"
class CPhysicsShellHolder;
class CClimableObject;
class CGameObject;
 static enum EEnvironment
			{
				peOnGround,
				peAtWall,
				peInAir
			};


class CPHCharacter : 
	public CPHObject,
	public CPHSynchronize,
	public CPHDisablingTranslational
#ifdef DEBUG
	,public pureRender
#endif
{
public:

bool b_exist;

protected:
////////////////////////////damage////////////////////////////////////////
float					m_contact_velocity;
////////////////////////// dynamic

CPHInterpolation m_body_interpolation;
dBodyID				m_body;
CPhysicsShellHolder* m_phys_ref_object;


dReal m_mass;
bool					was_enabled_before_freeze;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
u16* p_lastMaterial;
///////////////////////////////////////////////////////////////////////////
dVector3 m_safe_velocity;
dVector3 m_safe_position;




public:
	virtual ECastType	CastType							(){return CPHObject::tpCharacter;}
	virtual void		FreezeContent						();
	virtual void		UnFreezeContent						();
	virtual	dBodyID		get_body							()															{return m_body;}
	virtual	void		Disable								()															;																		
	virtual	void		ReEnable							()															{;}																				
			void		Enable								()															;											//!!
			bool		IsEnabled							()															{ if(!b_exist)return false; return !!dBodyIsEnabled(m_body);}
			float		ContactVelocity						()															{ dReal ret= m_contact_velocity; m_contact_velocity=0; return ret;}			//!!



void					SetPLastMaterial					(u16* p)													{p_lastMaterial=p;}													
virtual void			SetElevator							(CClimableObject* climable)									{};
virtual void			SetMaterial							(u16 material)												=0 ;
virtual void			SetMaximumVelocity					(dReal /**vel/**/)											{}																			//!!
virtual		dReal		GetMaximumVelocity					()															{return 0;}
virtual	void			SetJupmUpVelocity					(dReal /**velocity/**/)										{}																			//!!
virtual	void			IPosition							(Fvector& /**pos/**/)										{}
virtual u16				ContactBone							()															{return 0;}
virtual void			DeathPosition						(Fvector& /**deathPos/**/)									{}
virtual		void		ApplyImpulse						(const Fvector& /**dir/**/,const dReal /**P/**/)			{}
virtual		void		ApplyForce							(const Fvector& force)										=0 ;
virtual		void		ApplyForce							(const Fvector& dir,float force)							=0 ;
virtual		void		ApplyForce							(float x,float y, float z)									=0 ;
virtual		void		AddControlVel						(const Fvector& vel)										=0 ;
virtual		void		Jump								(const Fvector& jump_velocity)								=0 ;
virtual		bool		JumpState							()															=0 ;
virtual		EEnvironment CheckInvironment					()															=0 ;
virtual		bool		ContactWas							()															=0 ;
virtual		void		GroundNormal						(Fvector &norm)												=0 ;
virtual		void		Create								(dVector3 /**sizes/**/)										=0 ;
virtual		void		Destroy								(void)														=0 ;
virtual		void		SetAcceleration						(Fvector accel)												=0 ;
virtual		Fvector		GetAcceleration						()															=0 ;
virtual		void		SetPosition							(Fvector pos)												=0 ;
virtual		void		SetApplyGravity						(BOOL flag)						{ dBodySetGravityMode(m_body,flag); }
virtual		void		SetObjectContactCallback			(ObjectContactCallbackFun* callback)						=0 ;
virtual		ObjectContactCallbackFun* ObjectContactCallBack	()															{return NULL;}
virtual		void		GetVelocity							(Fvector& vvel)												=0 ;
virtual		void		SetVelocity							(Fvector vel)												=0 ;
virtual		void		SetAirControlFactor					(float factor)												=0 ;
virtual		void		GetPosition							(Fvector& vpos)												=0 ;
virtual		void		GetFootCenter						(Fvector& vpos)												{vpos.set(*(Fvector*)dBodyGetPosition(m_body));}
virtual		void		SetMas								(dReal mass)												=0 ;
virtual		void		SetPhysicsRefObject					(CPhysicsShellHolder* ref_object)									=0 ;
virtual		CPhysicsShellHolder* PhysicsRefObject					()									{return m_phys_ref_object;}

//AICharacter
virtual		void		GetDesiredPosition					(Fvector& /**dpos/**/)										{}
virtual		void		SetDesiredPosition					(const Fvector& /**pos/**/)									{}
virtual		void		BringToDesired						(float /**time/**/,float /**velocity/**/,float force=1.f)	{}
virtual		bool		TryPosition							(Fvector /**pos/**/)										{return false;}
virtual		void		getForce							(Fvector& force)											;
virtual		void		setForce							(const	Fvector& force)										;
virtual		float		FootRadius							()															=0 ;
virtual		void		get_State							(		SPHNetState&	state)								;
virtual		void		set_State							(const	SPHNetState&	state)								;
virtual		void		cv2obj_Xfrom						(const Fquaternion& q,const Fvector& pos, Fmatrix& xform)	{;}
virtual		void		cv2bone_Xfrom						(const Fquaternion& q,const Fvector& pos, Fmatrix& xform)	{;}
virtual	const Fvector&  ControlAccel						()const														=0;
			CPHCharacter									(void)														;
virtual		~CPHCharacter									(void)														;
};


