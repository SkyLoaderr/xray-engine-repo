#pragma once
#include "customtarget.h"
#include "PhysicsShell.h"

class CTargetCS :
	public CCustomTarget
{
typedef	CGameObject	inherited;
public:
	CTargetCS(void);
	~CTargetCS(void);

	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent();
	virtual void OnDeviceCreate();
	virtual void OnDeviceDestroy();


	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();
	virtual void			UpdateCL			();								// Called each frame, so no need for dt

	CPhysicsShell*						m_pPhysicsShell;
};
