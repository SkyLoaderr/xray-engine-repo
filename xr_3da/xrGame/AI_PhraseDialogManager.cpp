///////////////////////////////////////////////////////////////
// AI_PhraseDialogManager.cpp
// �����, �� �������� ����������� NPC ���������, ������� ������
// � �������
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
	m_sStartDialog = m_sDefaultStartDialog = NULL;
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
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(this);
	VERIFY(pInvOwner);
	CGameObject* pOthersGO = smart_cast<CGameObject*>(phrase_dialog->OurPartner(this));
	VERIFY(pOthersGO);
	CInventoryOwner* pOthersIO = smart_cast<CInventoryOwner*>(pOthersGO);
	VERIFY(pOthersIO);

	if(!phrase_dialog->IsFinished())
	{
		CHARACTER_GOODWILL attitude = pInvOwner->CharacterInfo().Relations().GetAttitude(pOthersGO->ID(), pOthersIO->CharacterInfo().Community().index());

		//���� �� ������ ����� ����������� ������� �����
		//��������� �� ������ (����� ������)
		int phrase_num = phrase_dialog->PhraseList().size()-1;
		for(u32 i=0; i<phrase_dialog->PhraseList().size(); i++)
		{
			if(attitude >= phrase_dialog->PhraseList()[phrase_num]->GoodwillLevel())
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

void CAI_PhraseDialogManager::SetDefaultStartDialog(PHRASE_DIALOG_ID phrase_dialog)
{
	m_sDefaultStartDialog = phrase_dialog;
}

void CAI_PhraseDialogManager::RestoreDefaultStartDialog()
{
	m_sStartDialog = m_sDefaultStartDialog;
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