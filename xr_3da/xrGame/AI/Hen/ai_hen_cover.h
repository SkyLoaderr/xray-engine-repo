////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_cover.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Cover"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN_COVER__
#define __XRAY_AI_HEN_COVER__

#include "ai_hen.h"

class CCustomMonster;

namespace AI {
	class HenCover : public HenState
	{
		public:
			HenCover(): State(aiHenCover);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}

#endif