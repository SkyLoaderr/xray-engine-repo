////////////////////////////////////////////////////////////////////////////
//	Module 		: level_changer.h
//	Created 	: 10.07.2003
//  Modified 	: 10.07.2003
//	Author		: Dmitriy Iassenev
//	Description : Level change object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CustomZone.h"

class CLevelChanger : public CCustomZone {
public:
	typedef CCustomZone inherited;
	virtual void Affect		(CObject	*tpObject);
};