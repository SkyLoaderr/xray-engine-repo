#ifndef PHYSICSSHELL_HOLDER_H
#define PHYSICSSHELL_HOLDER_H
#include "GameObject.h"

class CPhysicsShellHolder: 
	virtual public CGameObject
{
public:
	typedef CGameObject inherited;
	

	CPhysicsShell			*m_pPhysicsShell;


	CPhysicsShellHolder							();



	IC CPhysicsShell	*&PPhysicsShell				()		
	{
		return m_pPhysicsShell;
	}
protected:
	IC CPhysicsShellHolder*	PhysicsShellHolder	()
	{
		return this;
	}
public:
	virtual void			PHGetLinearVell		(Fvector& velocity);
	virtual void			PHSetLinearVell		(Fvector& velocity);
	virtual void			PHSetMaterial		(LPCSTR m);
	virtual void			PHSetMaterial		(u16 m);
	virtual void			PHSetPushOut		(u32 time = 5000);
	virtual f32				GetMass				();
	virtual	void			PHHit				(Fvector &dir,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type=ALife::eHitTypeWound);
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
			void			Init				();

protected:
private:
};

#endif