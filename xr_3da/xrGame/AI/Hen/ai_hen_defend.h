////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_defend.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Defend"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_DEFEND__
#define __XRAY_AI_HEN_DEFEND__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenDefend : public HenState
	{
		public:
			HenDefend(): State(aiHenDefend);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

#endif