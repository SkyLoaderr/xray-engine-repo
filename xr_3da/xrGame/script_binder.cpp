////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script objects binder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "ai_space.h"
#include "script_engine.h"
#include "script_binder.h"
#include "xrServer_Objects_ALife.h"
#include "script_binder_object.h"
#include "script_game_object.h"
#include "gameobject.h"

CScriptBinder::CScriptBinder		()
{
	init					();
}

CScriptBinder::~CScriptBinder		()
{
	VERIFY					(!m_object);
}

void CScriptBinder::init			()
{
	m_object				= 0;
}

void CScriptBinder::reinit			()
{
	if (m_object)
		m_object->reinit	();
}

void CScriptBinder::Load			(LPCSTR section)
{
}

void CScriptBinder::reload			(LPCSTR section)
{
#ifdef DBG_DISABLE_SCRIPTS
	return;
#endif
	VERIFY					(!m_object);
	if (!pSettings->line_exist(section,"script_binding"))
		return;
	
	luabind::functor<void>	lua_function;
	if (!ai().script_engine().functor(pSettings->r_string(section,"script_binding"),lua_function)) {
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"function %s is not loaded!",pSettings->r_string(section,"script_binding"));
		return;
	}
	
	CGameObject				*game_object = smart_cast<CGameObject*>(this);
	lua_function			(game_object ? game_object->lua_game_object() : 0);
	
	if (m_object)
		m_object->reload	(section);
}

BOOL CScriptBinder::net_Spawn		(LPVOID DC)
{
	CSE_Abstract			*abstract = (CSE_Abstract*)DC;
	CSE_ALifeObject			*object = smart_cast<CSE_ALifeObject*>(abstract);
	if (object && m_object && !m_object->net_Spawn(object))
		return				(FALSE);

	return					(TRUE);
}

void CScriptBinder::net_Destroy		()
{
	if (m_object)
		m_object->net_Destroy	();
	xr_delete				(m_object);
}

void CScriptBinder::set_object		(CScriptBinderObject *object)
{
	VERIFY2					(!m_object,"Cannot bind to the object twice!");
	Msg						("Core object %s is binded with the script object",smart_cast<CGameObject*>(this) ? *smart_cast<CGameObject*>(this)->cName() : "");
	m_object				= object;
}

void CScriptBinder::shedule_Update	(u32 time_delta)
{
	if (m_object)
		m_object->shedule_Update	(time_delta);
}

void CScriptBinder::save			(NET_Packet &output_packet)
{
	if (m_object)
		m_object->save				(&output_packet);
}

void CScriptBinder::load			(IReader &input_packet)
{
	if (m_object)
		m_object->load				(&input_packet);
}
