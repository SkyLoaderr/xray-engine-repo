////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_go_dir.cpp
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Go In This Direction"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_hen_cover.h"
		_HenGoDirection(Fvector& D) : State(aiHenGoInThisDirection)
		{
			direction.set		(D);
		}
		_HenGoDirection()			: State(aiHenGoInThisDirection)
		{
			direction.set		(0,0,0);
		}
