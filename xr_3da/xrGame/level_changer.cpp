////////////////////////////////////////////////////////////////////////////
//	Module 		: level_changer.cpp
//	Created 	: 10.07.2003
//  Modified 	: 10.07.2003
//	Author		: Dmitriy Iassenev
//	Description : Level change object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_changer.h"

void CLevelChanger::Affect		(CObject	*tpObject)
{
	CActor	*l_tpActor = dynamic_cast<CActor*>(tpObject);
	if (l_tpActor)
		Level().IR_OnKeyboardPress(DIK_F7);
}