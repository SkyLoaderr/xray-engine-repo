#pragma once
#include "gameobject.h"

#include "physicsshellholder.h"

#include "physicsskeletonobject.h"
#include "PHSkeleton.h"
#include "UsableScriptObject.h"
class CSE_ALifeObjectPhysic;

class CPhysicsElement;
class CPhysicObject : 
	public CPhysicsShellHolder,
	public CPHSkeleton,
	public CUsableScriptObject
{
	typedef CPhysicsShellHolder inherited;
	EPOType				m_type;
	float				m_mass;

private:
	//Creating
			void	CreateBody			(CSE_ALifeObjectPhysic	*po)													;
			void	CreateSkeleton		(CSE_ALifeObjectPhysic	*po)													;
			void	AddElement			(CPhysicsElement* root_e, int id)												;

	
public:
	CPhysicObject(void);
	virtual ~CPhysicObject(void);
	virtual BOOL	net_Spawn			( LPVOID DC)																	;

	virtual void	CreatePhysicsShell	(CSE_Abstract* e)																;
	virtual void	net_Destroy			()																				;
	virtual void	Load				(LPCSTR section)																;
	virtual void	shedule_Update		(u32 dt)																		;	//
	virtual void	UpdateCL			()																				;
	virtual void	net_Save			(NET_Packet& P)																	;
	virtual	BOOL	net_SaveRelevant	()																				;
	virtual BOOL	UsedAI_Locations	()																				;
protected:
	virtual void	SpawnInitPhysics	(CSE_Abstract	*D)																;
	virtual CPhysicsShellHolder*	PPhysicsShellHolder	()	{return PhysicsShellHolder();}								;
	virtual	void	InitServerObject	(CSE_Abstract	*po)															;
	virtual void	PHObjectPositionUpdate()																			;

};
