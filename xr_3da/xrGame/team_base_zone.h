////////////////////////////////////////////////////////////////////////////
//	Module 		: team_base_zone.h
//	Created 	: 27.04.2004
//  Modified 	: 27.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Team base zone object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"
#include "../feel_touch.h"

class CTeamBaseZone : public CGameObject, public Feel::Touch {
protected:

public:
	typedef	CGameObject	inherited;

					CTeamBaseZone		();
	virtual			~CTeamBaseZone		();
	virtual void	reinit				();
	virtual BOOL	net_Spawn			(LPVOID DC);

	virtual void	Center				(Fvector &C)	const;
	virtual float	Radius				() const;

			void	UpdateCL			();
	virtual void	feel_touch_new		(CObject* O);
	virtual void	feel_touch_delete	(CObject* O);
	virtual BOOL	feel_touch_contact	(CObject* O);
#ifdef DEBUG
	virtual	void	OnRender			();
#endif
};