////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_u_fire.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Under Fire"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_UNDER_FIRE__
#define __XRAY_AI_HEN_UNDER_FIRE__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenUnderFire : public HenState
	{
		public:
			HenUnderFire(): State(aiHenUnderFire);
			virtual BOOL Parse(CCustomMonster* Me);
			virtual void Hit(Fvector &dir);
			virtual void Sense(Fvector &dir);
	};
}

class HenSelectorUnderFire : public SelectorBase
{
	public:
		HenSelectorUnderFire() 
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);

};

#endif