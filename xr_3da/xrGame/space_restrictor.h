////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor
////////////////////////////////////////////////////////////////////////////

#pragma once

class CSpaceRestrictor : public CObject {
private:
	typedef CObject inherited;

public:
	IC				CSpaceRestrictor	();
	virtual			~CSpaceRestrictor	();
	virtual	BOOL	net_Spawn			(LPVOID data);
			bool	inside				(const Fvector &position, float radius = EPS_L) const;
};

#include "space_restrictor_inline.h"