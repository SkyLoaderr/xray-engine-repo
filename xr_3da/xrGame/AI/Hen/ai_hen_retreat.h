////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_retreat.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Retreat"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_RETREAT__
#define __XRAY_AI_HEN_RETREAT__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenPursuit : public HenState
	{
		public:
			HenPursuit(): State(aiHenPursuit);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

#endif