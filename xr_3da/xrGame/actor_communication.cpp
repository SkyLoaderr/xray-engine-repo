// actor_communication.cpp:	 связь по PDA и диалоги
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "actor.h"

#include "UIGameSP.h"
#include "UI.h"
#include "PDA.h"
#include "HUDManager.h"
#include "level.h"

//information receive
void CActor::OnReceiveInfo(int info_index)
{
	//только если находимся в режиме single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	CInfoPortion info_portion;
	info_portion.Load(info_index);

	//добавить отметки на карте
	for(int i=0; i<info_portion.GetLocationsNum(); i++)
		Level().AddMapLocation(info_portion.GetLocation(i));


	if(pGameSP->TalkMenu.IsShown())
	{
		if(pGameSP->TalkMenu.IsShown())
		{
			pGameSP->TalkMenu.UpdateQuestions();
		}
		//если пришло сообщение по PDA когда сам PDA не был запущен
		else if(!pGameSP->m_pUserMenu->IsShown())
		{
		}
	}

	CInventoryOwner::OnReceiveInfo(info_index);
}

void CActor::ReceivePdaMessage(u16 who, EPdaMsg msg, int info_index)
{
	//только если находимся в режиме single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	//визуализация в интерфейсе
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

void  CActor::ReceivePhrase		(DIALOG_SHARED_PRT phrase_dialog)
{
	CPhraseDialogManager::ReceivePhrase(phrase_dialog);
}