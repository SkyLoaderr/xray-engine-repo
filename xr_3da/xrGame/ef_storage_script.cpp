////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_storage_script.cpp
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Evaluation functions storage class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ef_storage.h"
#include "ai_space.h"
#include "script_space.h"
#include "script_game_object.h"
#include "entity_alive.h"
#include "script_engine.h"
#include "ef_base.h"
#include "xrServer_Objects_ALife.h"

using namespace luabind;

CEF_Storage *ef_storage()
{
	return	(&ai().ef_storage());
}

float evaluate(CEF_Storage *ef_storage, LPCSTR function, CScriptGameObject *_0, CScriptGameObject *_1, CScriptGameObject *_2)
{
	CBaseFunction	*f = ef_storage->function(function);
	if (!f) {
		ai().script_engine().script_log(eLuaMessageTypeError,"Cannot find evaluation function %s",function);
		return		(0.f);
	}

	ef_storage->m_tpCurrentMember	= smart_cast<CEntityAlive*>(_0 ? _0->object() : 0);
	if (_0 && !ef_storage->m_tpCurrentMember) {
		ai().script_engine().script_log(eLuaMessageTypeError,"object %s is not herited from CSE_ALifeSchedulable!",*_0->cName());
		return		(0.f);
	}
	
	ef_storage->m_tpCurrentEnemy	= smart_cast<CEntityAlive*>(_1 ? _1->object() : 0);
	if (_1 && !ef_storage->m_tpCurrentEnemy) {
		ai().script_engine().script_log(eLuaMessageTypeError,"object %s is not herited from CSE_ALifeSchedulable!",*_1->cName());
		return		(0.f);
	}

	ef_storage->m_tpGameObject		= _2->object();

	return			(f->ffGetValue());
}

float evaluate(CEF_Storage *ef_storage, LPCSTR function, CScriptGameObject *_0, CScriptGameObject *_1)
{
	return			(evaluate(ef_storage,function,_0,_1,0));
}

float evaluate(CEF_Storage *ef_storage, LPCSTR function, CScriptGameObject *_0)
{
	return			(evaluate(ef_storage,function,_0,0,0));
}

float evaluate(CEF_Storage *ef_storage, LPCSTR function, CSE_ALifeObject *_0, CSE_ALifeObject *_1, CSE_ALifeObject *_2)
{
	CBaseFunction	*f = ef_storage->function(function);
	if (!f) {
		ai().script_engine().script_log(eLuaMessageTypeError,"Cannot find evaluation function %s",function);
		return		(0.f);
	}

	ef_storage->m_tpCurrentALifeMember	= smart_cast<CSE_ALifeSchedulable*>(_0);
	if (_0 && !ef_storage->m_tpCurrentALifeMember) {
		ai().script_engine().script_log(eLuaMessageTypeError,"object %s is not herited from CSE_ALifeSchedulable!",_1->name_replace());
		return		(0.f);
	}

	ef_storage->m_tpCurrentALifeEnemy	= smart_cast<CSE_ALifeSchedulable*>(_1);
	if (_1 && !ef_storage->m_tpCurrentALifeEnemy) {
		ai().script_engine().script_log(eLuaMessageTypeError,"object %s is not herited from CSE_ALifeSchedulable!",_1->name_replace());
		return		(0.f);
	}

	ef_storage->m_tpCurrentALifeObject	= _0;

	return			(f->ffGetValue());
}

float evaluate(CEF_Storage *ef_storage, LPCSTR function, CSE_ALifeObject *_0, CSE_ALifeObject *_1)
{
	return			(evaluate(ef_storage,function,_0,_1,0));
}

float evaluate(CEF_Storage *ef_storage, LPCSTR function, CSE_ALifeObject *_0)
{
	return			(evaluate(ef_storage,function,_0,0,0));
}

void CEF_Storage::script_register(lua_State *L)
{
	module(L)
	[
		def("ef_storage",&ef_storage),

		class_<CEF_Storage>("cef_storage")
			.def("evaluate",	(float (*)(CEF_Storage*,LPCSTR,CScriptGameObject*))(&evaluate))
			.def("evaluate",	(float (*)(CEF_Storage*,LPCSTR,CScriptGameObject*,CScriptGameObject*))(&evaluate))
			.def("evaluate",	(float (*)(CEF_Storage*,LPCSTR,CScriptGameObject*,CScriptGameObject*,CScriptGameObject*))(&evaluate))
			.def("evaluate",	(float (*)(CEF_Storage*,LPCSTR,CSE_ALifeObject*))(&evaluate))
			.def("evaluate",	(float (*)(CEF_Storage*,LPCSTR,CSE_ALifeObject*,CSE_ALifeObject*))(&evaluate))
			.def("evaluate",	(float (*)(CEF_Storage*,LPCSTR,CSE_ALifeObject*,CSE_ALifeObject*,CSE_ALifeObject*))(&evaluate))
	];
}