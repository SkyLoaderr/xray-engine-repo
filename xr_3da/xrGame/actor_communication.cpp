// actor_communication.cpp:	 ����� �� PDA � �������
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "actor.h"

#include "UIGameSP.h"
#include "UI.h"
#include "PDA.h"
#include "HUDManager.h"
#include "level.h"
#include "PhraseDialog.h"


//information receive
void CActor::OnReceiveInfo(INFO_ID info_index)
{
	//������ ���� ��������� � ������ single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	CInfoPortion info_portion;
	info_portion.Load(info_index);

	//�������� ������� �� �����
	for(u32 i=0; i<info_portion.MapLocations().size(); i++)
		Level().AddMapLocation(info_portion.MapLocations()[i]);


	if(pGameSP->TalkMenu.IsShown())
	{
		if(pGameSP->TalkMenu.IsShown())
		{
			pGameSP->TalkMenu.UpdateQuestions();
		}
		//���� ������ ��������� �� PDA ����� ��� PDA �� ��� �������
		else if(!pGameSP->m_pUserMenu->IsShown())
		{
		}
	}

	CInventoryOwner::OnReceiveInfo(info_index);
}


void CActor::OnDisableInfo(INFO_ID info_index)
{
	//������ ���� ��������� � ������ single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(pGameSP && pGameSP->TalkMenu.IsShown()) pGameSP->TalkMenu.UpdateQuestions();

	Level().RemoveMapLocationByInfo(info_index);
	CInventoryOwner::OnDisableInfo(info_index);
}



void CActor::ReceivePdaMessage(u16 who, EPdaMsg msg, INFO_ID info_index)
{
	//������ ���� ��������� � ������ single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	//������������ � ����������
	CObject* pPdaObject =  Level().Objects.net_Find(who);
	VERIFY(pPdaObject);
	CPda* pPda = dynamic_cast<CPda*>(pPdaObject);
	VERIFY(pPda);
	HUD().GetUI()->UIMainIngameWnd.ReceivePdaMessage(pPda->GetOriginalOwner(), msg, info_index);

	SPdaMessage last_pda_message;
	//bool prev_msg = 
	GetPDA()->GetLastMessageFromLog(who, last_pda_message);


	CInventoryOwner::ReceivePdaMessage(who, msg, info_index);
}

void  CActor::ReceivePhrase		(DIALOG_SHARED_PTR& phrase_dialog)
{
	//������ ���� ��������� � ������ single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	if(pGameSP->TalkMenu.IsShown())
	{
		pGameSP->TalkMenu.UpdateQuestions();
	}

	CPhraseDialogManager::ReceivePhrase(phrase_dialog);
}

void   CActor::UpdateAvailableDialogs	(CPhraseDialogManager* partner)
{
	m_AvailableDialogs.clear();

	for(KNOWN_INFO_VECTOR::const_iterator it = CInventoryOwner::KnownInfo().begin();
								CInventoryOwner::KnownInfo().end() != it; ++it)
	{
		//���������� ������� ���������� � ������� �� ��������
		CInfoPortion info_portion;
		info_portion.Load(*it);
	
		for(u32 i = 0; i<info_portion.DialogNames().size(); i++)
			AddAvailableDialog(*info_portion.DialogNames()[i], partner);
	}
	CPhraseDialogManager::UpdateAvailableDialogs(partner);
}

void CActor::TryToTalk()
{
	VERIFY(m_pPersonWeLookingAt);

	if(!IsTalking())
	{
		//���������� ���������� � ����
		if(m_pPersonWeLookingAt->OfferTalk(this))
		{	
			StartTalk(m_pPersonWeLookingAt);
			//������ ���� ��������� � ������ single
			CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
			if(pGameSP)pGameSP->StartTalk();
		}
	}
}