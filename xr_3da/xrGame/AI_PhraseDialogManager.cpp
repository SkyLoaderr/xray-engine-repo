///////////////////////////////////////////////////////////////
// AI_PhraseDialogManager.cpp
// Класс, от которого наследуются NPC персонажи, ведущие диалог
// с актером
//
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AI_PhraseDialogManager.h"
#include "PhraseDialog.h"
#include "inventoryowner.h"
#include "character_info.h"
#include "gameobject.h"

CAI_PhraseDialogManager::CAI_PhraseDialogManager	(void)
{
}

CAI_PhraseDialogManager::~CAI_PhraseDialogManager	(void)
{
}


//PhraseDialogManager
void CAI_PhraseDialogManager::ReceivePhrase (DIALOG_SHARED_PTR& phrase_dialog)
{
	if(phrase_dialog->GetDialogType() != eDialogTypePDA)
		AnswerPhrase(phrase_dialog);
	else
		m_PendingPdaDialogs.push_back(phrase_dialog);

	CPhraseDialogManager::ReceivePhrase(phrase_dialog);
}

void CAI_PhraseDialogManager::AnswerPhrase (DIALOG_SHARED_PTR& phrase_dialog)
{
	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(this);
	VERIFY(pInvOwner);
	CGameObject* pOthersGO = dynamic_cast<CGameObject*>(phrase_dialog->OurPartner(this));
	VERIFY(pOthersGO);

	if(!phrase_dialog->IsFinished())
	{
		int goodwill = pInvOwner->CharacterInfo().GetGoodwill(pOthersGO->ID());

		//если не найдем более подходяещей выводим фразу
		//последнюю из списка (самую грубую)
		int phrase_num = phrase_dialog->PhraseList().size()-1;
		for(u32 i=0; i<phrase_dialog->PhraseList().size(); i++)
		{
			if(goodwill >= phrase_dialog->PhraseList()[phrase_num]->GoodwillLevel())
			{
				phrase_num = i;
				break;
			}
		}

		CPhraseDialogManager::SayPhrase(phrase_dialog, 
			phrase_dialog->PhraseList()[phrase_num]->GetIndex());

	}
}

bool CAI_PhraseDialogManager::NeedAnswerOnPDA		()
{
	return !m_PendingPdaDialogs.empty();
}

void CAI_PhraseDialogManager::AnswerOnPDA			()
{
	for(DIALOG_SHARED_IT it = m_PendingPdaDialogs.begin();
		 m_PendingPdaDialogs.end() != it; it++)
	{
		AnswerPhrase(*it);
	}
	m_PendingPdaDialogs.clear();
}


void CAI_PhraseDialogManager::SetStartDialog(PHRASE_DIALOG_ID phrase_dialog)
{
	m_sStartDialog = phrase_dialog;
}
void CAI_PhraseDialogManager::ResetStartDialog()
{
	m_sStartDialog = NULL;
}


void CAI_PhraseDialogManager::UpdateAvailableDialogs	(CPhraseDialogManager* partner)
{		
	m_AvailableDialogs.clear();
	m_CheckedDialogs.clear();

	if(*m_sStartDialog) 
		inherited::AddAvailableDialog(*m_sStartDialog, partner);
	inherited::AddAvailableDialog("hello_dialog", partner);

	inherited::UpdateAvailableDialogs(partner);
}