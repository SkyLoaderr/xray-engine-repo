#pragma once

#include "../../entity.h"
#include "../../../SkeletonAnimated.h"

class CMotionDef;
class CBlend;

class CPhantom : public CEntity {
	
	typedef	CEntity inherited;

	MotionID		m_motion;

public:
					CPhantom					();
	virtual			~CPhantom					();
	
	virtual void	Load						( LPCSTR section );
	virtual BOOL	net_Spawn					( CSE_Abstract* DC );
	
	virtual void	net_Export					(NET_Packet& P);
	virtual void	net_Import					(NET_Packet& P);

	virtual void	shedule_Update				(u32 DT); 
	virtual void	UpdateCL					();

	virtual void	HitSignal					(float	HitAmount,	Fvector& local_dir, CObject* who, s16 element);
	virtual void	HitImpulse					(float	amount,		Fvector& vWorldDir, Fvector& vLocalDir) {}
	
	virtual void	Die							(CObject* who){inherited::Die(who);};

	//virtual void	create_physic_shell			() {}

	static void	__stdcall animation_callback	(CBlend* B);

	virtual BOOL	IsVisibleForHUD				() {return false;}
	virtual bool	IsVisibleForZones			() {return false;}

	virtual BOOL	UsedAI_Locations			() {return false;}

	virtual BOOL	renderable_ShadowGenerate	() { return FALSE;	}
	virtual BOOL	renderable_ShadowReceive	() { return FALSE;	}

	virtual CEntity*cast_entity					() {return this;}
};

