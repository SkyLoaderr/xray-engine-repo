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
}

CAI_PhraseDialogManager::~CAI_PhraseDialogManager	(void)
{
}


//PhraseDialogManager
void CAI_PhraseDialogManager::ReceivePhrase (DIALOG_SHARED_PTR& phrase_dialog)
{
	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(this);
	VERIFY(pInvOwner);
	CGameObject* pOthersGO = dynamic_cast<CGameObject*>(phrase_dialog->OurPartner(this));
	VERIFY(pOthersGO);

	if(!phrase_dialog->IsFinished())
	{
		int goodwill = pInvOwner->CharacterInfo().GetGoodwill(pOthersGO->ID());
		
		//���� �� ������ ����� ����������� ������� �����
		//��������� �� ������ (����� ������)
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
	CPhraseDialogManager::ReceivePhrase(phrase_dialog);
}


void CAI_PhraseDialogManager::SetStartDialog(LPCSTR phrase_dialog)
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

	if(*m_sStartDialog) 
		inherited::AddAvailableDialog(*m_sStartDialog, partner);
	inherited::AddAvailableDialog("hello_dialog", partner);

	inherited::UpdateAvailableDialogs(partner);
}