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
#include "ai_debug.h"

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
	THROW(pOwner);

	for(u32 i=0; i<m_HasInfo.size(); i++) {
		INFO_INDEX	result = CInfoPortion::IdToIndex(m_HasInfo[i],NO_INFO_INDEX,true);
		if (result == NO_INFO_INDEX) {
			ai().script_engine().script_log(eLuaMessageTypeError,"XML item not found : \"%s\"",*m_HasInfo[i]);
			break;
		}

		if (!pOwner->HasInfo(result)) {
#ifdef DEBUG
			Msg("[%s] has info %s", pOwner->Name(), *m_HasInfo[i]);
#endif
			return false;
		}
	}

	for(i=0; i<m_DontHasInfo.size(); i++) {
		INFO_INDEX	result = CInfoPortion::IdToIndex(m_DontHasInfo[i],NO_INFO_INDEX,true);
		if (result == NO_INFO_INDEX) {
			ai().script_engine().script_log(eLuaMessageTypeError,"XML item not found : \"%s\"",*m_DontHasInfo[i]);
			break;
		}

		if (pOwner->HasInfo(result)) {
#ifdef DEBUG
			Msg("[%s] dont has info %s", pOwner->Name(), *m_DontHasInfo[i]);
#endif
			return false;
		}
	}
	return true;
}


void  CPhraseScript::TransferInfo	(const CInventoryOwner* pOwner) const
{
	THROW(pOwner);

	for(u32 i=0; i<m_GiveInfo.size(); i++)
		pOwner->TransferInfo(CInfoPortion::IdToIndex(m_GiveInfo[i]), true);

	for(i=0; i<m_DisableInfo.size(); i++)
		pOwner->TransferInfo(CInfoPortion::IdToIndex(m_DisableInfo[i]),false);
}



bool CPhraseScript::Precondition	(const CGameObject* pSpeakerGO, LPCSTR dialog_id, int phrase_num) const 
{
	bool predicate_result = true;

	if(!CheckInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO))){
		if (psAI_Flags.test(aiDialogs))
			Msg("dialog [%s] phrase[%d] rejected by CheckInfo",dialog_id,phrase_num);
		return false;
	}

	for(u32 i = 0; i<Preconditions().size(); i++)
	{
		luabind::functor<bool>	lua_function;
		THROW(*Preconditions()[i]);
		bool functor_exists = ai().script_engine().functor(*Preconditions()[i] ,lua_function);
		THROW3(functor_exists, "Cannot find precondition", *Preconditions()[i]);
		predicate_result = lua_function	(pSpeakerGO->lua_game_object());
		if(!predicate_result){
			if (psAI_Flags.test(aiDialogs))
				Msg("dialog [%s] phrase[%d] rejected by script predicate",dialog_id,phrase_num);
		} break;
	}
	return predicate_result;
}

void CPhraseScript::Action			(const CGameObject* pSpeakerGO, LPCSTR dialog_id, int phrase_num) const 
{
	TransferInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO));

	for(u32 i = 0; i<Actions().size(); i++)
	{
		luabind::functor<void>	lua_function;
		THROW(*Actions()[i]);
		bool functor_exists = ai().script_engine().functor(*Actions()[i] ,lua_function);
		THROW3(functor_exists, "Cannot find phrase dialog script function", *Actions()[i]);
		lua_function		(pSpeakerGO->lua_game_object(), dialog_id);
	}
}

bool CPhraseScript::Precondition	(const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2, LPCSTR dialog_id, int phrase_num) const 
{
	bool predicate_result = true;

	if(!CheckInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO1))){
		if (psAI_Flags.test(aiDialogs))
			Msg("dialog [%s] phrase[%d] rejected by CheckInfo",dialog_id,phrase_num);
		return false;
	}
	for(u32 i = 0; i<Preconditions().size(); i++)
	{
		luabind::functor<bool>	lua_function;
		THROW(*Preconditions()[i]);
		bool functor_exists = ai().script_engine().functor(*Preconditions()[i] ,lua_function);
		THROW3(functor_exists, "Cannot find phrase precondition", *Preconditions()[i]);
		predicate_result = lua_function	(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object(), dialog_id, phrase_num);
		if(!predicate_result){
			if (psAI_Flags.test(aiDialogs))
				Msg("dialog [%s] phrase[%d] rejected by script predicate",dialog_id,phrase_num);
			break;
		}
	}
	return predicate_result;
}

void CPhraseScript::Action			(const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2, LPCSTR dialog_id, int phrase_num) const 
{
	TransferInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO1));

	for(u32 i = 0; i<Actions().size(); i++)
	{
		luabind::functor<void>	lua_function;
		THROW(*Actions()[i]);
		bool functor_exists = ai().script_engine().functor(*Actions()[i] ,lua_function);
		THROW3(functor_exists, "Cannot find phrase dialog script function", *Actions()[i]);
		lua_function		(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object(), dialog_id, phrase_num);
	}
}

LPCSTR  CPhraseScript::Text		(LPCSTR original_text, const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2, LPCSTR dialog_id, int phrase_num) const 
{
	if(NULL == *m_sScriptTextFunc) return NULL;

	luabind::functor<LPCSTR>	lua_function;
	bool functor_exists = ai().script_engine().functor(*m_sScriptTextFunc ,lua_function);
	THROW3(functor_exists, "Cannot find phrase dialog text script function", *m_sScriptTextFunc);
	return lua_function		(original_text, pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object(), dialog_id, phrase_num);
}