///////////////////////////////////////////////////////////////
// PhraseDialogManager.cpp
// Класс, от которого наследуются персонажи, ведущие диалог
// между собой
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PhraseDialogManager.h"
#include "PhraseDialog.h"

#include "ai_space.h"
#include "script_engine.h"
#include "gameobject.h"
#include "ai_script_classes.h"

CPhraseDialogManager::CPhraseDialogManager	(void)
{
}
CPhraseDialogManager::~CPhraseDialogManager	(void)
{
}

void CPhraseDialogManager::InitDialog (CPhraseDialogManager* dialog_partner, DIALOG_SHARED_PTR& phrase_dialog)
{
	phrase_dialog->Init(this, dialog_partner);
	AddDialog(phrase_dialog);
	dialog_partner->AddDialog(phrase_dialog);
}

void CPhraseDialogManager::AddDialog(DIALOG_SHARED_PTR& phrase_dialog)
{
	DIALOG_VECTOR_IT it = std::find(m_ActiveDialogs.begin(), m_ActiveDialogs.end(), phrase_dialog);
	VERIFY(m_ActiveDialogs.end() == it);
	m_ActiveDialogs.push_back(phrase_dialog);
}

void CPhraseDialogManager::ReceivePhrase(DIALOG_SHARED_PTR& phrase_dialog)
{
}
void CPhraseDialogManager::SayPhrase(DIALOG_SHARED_PTR& phrase_dialog, 
									 PHRASE_ID phrase_id)
{
	DIALOG_VECTOR_IT it = std::find(m_ActiveDialogs.begin(), m_ActiveDialogs.end(), phrase_dialog);
	VERIFY(m_ActiveDialogs.end() != it);

	VERIFY(phrase_dialog->IsWeSpeaking(this));
	bool coninue_talking = CPhraseDialog::SayPhrase(phrase_dialog, phrase_id);

	if(!coninue_talking)
		m_ActiveDialogs.erase(it);
}


bool CPhraseDialogManager::AddAvailableDialog(LPCSTR dialog_id, CPhraseDialogManager* partner)
{
	DIALOG_SHARED_PTR phrase_dialog(xr_new<CPhraseDialog>());
	phrase_dialog->Load(dialog_id);

	//вызвать скриптовую присоединенную функцию 
	//активируется после сказанной фразы
	bool predicate_result = true;
	for(u32 i = 0; i<phrase_dialog->Preconditions().size(); i++)
	{
		luabind::functor<bool>	lua_function;
		bool functor_exists = ai().script_engine().functor(*phrase_dialog->Preconditions()[i] ,lua_function);
		R_ASSERT3(functor_exists, "Cannot find phrase dialog precondition", *phrase_dialog->Preconditions()[i]);
		const CGameObject*	pSpeakerGO1 = dynamic_cast<const CGameObject*>(this);		VERIFY(pSpeakerGO1);
		const CGameObject*	pSpeakerGO2 = dynamic_cast<const CGameObject*>(partner);	VERIFY(pSpeakerGO2);
		predicate_result = lua_function	(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object());
		if(!predicate_result) break;
	}
	if(predicate_result) m_AvailableDialogs.push_back(phrase_dialog);
	return predicate_result;
}