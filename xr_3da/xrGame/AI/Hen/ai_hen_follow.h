////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_follow.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Follow Me"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_FOLLOW_ME__
#define __XRAY_AI_HEN_FOLLOW_ME__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenFollowMe : public HenState
	{
		public:
			HenFollowMe(): State(aiHenFollowMe);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

class HenSelectorFollow : public HenSelectorBase
{
	public:
		HenSelectorFollow	();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);

};

#endif