////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_h_pos_u_f.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Hold Position Uder Fire"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_HOLD_POSITION_UNDER_FIRE__
#define __XRAY_AI_HEN_HOLD_POSITION_UNDER_FIRE__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenHoldPositionUnderFire : public HenState
	{
		public:
			HenHoldPositionUnderFire(): State(aiHenHoldPositionUnderFire);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

#endif