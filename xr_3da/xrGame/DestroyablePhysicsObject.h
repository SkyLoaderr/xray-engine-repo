#pragma once

class CDestroyablePhysicsObject :
public	 CPhysicObject,
public	 CPHDestroyable,
public	 CPHCollisionDamageReceiver,
public	 CHitImmunity,
public	 CDamageManager
{
typedef  CPhysicObject inherited;
	float m_fHealth;
	ref_sound							m_destroy_sound;
	shared_str							m_destroy_particles;
public:
										CDestroyablePhysicsObject									()																																			;
	virtual								~CDestroyablePhysicsObject									()																																			;
	virtual CPhysicsShellHolder*		PPhysicsShellHolder											()																																			;
	virtual BOOL						net_Spawn													(CSE_Abstract* DC)																																	;
	virtual	void						net_Destroy													()																																			;
	virtual void						Hit															(float P,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space, float impulse,  ALife::EHitType hit_type = ALife::eHitTypeWound)	;
	virtual	void						InitServerObject			(CSE_Abstract*				D)																																				;
	virtual CPHCollisionDamageReceiver	*PHCollisionDamageReceiver	()								{return static_cast<CPHCollisionDamageReceiver*>(this);}
	virtual DLL_Pure					*_construct					()								;
	virtual CPhysicsShellHolder*		cast_physics_shell_holder	()								{return this;}
	virtual CParticlesPlayer*			cast_particles_player		()								{return this;}


protected:
			void						Destroy						()								;
private:
};