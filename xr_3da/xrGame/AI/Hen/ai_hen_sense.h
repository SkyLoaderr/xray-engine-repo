////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen_attack.h
//	Created 	: 12.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Hen mode "Attack"
////////////////////////////////////////////////////////////////////////////

class CCustomMonster;

namespace AI {
	class HenAttack : public HenState
	{
		public:
			HenAttack(): State(aiHenAttack);
			virtual BOOL Parse(CCustomMonster* Me);
	};
}