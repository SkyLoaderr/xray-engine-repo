#pragma once

class CDestroyablePhysicsObject :
public	 CPhysicObject,
public	 CPHDestroyable,
public	 CPHCollisionDamageReceiver
{
typedef  CPhysicObject inherited;
	float m_fHealth;
public:
										CDestroyablePhysicsObject									()																																			;
	virtual								~CDestroyablePhysicsObject									()																																			;
	virtual CPhysicsShellHolder*		PPhysicsShellHolder											()																																			;
	virtual BOOL						net_Spawn													(LPVOID DC)																																	;
	virtual	void						net_Destroy													()																																			;
	virtual void						Hit															(float P,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space, float impulse,  ALife::EHitType hit_type = ALife::eHitTypeWound)	;
protected:
private:
};