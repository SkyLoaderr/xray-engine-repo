#pragma once
#include "gameobject.h"

class CPhysicObject :
	public CGameObject
{
	typedef CGameObject inherited;
public:
	CPhysicObject(void);
	virtual ~CPhysicObject(void);

	virtual BOOL	net_Spawn		( LPVOID DC);

	void			AddElement		(CPhysicsElement* root_e, int id);
	void			CreateBody		();

	EPOType m_type;
};
