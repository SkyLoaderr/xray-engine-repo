///////////////////////////////////////////////////////////////
// Phrase.cpp
// класс, описывающий фразу (элемент диалога)
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "phrase.h"

#include "ai_space.h"
#include "gameobject.h"
#include "ai_script_classes.h"

CPhrase::CPhrase	(void)
{
	m_iIndex = -1;
	m_iGoodwillLevel = 0;
}
CPhrase::~CPhrase	(void)
{
}

bool CPhrase::IsPhraseAvailable (const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2) const
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