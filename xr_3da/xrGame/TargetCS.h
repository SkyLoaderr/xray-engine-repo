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
	virtual void			UpdateCL			();					// Called each frame, so no need for dt
	virtual BOOL			ShadowGenerate		()					{ return FALSE;	}
	virtual BOOL			ShadowReceive		()					{ return FALSE;	}
	virtual	void			Hit					(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space){
												m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,P/5.f);
												}
	void SetPos									(const Fmatrix& pos);

	CPhysicsShell*			m_pPhysicsShell;
	Fmatrix					m_pos;
};
