////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_attack.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Attack"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_ATTACK__
#define __XRAY_AI_HEN_ATTACK__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenAttack : public HenState
	{
		public:
			HenAttack(): State(aiHenAttack);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

class HenSelectorAttack : public SelectorBase
{
	public:
		HenSelectorAttack();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

#endif