////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_hold_pos.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Hold This Position"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_HOLD_POSITION__
#define __XRAY_AI_HEN_HOLD_POSITION__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenHoldThisPosition : public HenState
	{
		public:
			HenHoldThisPosition(): State(aiHenHoldThisPosition);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

#endif