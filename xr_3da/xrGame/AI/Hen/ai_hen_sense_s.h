////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_sense_s.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Sense Something"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_SENSE_SOMETHING__
#define __XRAY_AI_HEN_SENSE_SOMETHING__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenSenseSomething : public HenState
	{
		public:
			HenSenseSomething(): State(aiHenSenseSomething);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

#endif