////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_wait_pos.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Wait On Position"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_WAIT_ON_POSITION__
#define __XRAY_AI_HEN_WAIT_ON_POSITION__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenWaitOnPosition : public HenState
	{
		public:
			HenWaitOnPosition(): State(aiHenWaitOnPosition);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

#endif