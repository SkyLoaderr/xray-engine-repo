#pragma once
#include "phsimplecharacter.h"

class CPHActorCharacter :
	public CPHSimpleCharacter
{
	typedef CPHSimpleCharacter	inherited;
	dGeomID	m_cap;
	dGeomID m_cap_transform;
public:
	virtual	void		InitContact							(dContact* c)		;
	virtual void		Create								(dVector3 sizes);
	virtual void		Destroy								(void);
	virtual void		SetPhysicsRefObject					(CPhysicsRefObject* ref_object);
						CPHActorCharacter					(void);
	virtual				~CPHActorCharacter					(void);
};
