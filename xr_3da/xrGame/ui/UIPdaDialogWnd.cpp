///////////////////////////////////////////////////////////
/// Режим диалога в PDA
///////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIPdaWnd.h"

#include "../Pda.h"
#include "../InventoryOwner.h"
#include "../HUDManager.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"



#define YES_MSG "Yes"
#define NO_MSG "No"
#define GETLOST_MSG "Get Lost!"
#define TRADE_MSG "Trade?"
#define HELP_MSG "Need Help!"
#define GETOUT_MSG "Get Out Of Here!"
#define IAMLEAVE_MSG "I'm leave now!"




CUIPdaDialogWnd::CUIPdaDialogWnd()
{
}
CUIPdaDialogWnd::~CUIPdaDialogWnd()
{
}

void CUIPdaDialogWnd::Init(int x, int y, int width, int height)
{
	CUIXml uiXml;
	bool xml_result =uiXml.Init("$game_data$","pda_dialog.xml");
	R_ASSERT2(xml_result, "xml file not found");
	CUIXmlInit xml_init;

	AttachChild(&UILogListWnd);
	xml_init.InitListWnd(uiXml, "list", 0, &UILogListWnd);
	UILogListWnd.ActivateList(false);
	UILogListWnd.EnableScrollBar(true);

	AttachChild(&UIPhrasesListWnd);
	xml_init.InitListWnd(uiXml, "list", 1, &UIPhrasesListWnd);
	UIPhrasesListWnd.EnableScrollBar(true);



	AttachChild(&UIBackButton);
	UIBackButton.Init("ui\\ui_button_02", 10,height - 50, 150, 40);
	UIBackButton.SetText("<< Back");

	
	AttachChild(&UIMsgButton1);
	UIMsgButton1.Init("ui\\ui_button_02", 10, 30 , 150, 40);
	UIMsgButton1.SetText("Yes");

	AttachChild(&UIMsgButton2);
	UIMsgButton2.Init("ui\\ui_button_02", 10, 90, 150, 40);
	UIMsgButton2.SetText("No");

	AttachChild(&UIMsgButton3);
	UIMsgButton3.Init("ui\\ui_button_02", 10, 150, 150, 40);
	UIMsgButton3.SetText("Get Lost!");

	AttachChild(&UIStaticContactName);
	UIStaticContactName.Init(10,10, 150, 40);

	AttachChild(&UIStaticContactStatus);
	UIStaticContactStatus.Init(10,60, 150, 40);
	UIStaticContactStatus.SetText("The contact has been lost.");
	UIStaticContactStatus.Show(false);

	
	AttachChild(&UICharacterWindow);
	xml_init.InitWindow(uiXml, "character_info", 0, &UICharacterWindow);
	
	UICharacterWindow.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0,UICharacterWindow.GetWidth(), 
							 UICharacterWindow.GetHeight(), 
							 "pda_character.xml");


	inherited::Init(x, y, width, height);
}

void CUIPdaDialogWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIPhrasesListWnd)
	{
		if(msg == CUIListWnd::LIST_ITEM_CLICKED)
		{
			m_iMsgNum = (EPdaMsg)((CUIListItem*)pData)->GetValue();
			
			if(m_iMsgNum == ePdaMessageMax)
				GetTop()->SendMessage(this, BACK_BUTTON_CLICKED);
			else
				GetTop()->SendMessage(this, MESSAGE_BUTTON_CLICKED);
		}
	}

	inherited::SendMessage(pWnd, msg, pData);
}

void CUIPdaDialogWnd::Show()
{
	inherited::Show(true);
	inherited::Enable(true);
	UIBackButton.Reset();
}

void CUIPdaDialogWnd::Hide()
{
	inherited::Show(false);
	inherited::Enable(false);
}

void CUIPdaDialogWnd::ContactLoss()
{
	UIStaticContactStatus.SetText("The contact has been lost.");
	UIStaticContactStatus.Show(true);
	
	UIMsgButton1.Show(false);
	UIMsgButton1.Enable(false);
	UIMsgButton2.Show(false);
	UIMsgButton2.Enable(false);
	UIMsgButton3.Show(false);
	UIMsgButton3.Enable(false);
}

void CUIPdaDialogWnd::ContactRestore()
{
	UIStaticContactStatus.Show(false);
	
	UIMsgButton1.Show(true);
	UIMsgButton1.Enable(true);
	UIMsgButton2.Show(true);
	UIMsgButton2.Enable(true);
	UIMsgButton3.Show(true);
	UIMsgButton3.Enable(true);

	UIMsgButton1.Reset();
	UIMsgButton2.Reset();
	UIMsgButton3.Reset();

}

void CUIPdaDialogWnd::ContactWaitForReply()
{
	UIStaticContactStatus.SetText("Waiting for reply.");
	UIStaticContactStatus.Show(true);
	

}

void CUIPdaDialogWnd::Update()
{
	inherited::Update();
}

//добавление сообщения в лог
void CUIPdaDialogWnd::AddOurMessageToLog(EPdaMsg msg, CInventoryOwner* pInvOwner)
{
	UILogListWnd.SetTextColor(0xFF00CCCC);
	UILogListWnd.AddItem(pInvOwner->GetGameName());
	AddMessageToLog(msg, 0xFF00FFFF);
}
void CUIPdaDialogWnd::AddOthersMessageToLog(EPdaMsg msg, CInventoryOwner* pInvOwner)
{
	UILogListWnd.SetTextColor(0xFFCCCC00);
	UILogListWnd.AddItem(pInvOwner->GetGameName());
	AddMessageToLog(msg, 0xFFFFFF00);
}

void CUIPdaDialogWnd::AddMessageToLog(EPdaMsg msg, u32 color)
{		
	UILogListWnd.SetTextColor(color);

	switch(msg)
	{
	case ePdaMsgAccept:
		UILogListWnd.AddItem(YES_MSG);
		break;
	case ePdaMsgDecline:
		UILogListWnd.AddItem(NO_MSG);
		break;
	case ePdaMsgDeclineRude:
		UILogListWnd.AddItem(GETLOST_MSG);
		break;
	case ePdaMsgTrade:
		UILogListWnd.AddItem(TRADE_MSG);
		break;
	case ePdaMsgNeedHelp:
		UILogListWnd.AddItem(HELP_MSG);
		break;
	case ePdaMsgGoAway:
		UILogListWnd.AddItem(GETOUT_MSG);
		break;		
	default:
		UILogListWnd.AddItem("unknown message");
	}
}

void  CUIPdaDialogWnd::PhrasesAnswer()
{	
	UIPhrasesListWnd.RemoveAll();
	UIPhrasesListWnd.AddItem(YES_MSG, NULL, ePdaMsgAccept);
	UIPhrasesListWnd.AddItem(NO_MSG, NULL, ePdaMsgDecline);
	UIPhrasesListWnd.AddItem(GETLOST_MSG, NULL, ePdaMsgDeclineRude);
	//завершение диалога
	UIPhrasesListWnd.AddItem(IAMLEAVE_MSG, NULL, ePdaMessageMax);
	
}
void  CUIPdaDialogWnd::PhrasesAsk()
{
	UIPhrasesListWnd.RemoveAll();
	UIPhrasesListWnd.AddItem(TRADE_MSG, NULL, ePdaMsgTrade);
	UIPhrasesListWnd.AddItem(HELP_MSG, NULL, ePdaMsgNeedHelp);
	UIPhrasesListWnd.AddItem(GETOUT_MSG, NULL, ePdaMsgGoAway);
	//завершение диалога
	UIPhrasesListWnd.AddItem(IAMLEAVE_MSG, NULL, ePdaMessageMax);
}