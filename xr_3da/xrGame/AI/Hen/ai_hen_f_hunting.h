////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_f_hunting.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Free Hunting"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_FREE_HUNTING__
#define __XRAY_AI_HEN_FREE_HUNTING__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenFreeHunting : public HenState
	{
		public:
			HenFreeHunting(): State(aiHenFreeHunting);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

class HenSelectorFreeHunting : public SelectorBase
{
	public:
		HenSelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

#endif