#pragma once
#include "phsimplecharacter.h"

class CPHActorCharacter :
	public CPHSimpleCharacter
{
	typedef CPHSimpleCharacter	inherited;

public:
	virtual	void		SetObjectContactCallback			(ObjectContactCallbackFun* callback);
	virtual void		Create								(dVector3 sizes);
	virtual void		Destroy								(void);
	virtual void		SetPhysicsRefObject					(CGameObject* ref_object);
	virtual void		SetAcceleration						(Fvector accel);
	virtual	void		Disable								();
	virtual	void		Jump								(const Fvector& jump_velocity);
						CPHActorCharacter					(void);
	virtual				~CPHActorCharacter					(void);
};
