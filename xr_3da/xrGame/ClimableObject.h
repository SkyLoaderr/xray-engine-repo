#pragma once
#include "physicsshellholder.h"
class CPHStaticGeomShell;
class CClimableObject: public CPhysicsShellHolder 
{
	typedef	CPhysicsShellHolder		inherited;
	CPHStaticGeomShell* m_pStaticShell;
public:
					CClimableObject		();
					~CClimableObject	();
	virtual void	Load				( LPCSTR section);
	virtual BOOL	net_Spawn			( LPVOID DC);
	virtual	void	net_Destroy			();
	virtual void	shedule_Update		( u32 dt);							// Called by sheduler
	virtual void	UpdateCL			( );								// Called each frame, so no need for dt
};
