#pragma once
#include "phsimplecharacter.h"

class CPHActorCharacter :
	public CPHSimpleCharacter
{
	typedef CPHSimpleCharacter	inherited;
	dGeomID	m_cap;
	dGeomID m_cap_transform;
public:
	virtual	void		SetObjectContactCallback			(ObjectContactCallbackFun* callback);
	virtual	void		InitContact							(dContact* c)		;
	virtual void		Create								(dVector3 sizes);
	virtual void		Destroy								(void);
	virtual void		SetPhysicsRefObject					(CPhysicsRefObject* ref_object);
	virtual void		SetAcceleration						(Fvector accel);
	virtual	void		Jump								(const Fvector& jump_velocity){};
						CPHActorCharacter					(void);
	virtual				~CPHActorCharacter					(void);
};
