////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script objects binder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_engine.h"
#include "script_binder.h"
#include "xrServer_Objects_ALife.h"
#include "script_binder_object.h"
#include "ai_script_classes.h"

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
	VERIFY					(!m_object);
	if (!pSettings->line_exist(section,"script_binding"))
		return;
	
	luabind::functor<void>	lua_function;
	if (!ai().script_engine().functor(pSettings->r_string(section,"script_binding"),lua_function)) {
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"function %s is not loaded!",pSettings->r_string(section,"script_binding"));
		return;
	}
	
	CGameObject				*game_object = dynamic_cast<CGameObject*>(this);
	lua_function			(game_object ? game_object->lua_game_object() : 0);
	
	if (m_object)
		m_object->reload	(section);
}

BOOL CScriptBinder::net_Spawn		(LPVOID DC)
{
	CSE_Abstract			*abstract = (CSE_Abstract*)DC;
	if (m_object && !m_object->net_Spawn(abstract))
		return				(FALSE);

	return					(TRUE);
}

void CScriptBinder::net_Destroy		()
{
	if (m_object)
		m_object->net_Destroy	();
	xr_delete				(m_object);
}

void CScriptBinder::net_Import		(NET_Packet &net_packet)
{
	if (m_object)
		m_object->net_Import(&net_packet);
}

void CScriptBinder::net_Export		(NET_Packet &net_packet)
{
	if (m_object)
		m_object->net_Export(&net_packet);
}

void CScriptBinder::set_object		(CScriptBinderObject *object)
{
	VERIFY2					(!m_object,"Cannot bind to the object twice!");
	Msg						("Core object %s is binded with the script object",dynamic_cast<CGameObject*>(this) ? *dynamic_cast<CGameObject*>(this)->cName() : "");
	m_object				= object;
}
