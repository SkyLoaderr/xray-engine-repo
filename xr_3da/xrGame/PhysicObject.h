#pragma once
#include "gameobject.h"

class CPhysicObject :
	public CGameObject
{
	typedef CGameObject inherited;
public:
	CPhysicObject(void);
	virtual ~CPhysicObject(void);

	virtual	void			Hit					(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse);

	virtual BOOL	net_Spawn		( LPVOID DC);

	virtual void	UpdateCL		( );									// Called each frame, so no need for dt
	
	void			AddElement		(CPhysicsElement* root_e, int id);
	void			CreateBody		();

	EPOType m_type;
	float m_mass;
};
