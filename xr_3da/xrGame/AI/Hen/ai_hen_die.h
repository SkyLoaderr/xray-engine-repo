////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_die.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Die"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_DIE__
#define __XRAY_AI_HEN_DIE__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenDie : public HenState
	{
		public:
			HenDie(): State(aiHenDie);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

#endif