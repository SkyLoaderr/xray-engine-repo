////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script sound action class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_sound_action.h"
#include "script_space.h"

using namespace luabind;

void CScriptSoundAction::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptSoundAction>("sound")
			.enum_("type")
			[
				value("idle",					int(MonsterSpace::eMonsterSoundIdle)),
				value("eat",					int(MonsterSpace::eMonsterSoundEat)),
				value("attack",					int(MonsterSpace::eMonsterSoundAttack)),
				value("attack_hit",				int(MonsterSpace::eMonsterSoundAttackHit)),
				value("take_damage",			int(MonsterSpace::eMonsterSoundTakeDamage)),
				value("die",					int(MonsterSpace::eMonsterSoundDie)),
				value("threaten",				int(MonsterSpace::eMonsterSoundThreaten)),
				value("steal",					int(MonsterSpace::eMonsterSoundSteal)),
				value("panic",					int(MonsterSpace::eMonsterSoundPanic)),
				value("growling",				int(MonsterSpace::eMonsterSoundGrowling))
			]

			.def(								constructor<>())
			.def(								constructor<LPCSTR,LPCSTR>())
			.def(								constructor<LPCSTR,LPCSTR,const Fvector &>())
			.def(								constructor<LPCSTR,LPCSTR,const Fvector &,const Fvector &>())
			.def(								constructor<LPCSTR,LPCSTR,const Fvector &,const Fvector &,bool>())
			.def(								constructor<LPCSTR,const Fvector &>())
			.def(								constructor<LPCSTR,const Fvector &,const Fvector &>())
			.def(								constructor<LPCSTR,const Fvector &,const Fvector &,bool>())
			.def(								constructor<CScriptSound&,LPCSTR,const Fvector &>())
			.def(								constructor<CScriptSound&,LPCSTR,const Fvector &,const Fvector &>())
			.def(								constructor<CScriptSound&,LPCSTR,const Fvector &,const Fvector &,bool>())
			.def(								constructor<CScriptSound&,const Fvector &>())
			.def(								constructor<CScriptSound&,const Fvector &,const Fvector &>())
			.def(								constructor<CScriptSound&,const Fvector &,const Fvector &,bool>())
			// monster specific
			.def(								constructor<MonsterSpace::EMonsterSounds>())
			.def(								constructor<MonsterSpace::EMonsterSounds,int>())
			// trader specific
			.def(								constructor<LPCSTR,LPCSTR,MonsterSpace::EMonsterHeadAnimType>())

			.def("set_sound",					(void (CScriptSoundAction::*)(LPCSTR))(CScriptSoundAction::SetSound))
			.def("set_sound",					(void (CScriptSoundAction::*)(const CScriptSound &))(CScriptSoundAction::SetSound))
			.def("set_sound_type",				&CScriptSoundAction::SetSoundType)
			.def("set_bone",					&CScriptSoundAction::SetBone)
			.def("set_position",				&CScriptSoundAction::SetPosition)
			.def("set_angles",					&CScriptSoundAction::SetAngles)
			.def("completed",					(bool (CScriptSoundAction::*)())(CScriptSoundAction::completed))
	];
}
