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
#include "script_space_forward.h"

class CScriptGameObject;
class CScriptCallback;

class CScriptZone : public CGameObject, public Feel::Touch {
	CScriptCallback *m_tpOnEnter;
	CScriptCallback *m_tpOnExit;
public:
	typedef	CGameObject	inherited;

					CScriptZone			();
	virtual			~CScriptZone		();
	virtual void	reinit				();
	virtual BOOL	net_Spawn			(LPVOID DC);
	virtual void	net_Destroy			();

	virtual void	Center				(Fvector& C)	const;
	virtual float	Radius				() const;

	virtual void	shedule_Update		(u32 dt);
	virtual void	feel_touch_new		(CObject* O);
	virtual void	feel_touch_delete	(CObject* O);
	virtual BOOL	feel_touch_contact	(CObject* O);
			void	set_callback		(const luabind::object &lua_object, LPCSTR method, bool bOnEnter);
			void	set_callback		(const luabind::functor<void> &lua_function, bool bOnEnter);
			void	clear_callback		(bool bOnEnter);
	virtual bool	IsVisibleForZones() { return false;		}
#ifdef DEBUG
	virtual	void	OnRender			();
#endif
};