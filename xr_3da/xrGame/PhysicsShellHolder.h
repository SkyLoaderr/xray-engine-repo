#ifndef PHYSICSSHELL_HOLDER_H
#define PHYSICSSHELL_HOLDER_H

#include "GameObject.h"
#include "ParticlesPlayer.h"


class CPHDestroyable;
class CPHCollisionDamageReceiver;
class CPHSoundPlayer;
class IDamageSource;
class CPhysicsShellHolder:  public CGameObject,
							public CParticlesPlayer
	
{
public:
	typedef CGameObject inherited;
	

	CPhysicsShell			*m_pPhysicsShell;


	CPhysicsShellHolder							();



	IC CPhysicsShell	*&PPhysicsShell				()		
	{
		return m_pPhysicsShell;
	}

	IC CPhysicsShellHolder*	PhysicsShellHolder	()
	{
		return this;
	}
	virtual CPHDestroyable				*ph_destroyable		(){return NULL;}
	virtual CPHCollisionDamageReceiver	*PHCollisionDamageReceiver(){return NULL;}
	virtual CPhysicsShellHolder*		cast_physics_shell_holder	()	{return this;}
	virtual CParticlesPlayer*			cast_particles_player		()	{return this;}
	virtual IDamageSource*				cast_IDamageSource			()	{return NULL;}
	virtual CPHSoundPlayer*				ph_sound_player				()  {return NULL;}	
	virtual void						enable_notificate			()	{}
public:
	virtual void			PHGetLinearVell		(Fvector& velocity);
	virtual void			PHSetLinearVell		(Fvector& velocity);
	virtual void			PHSetMaterial		(LPCSTR m);
	virtual void			PHSetMaterial		(u16 m);
	virtual void			PHSetPushOut		(u32 time = 5000);
	virtual f32				GetMass				();
	virtual	void			PHHit				(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type=ALife::eHitTypeWound);
	virtual	void			Hit					(float P, Fvector &dir,	CObject* who, 
		s16 element,Fvector p_in_object_space, 
		float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
///////////////////////////////////////////////////////////////////////
	virtual u16				PHGetSyncItemsNumber();
	virtual CPHSynchronize*	PHGetSyncItem		(u16 item);
	virtual void			PHUnFreeze			();
	virtual void			PHFreeze			();

///////////////////////////////////////////////////////////////
	virtual void			create_physic_shell		();
	virtual void			activate_physic_shell	();
	virtual void			setup_physic_shell		();

	virtual void			net_Destroy			();
	virtual BOOL			net_Spawn			(CSE_Abstract*	DC);
			void			init				();

	//для наследования CParticlesPlayer
	virtual IRender_Sector*	Sector				()		{return inherited::Sector();}
	virtual void			UpdateCL			();
};

#endif