#pragma once

#include "../../CustomMonster.h"
#include "../../state_internal.h"

class CCharacterPhysicsSupport;

class CAI_Chimera : 	public CCustomMonster { 
						//public CStateInternal<CAI_Chimera>  {

	typedef CCustomMonster		inherited;


	CCharacterPhysicsSupport	*m_pPhysics_support;
	float						m_fGoingSpeed;

public:
	CMotionDef					*cur_anim;
public:
							CAI_Chimera					();
	virtual					~CAI_Chimera				();	

	virtual	void			Init						();
	virtual	void			Load						(LPCSTR section);
	virtual	void			reinit						();
	virtual void			reload						(LPCSTR	section );	
	virtual BOOL			net_Spawn					(LPVOID DC);
	virtual	void			net_Destroy					();
	virtual	void			net_Import					(NET_Packet& P);
	virtual	void			net_Export					(NET_Packet& P);

	virtual void			UpdateCL					();
	virtual void			shedule_Update				(u32 dt);
	virtual void			Think						();
	virtual void			Die							();

	virtual void			SelectAnimation				(const Fvector& _view, const Fvector& _move, float speed );
	virtual BOOL			feel_vision_isRelevant		(CObject *O);
	
	virtual void			HitSignal					(float amount, Fvector& vLocalDir, CObject* who, s16 element) {}
};

