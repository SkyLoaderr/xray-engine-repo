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