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
void CActor::OnReceiveInfo(int info_index)
{
	//������ ���� ��������� � ������ single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	CInfoPortion info_portion;
	info_portion.Load(info_index);

	//�������� ������� �� �����
	for(int i=0; i<info_portion.GetLocationsNum(); i++)
		Level().AddMapLocation(info_portion.GetLocation(i));


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

void CActor::ReceivePdaMessage(u16 who, EPdaMsg msg, int info_index)
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
/*	//������ ���� ��������� � ������ single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	if(pGameSP->TalkMenu.IsShown())
	{
		pGameSP->TalkMenu.UpdateQuestions();
		pGameSP->TalkMenu.UpdateAnswers();
	}
*/
	CPhraseDialogManager::ReceivePhrase(phrase_dialog);
}

void   CActor::UpdateAvailableDialogs	()
{
	m_AvailableDialogs.clear();

	for(KNOWN_INFO_PAIR_IT it = GetPDA()->m_mapKnownInfo.begin();
								GetPDA()->m_mapKnownInfo.end() != it; ++it)
	{
		//���������� ������� ���������� � ������� �� ��������
		CInfoPortion info_portion;
		info_portion.Load((*it).first);
	
		for(u32 i = 0; i<info_portion.m_DialogNames.size(); i++)
		{
			DIALOG_SHARED_PTR phrase_dialog(xr_new<CPhraseDialog>());
			phrase_dialog->Load(info_portion.m_DialogNames[i]);
			m_AvailableDialogs.push_back(phrase_dialog);
		}
	}
	CPhraseDialogManager::UpdateAvailableDialogs();
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