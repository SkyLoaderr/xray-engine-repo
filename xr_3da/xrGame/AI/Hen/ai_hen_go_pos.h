////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_go_pos.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Go In This Position"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_GO_IN_THIS_POSITION__
#define __XRAY_AI_HEN_GO_IN_THIS_POSITION__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenGoInThisPosition : public HenState
	{
		public:
			HenGoInThisPosition(): State(aiHenGoInThisPosition);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

#endif