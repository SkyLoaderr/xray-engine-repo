///////////////////////////////////////////////////////////////
// PhraseScript.cpp
// классы для связи диалогов со скриптами
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PhraseScript.h"

#include "script_engine.h"
#include "ai_space.h"
#include "gameobject.h"
#include "ai_script_classes.h"


CPhraseScript::CPhraseScript	()
{
}
CPhraseScript::~CPhraseScript	()
{
}

//загрузка из XML файла
void CPhraseScript::Load		(CUIXml& uiXml, XML_NODE* phrase_node)
{
	int preconditions_num = uiXml.GetNodesNum(phrase_node, "precondition");
	m_Preconditions.clear();
	for(int i=0; i<preconditions_num; i++)
	{
		ref_str precondition_name = uiXml.Read(phrase_node, "precondition", i, NULL);
		m_Preconditions.push_back(precondition_name);
	}

	int script_actions_num = uiXml.GetNodesNum(phrase_node, "action");
	m_ScriptActions.clear();
	for(int i=0; i<script_actions_num; i++)
	{
		ref_str action_name = uiXml.Read(phrase_node, "action", i, NULL);
		m_ScriptActions.push_back(action_name);
	}
}


bool CPhraseScript::Precondition	(const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2) const 
{
	bool predicate_result = true;
	for(u32 i = 0; i<Preconditions().size(); i++)
	{
		luabind::functor<bool>	lua_function;
		bool functor_exists = ai().script_engine().functor(*Preconditions()[i] ,lua_function);
		R_ASSERT3(functor_exists, "Cannot find phrase precondition", *Preconditions()[i]);
		predicate_result = lua_function	(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object());
		if(!predicate_result) break;
	}
	return predicate_result;
}

void CPhraseScript::Action			(const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2) const 
{
	for(u32 i = 0; i<Actions().size(); i++)
	{
		luabind::functor<void>	lua_function;
		bool functor_exists = ai().script_engine().functor(*Actions()[i] ,lua_function);
		R_ASSERT3(functor_exists, "Cannot find phrase dialog script function", *Actions()[i]);
		lua_function		(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object());
	}
}