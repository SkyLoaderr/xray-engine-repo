#pragma once
#include "gameobject.h"

class CCustomZone :
	public CGameObject
{
public:
	CCustomZone(void);
	virtual ~CCustomZone(void);
	virtual	void			Hit					(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space){}
};
