////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_go_dir.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Go In This Direction"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_GO_IN_THIS_DIRECTION__
#define __XRAY_AI_HEN_GO_IN_THIS_DIRECTION__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenGoInThisDirection : public HenState
	{
		public:
			HenGoInThisDirection(): State(aiHenGoInThisDirection);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

#endif