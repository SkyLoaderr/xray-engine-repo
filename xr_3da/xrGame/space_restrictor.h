////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"

class CSpaceRestrictor : public CGameObject {
private:
	typedef CGameObject inherited;

public:
	IC				CSpaceRestrictor	();
	virtual			~CSpaceRestrictor	();
	virtual	BOOL	net_Spawn			(LPVOID data);
			bool	inside				(const Fvector &position, float radius = EPS_L) const;
	virtual void	Center				(Fvector &C) const;
	virtual float	Radius				() const;
#ifdef DEBUG
	virtual void	OnRender			();
#endif
};

#include "space_restrictor_inline.h"