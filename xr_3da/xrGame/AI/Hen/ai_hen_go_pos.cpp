////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_go_pos.cpp
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Go In This Position"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_hen_cover.h"
		_HenGoPosition(Fvector& P) : State(aiHenGoToThatPosition)
		{
			position.set		(P);
		}
		_HenGoPosition()			: State(aiHenGoToThatPosition)
		{
			position.set		(0,0,0);
		}
