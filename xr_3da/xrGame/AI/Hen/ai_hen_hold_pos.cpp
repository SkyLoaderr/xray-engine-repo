////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_hold_pos.cpp
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Hold This Position"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_hen_cover.h"
		Fvector					position;
		DWORD					nodeID;

		_HenHoldPosition(Fvector& P) : State(aiHenHoldPosition)
		{
			position.set		(P);
		}
		_HenHoldPosition()			: State(aiHenHoldPosition)
		{
			position.set		(0,0,0);
		}
