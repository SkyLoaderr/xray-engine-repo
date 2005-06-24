#pragma once
#include "gameobject.h"

#include "physicsshellholder.h"

#include "physicsskeletonobject.h"
#include "PHSkeleton.h"
class CSE_ALifeObjectPhysic;

class CPhysicsElement;
class CPhysicObject : 
	public CPhysicsShellHolder,
	public CPHSkeleton
{
	typedef CPhysicsShellHolder inherited;
	EPOType				m_type;
	float				m_mass;
	bool				b_sheduled;
protected:
			void	SheduleRegister		(){if(!b_sheduled)shedule_register();b_sheduled=true;}
			void	SheduleUnregister	(){if(b_sheduled)shedule_unregister();b_sheduled=false;}
private:
	//Creating
			void	CreateBody			(CSE_ALifeObjectPhysic	*po)													;
			void	CreateSkeleton		(CSE_ALifeObjectPhysic	*po)													;
			void	AddElement			(CPhysicsElement* root_e, int id)												;

	
public:
	CPhysicObject(void);
	virtual ~CPhysicObject(void);
IC			bool	IsSheduled			(){return b_sheduled;}	
	virtual BOOL	net_Spawn			( CSE_Abstract* DC)																	;
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
	virtual void	RunStartupAnim		(CSE_Abstract	*D)																;
	virtual CPhysicsShellHolder*	PPhysicsShellHolder	()	{return PhysicsShellHolder();}								;
	virtual CPHSkeleton				*PHSkeleton			()	{return this;}
	virtual	void	InitServerObject	(CSE_Abstract	*po)															;
	virtual void	PHObjectPositionUpdate()																			;

};
