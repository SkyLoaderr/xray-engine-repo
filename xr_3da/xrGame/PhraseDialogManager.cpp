///////////////////////////////////////////////////////////////
// PhraseDialogManager.cpp
// Класс, от которого наследуются персонажи, ведущие диалог
// между собой
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PhraseDialogManager.h"
#include "PhraseDialog.h"


CPhraseDialogManager::CPhraseDialogManager	(void)
{
}
CPhraseDialogManager::~CPhraseDialogManager	(void)
{
}

DIALOG_SHARED_PRT CPhraseDialogManager::InitDialog (CPhraseDialogManager* dialog_partner, 
													  ref_str dialog_id, 
													  EDialogType dialog_type)
{
	DIALOG_SHARED_PRT phrase_dialog(xr_new<CPhraseDialog>());
	phrase_dialog->Load(dialog_id);
	phrase_dialog->SetDialogType(dialog_type);
	phrase_dialog->Init(this, dialog_partner);
	m_Dialogs.push_back(phrase_dialog);

	DIALOG_SHARED_PRT phrase_dialog111(xr_new<CPhraseDialog>());
	phrase_dialog111->Load(dialog_id);

	return phrase_dialog;
}

void CPhraseDialogManager::ReceivePhrase(DIALOG_SHARED_PRT phrase_dialog)
{
}
void CPhraseDialogManager::SayPhrase(DIALOG_SHARED_PRT phrase_dialog, 
									 PHRASE_ID phrase_id)
{
	DIALOG_VECTOR_IT it = std::find(m_Dialogs.begin(), m_Dialogs.end(), phrase_dialog);
	VERIFY(m_Dialogs.end() != it);

	VERIFY(phrase_dialog->IsWeSpeaking(this));
	bool coninue_talking = phrase_dialog->SayPhrase(phrase_id);

	if(!coninue_talking)
		m_Dialogs.erase(it);
}