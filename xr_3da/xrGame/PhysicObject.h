#pragma once
#include "gameobject.h"

#include "physicsshellholder.h"

#include "physicsskeletonobject.h"

class CSE_ALifeObjectPhysic;

class CPhysicsElement;
class CPhysicObject : public CPhysicsSkeletonObject {
	typedef CPhysicsSkeletonObject inherited;
	EPOType				m_type;
	float				m_mass;

private:
	//Creating
			void	CreateBody			(CSE_ALifeObjectPhysic	*po)													;
			void	CreateSkeleton		(CSE_ALifeObjectPhysic	*po)													;
			void	AddElement			(CPhysicsElement* root_e, int id)												;
			void	Init				()																				;
			void	RespawnInit			()																				;
public:
	CPhysicObject(void);
	virtual ~CPhysicObject(void);
	virtual BOOL	net_Spawn			( LPVOID DC)																	;
	virtual void	CreatePhysicsShell	(CSE_Abstract* e)																;
	virtual void	net_Destroy			()																				;
	virtual void	Load				(LPCSTR section)																;
	virtual void	shedule_Update		(u32 dt)																		;	//
	virtual void	net_Save			(NET_Packet& P)																	;
	virtual	BOOL	net_SaveRelevant	()																				;
	virtual BOOL	UsedAI_Locations	()																				;
protected:
	virtual	void	InitServerObject	(CSE_Abstract	*po)															;
	virtual void	PHObjectPositionUpdate()																			;

};
