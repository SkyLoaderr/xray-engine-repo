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
#include "script_game_object.h"

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


void CPhraseDialogManager::UpdateAvailableDialogs(CPhraseDialogManager* partner)
{
}

bool CPhraseDialogManager::AddAvailableDialog(PHRASE_DIALOG_ID dialog_id, CPhraseDialogManager* partner)
{
	PHRASE_DIALOG_INDEX dialog_index =  CPhraseDialog::IdToIndex(dialog_id);
	if(std::find(m_CheckedDialogs.begin(), m_CheckedDialogs.end(), dialog_index) != m_CheckedDialogs.end())
		return false;
	m_CheckedDialogs.push_back(dialog_index);

	DIALOG_SHARED_PTR phrase_dialog(xr_new<CPhraseDialog>());
	phrase_dialog->Load(dialog_id);

	//вызвать скриптовую присоединенную функцию 
	//активируется после сказанной фразы
	const CGameObject*	pSpeakerGO1 = dynamic_cast<const CGameObject*>(this);		VERIFY(pSpeakerGO1);
	const CGameObject*	pSpeakerGO2 = dynamic_cast<const CGameObject*>(partner);	VERIFY(pSpeakerGO2);

	bool predicate_result = phrase_dialog->Precondition(pSpeakerGO1, pSpeakerGO2);
	if(predicate_result) m_AvailableDialogs.push_back(phrase_dialog);
	return predicate_result;
}