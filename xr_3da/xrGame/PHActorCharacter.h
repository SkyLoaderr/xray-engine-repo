#pragma once
#include "phsimplecharacter.h"
class CPhysicShellHolder;

class CPHActorCharacter :
	public CPHSimpleCharacter
{
	typedef CPHSimpleCharacter	inherited;
	dGeomID				m_restrictor;
	dGeomID				m_restrictor_transform;
	float				m_restrictor_radius;
static void __stdcall RestrictorCallBack (bool& do_colide,dContact& c,SGameMtl* material_1,SGameMtl* material_2);
public:
	virtual	void		SetObjectContactCallback			(ObjectContactCallbackFun* callback);
	virtual void		SetMaterial							(u16 material);
	virtual void		Create								(dVector3 sizes);
	virtual void		Destroy								(void);
	virtual void		SetPhysicsRefObject					(CPhysicsShellHolder* ref_object);
	virtual void		SetAcceleration						(Fvector accel);
	virtual	void		Disable								();
	virtual	void		Jump								(const Fvector& jump_velocity);
	virtual void		InitContact							(dContact* c,bool &do_collide,SGameMtl *material_1 ,SGameMtl * material_2);
						CPHActorCharacter					(float restrictor_radius);
	virtual				~CPHActorCharacter					(void);
};
