////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_pursuit.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Pursuit"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_PURSUIT__
#define __XRAY_AI_HEN_PURSUIT__

class CCustomMonster;

#include "ai_hen.h"

namespace AI {
	class HenPursuit : public HenState
	{
		public:
			HenPursuit(): State(aiHenPursuit);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

class HenSelectorPursuit : public SelectorBase
{
	public:
		HenSelectorPursuit();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

#endif