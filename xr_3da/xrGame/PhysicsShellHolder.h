#ifndef PHYSICSSHELL_HOLDER_H
#define PHYSICSSHELL_HOLDER_H

#include "GameObject.h"
#include "ParticlesPlayer.h"


class CPHDestroyable;
class CPHCollisionDamageReceiver;

class CPhysicsShellHolder:  virtual public CGameObject,
									public CParticlesPlayer
	
{
private:
	u32						m_dwFrameSpawn;
	u32						m_dwFrameDestroy;
	u32						m_dwFrameClient;

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
public:
	virtual void			PHGetLinearVell		(Fvector& velocity);
	virtual void			PHSetLinearVell		(Fvector& velocity);
	virtual void			PHSetMaterial		(LPCSTR m);
	virtual void			PHSetMaterial		(u16 m);
	virtual void			PHSetPushOut		(u32 time = 5000);
	virtual f32				GetMass				();
	virtual	void			PHHit				(float P,Fvector &dir,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type=ALife::eHitTypeWound);
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
	virtual BOOL			net_Spawn			(LPVOID	DC);
			void			init				();

	//��� ������������ CParticlesPlayer
	virtual IRender_Sector*	Sector				()		{return inherited::Sector();}
	virtual void			UpdateCL			();
};

#endif