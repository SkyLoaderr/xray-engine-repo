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
#include "relation_registry.h"

CAI_PhraseDialogManager::CAI_PhraseDialogManager	(void)
{
	m_sStartDialog = m_sDefaultStartDialog = NULL;
}

CAI_PhraseDialogManager::~CAI_PhraseDialogManager	(void)
{
}


//PhraseDialogManager
void CAI_PhraseDialogManager::ReceivePhrase (DIALOG_SHARED_PTR& phrase_dialog)
{
	if(/*phrase_dialog->GetDialogType(eDialogTypePDA)|| */
		phrase_dialog->GetDialogType(eDialogTypeAI))
		m_PendingDialogs.push_back(phrase_dialog);
	else
		AnswerPhrase(phrase_dialog);
		
	CPhraseDialogManager::ReceivePhrase(phrase_dialog);
}

void CAI_PhraseDialogManager::AnswerPhrase (DIALOG_SHARED_PTR& phrase_dialog)
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(this);
	THROW(pInvOwner);
	CGameObject* pOthersGO = smart_cast<CGameObject*>(phrase_dialog->OurPartner(this));
	THROW(pOthersGO);
	CInventoryOwner* pOthersIO = smart_cast<CInventoryOwner*>(pOthersGO);
	THROW(pOthersIO);

	if(!phrase_dialog->IsFinished())
	{
		CHARACTER_GOODWILL attitude = RELATION_REGISTRY().GetAttitude(pOthersIO, pInvOwner);

		xr_vector<int> phrases;
		CHARACTER_GOODWILL phrase_goodwill = NO_GOODWILL;
		//если не найдем более подходяещей выводим фразу
		//последнюю из списка (самую грубую)
		int phrase_num = phrase_dialog->PhraseList().size()-1;
		for(u32 i=0; i<phrase_dialog->PhraseList().size(); i++)
		{
			phrase_goodwill = phrase_dialog->PhraseList()[phrase_num]->GoodwillLevel();
			if(attitude >= phrase_goodwill)
			{
				phrase_num = i;
				break;
			}
		}

		for(i=0; i<phrase_dialog->PhraseList().size(); i++)
		{
			if(phrase_goodwill == phrase_dialog->PhraseList()[phrase_num]->GoodwillLevel())
				phrases.push_back(i);
		}
		
		phrase_num = phrases[Random.randI(0, phrases.size())];

		CPhraseDialogManager::SayPhrase(phrase_dialog, 
			phrase_dialog->PhraseList()[phrase_num]->GetIndex());

	}
}

bool CAI_PhraseDialogManager::NeedAnswerOnPending		()
{
	return !m_PendingDialogs.empty();
}

void CAI_PhraseDialogManager::AnswerOnPending			()
{
	for(DIALOG_SHARED_IT it = m_PendingDialogs.begin();
		 m_PendingDialogs.end() != it; it++)
	{
		AnswerPhrase(*it);
	}
	m_PendingDialogs.clear();
}


void CAI_PhraseDialogManager::SetStartDialog(PHRASE_DIALOG_ID phrase_dialog)
{
	m_sStartDialog = phrase_dialog;

#ifdef _DEBUG
	//CInventoryOwner* pOwner = smart_cast<CInventoryOwner*>(this);
	//Msg("[PhraseDialogManager] %s set start dialog %s",pOwner->Name(), *m_sStartDialog);
#endif
}

void CAI_PhraseDialogManager::SetDefaultStartDialog(PHRASE_DIALOG_ID phrase_dialog)
{
	m_sDefaultStartDialog = phrase_dialog;
}

void CAI_PhraseDialogManager::RestoreDefaultStartDialog()
{
	m_sStartDialog = m_sDefaultStartDialog;

#ifdef _DEBUG
	//CInventoryOwner* pOwner = smart_cast<CInventoryOwner*>(this);
	//Msg("[PhraseDialogManager] %s restore default start dialog %s",pOwner->Name(), *m_sStartDialog);
#endif

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