////////////////////////////////////////////////////////////////////////////
//	Module 		: level_changer.h
//	Created 	: 10.07.2003
//  Modified 	: 10.07.2003
//	Author		: Dmitriy Iassenev
//	Description : Level change object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CustomZone.h"

class CLevelChanger : public CGameObject, public Feel::Touch {
private:
	string32		m_caLevelToChange;
public:
	typedef	CGameObject	inherited;

	virtual			~CLevelChanger		();
	virtual BOOL	net_Spawn			(LPVOID DC);
	virtual void	spatial_register	();
	virtual void	spatial_move		();
			void	UpdateCL			();
	virtual void	feel_touch_new		(CObject* O);
};