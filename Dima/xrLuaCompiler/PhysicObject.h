#pragma once
#include "gameobject.h"
class CPhysicsElement;
class CPhysicObject :
	public CGameObject
{
	typedef CGameObject inherited;
	EPOType			m_type;
	float			m_mass;

	void			CreateSkeleton  (CSE_ALifeObjectPhysic	*po);
	void			CreateBody		(CSE_ALifeObjectPhysic	*po);
	void			AddElement		(CPhysicsElement* root_e, int id);
public:
	CPhysicObject(void);
	virtual ~CPhysicObject(void);

	virtual	void	Hit				(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse);
	virtual BOOL	net_Spawn		( LPVOID DC);
	virtual void	UpdateCL		( );									// Called each frame, so no need for dt
	virtual void	net_Export		(NET_Packet& P);
	virtual void	net_Import		(NET_Packet& P);
};
