///////////////////////////////////////////////////////////////
// Phrase.cpp
// класс, описывающий фразу (элемент диалога)
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "phrase.h"

#include "ai_space.h"
#include "gameobject.h"
#include "script_game_object.h"

CPhrase::CPhrase	(void)
{
	m_iIndex = -1;
	m_iGoodwillLevel = 0;
}
CPhrase::~CPhrase	(void)
{
}

LPCSTR CPhrase::GetText		()	const			
{
	return m_text.c_str();
}

LPCSTR  CPhrase::GetScriptText	(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2) const
{
	return m_PhraseScript.Text(GetText(), pSpeaker1, pSpeaker2);
}

bool	CPhrase::IsDummy()		const
{
	if(xr_strlen(GetText()) == 0 && !m_PhraseScript.HasText())
		return true;

	return false;
}
