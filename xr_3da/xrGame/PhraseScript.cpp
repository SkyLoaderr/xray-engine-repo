///////////////////////////////////////////////////////////////
// PhraseScript.cpp
// классы для связи диалогов со скриптами
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PhraseScript.h"
#include "script_engine.h"
#include "ai_space.h"
#include "gameobject.h"
#include "script_game_object.h"
#include "infoportion.h"
#include "inventoryowner.h"
#include "script_space.h"

CPhraseScript::CPhraseScript	()
{
}
CPhraseScript::~CPhraseScript	()
{
}

//загрузка из XML файла
void CPhraseScript::Load		(CUIXml& uiXml, XML_NODE* phrase_node)
{
	m_sScriptTextFunc = uiXml.Read(phrase_node, "script_text", 0, NULL);

	LoadSequence(uiXml,phrase_node, "precondition",		m_Preconditions);
	LoadSequence(uiXml,phrase_node, "action",			m_ScriptActions);
	
	LoadSequence(uiXml,phrase_node, "has_info",			m_HasInfo);
	LoadSequence(uiXml,phrase_node, "dont_has_info",	m_DontHasInfo);

	LoadSequence(uiXml,phrase_node, "give_info",		m_GiveInfo);
	LoadSequence(uiXml,phrase_node, "disable_info",		m_DisableInfo);
}

template<class T> 
void  CPhraseScript::LoadSequence (CUIXml& uiXml, XML_NODE* phrase_node, 
								  LPCSTR tag, T&  str_vector)
{
	int tag_num = uiXml.GetNodesNum(phrase_node, tag);
	str_vector.clear();
	for(int i=0; i<tag_num; i++)
	{
		LPCSTR tag_text = uiXml.Read(phrase_node, tag, i, NULL);
		str_vector.push_back(tag_text);
	}
}

bool  CPhraseScript::CheckInfo		(const CInventoryOwner* pOwner) const
{
	VERIFY(pOwner);

	for(u32 i=0; i<m_HasInfo.size(); i++)
		if(!pOwner->HasInfo(CInfoPortion::IdToIndex(m_HasInfo[i])))
			return false;

	for(i=0; i<m_DontHasInfo.size(); i++)
		if(pOwner->HasInfo(CInfoPortion::IdToIndex(m_DontHasInfo[i])))
			return false;

	return true;
}


void  CPhraseScript::TransferInfo	(const CInventoryOwner* pOwner) const
{
	VERIFY(pOwner);

	for(u32 i=0; i<m_GiveInfo.size(); i++)
		pOwner->TransferInfo(CInfoPortion::IdToIndex(m_GiveInfo[i]), true);

	for(i=0; i<m_DisableInfo.size(); i++)
		pOwner->TransferInfo(CInfoPortion::IdToIndex(m_DisableInfo[i]),false);
}



bool CPhraseScript::Precondition	(const CGameObject* pSpeakerGO) const 
{
	bool predicate_result = true;

	if(!CheckInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO)))
		return false;

	for(u32 i = 0; i<Preconditions().size(); i++)
	{
		luabind::functor<bool>	lua_function;
		R_ASSERT(*Preconditions()[i]);
		bool functor_exists = ai().script_engine().functor(*Preconditions()[i] ,lua_function);
		R_ASSERT3(functor_exists, "Cannot find precondition", *Preconditions()[i]);
		predicate_result = lua_function	(pSpeakerGO->lua_game_object());
		if(!predicate_result) break;
	}
	return predicate_result;
}

void CPhraseScript::Action			(const CGameObject* pSpeakerGO) const 
{
	TransferInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO));

	for(u32 i = 0; i<Actions().size(); i++)
	{
		luabind::functor<void>	lua_function;
		R_ASSERT(*Actions()[i]);
		bool functor_exists = ai().script_engine().functor(*Actions()[i] ,lua_function);
		R_ASSERT3(functor_exists, "Cannot find phrase dialog script function", *Actions()[i]);
		lua_function		(pSpeakerGO->lua_game_object());
	}
}

bool CPhraseScript::Precondition	(const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2) const 
{
	bool predicate_result = true;

	if(!CheckInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO1)))
		return false;

	for(u32 i = 0; i<Preconditions().size(); i++)
	{
		luabind::functor<bool>	lua_function;
		R_ASSERT(*Preconditions()[i]);
		bool functor_exists = ai().script_engine().functor(*Preconditions()[i] ,lua_function);
		R_ASSERT3(functor_exists, "Cannot find phrase precondition", *Preconditions()[i]);
		predicate_result = lua_function	(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object());
		if(!predicate_result) break;
	}
	return predicate_result;
}

void CPhraseScript::Action			(const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2) const 
{
	TransferInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO1));

	for(u32 i = 0; i<Actions().size(); i++)
	{
		luabind::functor<void>	lua_function;
		R_ASSERT(*Actions()[i]);
		bool functor_exists = ai().script_engine().functor(*Actions()[i] ,lua_function);
		R_ASSERT3(functor_exists, "Cannot find phrase dialog script function", *Actions()[i]);
		lua_function		(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object());
	}
}

LPCSTR  CPhraseScript::Text		(LPCSTR original_text, const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2) const 
{
	if(NULL == *m_sScriptTextFunc) return NULL;

	luabind::functor<LPCSTR>	lua_function;
	bool functor_exists = ai().script_engine().functor(*m_sScriptTextFunc ,lua_function);
	R_ASSERT3(functor_exists, "Cannot find phrase dialog text script function", *m_sScriptTextFunc);
	return lua_function		(original_text, pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object());
}