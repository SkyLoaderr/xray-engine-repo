////////////////////////////////////////////////////////////////////////////
//	Module 		: script_zone.h
//	Created 	: 10.10.2003
//  Modified 	: 10.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Script zone object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"
#include "../feel_touch.h"
#include "ai_script_space.h"

class CLuaGameObject;

class CScriptZone : public CGameObject, public Feel::Touch {
	luabind::functor<void>	*m_tpOnEnter;
	luabind::functor<void>	*m_tpOnExit;
public:
	typedef	CGameObject	inherited;

					CScriptZone			();
	virtual			~CScriptZone		();
	virtual void	reinit				();
	virtual BOOL	net_Spawn			(LPVOID DC);

//	virtual void	spatial_register	();
//	virtual void	spatial_move		();
	virtual void	Center				(Fvector& C);
	virtual float	Radius				();

			void	UpdateCL			();
	virtual void	feel_touch_new		(CObject* O);
	virtual void	feel_touch_delete	(CObject* O);
	virtual BOOL	feel_touch_contact	(CObject* O);
			void	set_callback		(const luabind::functor<void> &tpZoneCallback, bool bOnEnter);
			void	clear_callback		(bool bOnEnter);
#ifdef DEBUG
	virtual	void	OnRender			();
#endif
};