#pragma once

#include "gameobject.h"
#include "HelicopterMovementManager.h"

class CHelicopter : 
	public CEntity

{
	typedef CGameObject inherited;
protected:
	ref_sound						m_engine_sound;
	CHelicopterMovementManager		m_movementMngr;
public:
	CHelicopter();
	virtual				~CHelicopter();
	
	//CAI_ObjectLocation
	void				init();
	virtual	void		reinit();


	//CGameObject
	virtual	void		Load				(LPCSTR		section);
	virtual	void		reload				(LPCSTR		section);

	virtual BOOL		net_Spawn			(LPVOID		DC);
	virtual void		net_Destroy			();
	virtual void		net_Export			(NET_Packet &P);
	virtual void		net_Import			(NET_Packet &P);

	virtual void		renderable_Render	();

	virtual void		UpdateCL			();
	virtual void		shedule_Update		(u32		time_delta);

	//CEntity
	virtual void		HitSignal			(float P, Fvector &local_dir,	CObject* who, s16 element){;}
	virtual void		HitImpulse			(float P, Fvector &vWorldDir, 	Fvector& vLocalDir){;}
	virtual void		Die					(){;}

};
