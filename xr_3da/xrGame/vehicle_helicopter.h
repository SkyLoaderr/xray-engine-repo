////////////////////////////////////////////////////////////////////////////
//	Module 		: vehicle_helicopter.h
//	Created 	: 13.02.2004
//  Modified 	: 13.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Vehicle helicopter class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"
#include "../feel_touch.h"

class CObjectAnimator;
class CMotionDef;

class CVehicleHelicopter : 
	public CGameObject,
	public Feel::Touch
{
	typedef CGameObject inherited;
protected:
	CObjectAnimator	*m_animator;
	ref_str			m_animation_sting;
	CMotionDef		*m_animation;
public:
					CVehicleHelicopter	();
	virtual			~CVehicleHelicopter	();
			void	init				();
	virtual	void	Load				(LPCSTR		section);
	virtual	void	reinit				();
	virtual	void	reload				(LPCSTR		section);
	virtual BOOL	net_Spawn			(LPVOID		DC);
	virtual void	net_Destroy			();
	virtual void	net_Export			(NET_Packet &P);
	virtual void	net_Import			(NET_Packet &P);
	virtual void	renderable_Render	();
	virtual void	UpdateCL			();
	virtual void	shedule_Update		(u32		time_delta);
	virtual void	feel_touch_new		(CObject	*O);
	virtual void	feel_touch_delete	(CObject	*O);
	virtual BOOL	feel_touch_contact	(CObject	*O);
protected:
			void	select_animation	();

	IC		const CObjectAnimator	*animator	() const;
	IC		const ref_str			&animation	() const;
};

#include "vehicle_helicopter_inline.h"