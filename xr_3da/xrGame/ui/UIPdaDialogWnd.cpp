///////////////////////////////////////////////////////////
/// Режим диалога в PDA
///////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIPdaWnd.h"

#include "../Pda.h"
#include "../InventoryOwner.h"
#include "../HUDManager.h"
#include "../character_info.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"



#define YES_MSG "Yes"
#define NO_MSG "No"
#define GETLOST_MSG "Get Lost!"
#define TRADE_MSG "Trade?"
#define HELP_MSG "Need Help!"
#define GETOUT_MSG "Get Out Of Here!"
#define IAMLEAVE_MSG "I'm leave now!"

#define CONTACT_LOST	"The contact has been lost."
#define WAIT_FOR_REPLY	"Waiting for reply."


#define PDA_DIALOG_XML "pda_dialog.xml"
#define PDA_DIALOG_CHAR_XML	"pda_dialog_character.xml"

CUIPdaDialogWnd::CUIPdaDialogWnd()
{
}
CUIPdaDialogWnd::~CUIPdaDialogWnd()
{
}

void CUIPdaDialogWnd::Init(int x, int y, int width, int height)
{
	CUIXml uiXml;
	bool xml_result =uiXml.Init("$game_data$",PDA_DIALOG_XML);
	R_ASSERT2(xml_result, "xml file not found");
	CUIXmlInit xml_init;

	AttachChild(&UILogListWnd);
	xml_init.InitListWnd(uiXml, "list", 0, &UILogListWnd);
	UILogListWnd.ActivateList(false);
	UILogListWnd.EnableScrollBar(true);

	AttachChild(&UIPhrasesListWnd);
	xml_init.InitListWnd(uiXml, "list", 1, &UIPhrasesListWnd);
	UIPhrasesListWnd.EnableScrollBar(true);


	AttachChild(&UICharacterWindow);
	xml_init.InitWindow(uiXml, "character_info", 0, &UICharacterWindow);
	
	UICharacterWindow.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0,UICharacterWindow.GetWidth(), 
							 UICharacterWindow.GetHeight(), 
							 PDA_DIALOG_CHAR_XML);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);

	inherited::Init(x, y, width, height);
}

void CUIPdaDialogWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIPhrasesListWnd)
	{
		if(msg == CUIListWnd::LIST_ITEM_CLICKED)
		{
			m_iMsgNum = EPdaMsg(((CUIListItem*)pData)->GetValue());
			
			if(m_iMsgNum == ePdaMsgMax)
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

	ResetAll();
}

void CUIPdaDialogWnd::Hide()
{
	inherited::Show(false);
	inherited::Enable(false);
}

void CUIPdaDialogWnd::ContactLoss()
{
	UILogListWnd.AddItem(CONTACT_LOST);


	UIPhrasesListWnd.Show(false);
	UIPhrasesListWnd.Enable(false);
}

void CUIPdaDialogWnd::ContactWaitForReply()
{
	UILogListWnd.AddItem(WAIT_FOR_REPLY);

	UIPhrasesListWnd.Show(false);
	UIPhrasesListWnd.Enable(false);
}

void CUIPdaDialogWnd::ContactRestore()
{
	UIPhrasesListWnd.Show(true);
	UIPhrasesListWnd.Enable(true);
}

void CUIPdaDialogWnd::Update()
{
	inherited::Update();
}

//добавление сообщения в лог
void CUIPdaDialogWnd::AddOurMessageToLog(EPdaMsg msg, CInventoryOwner* pInvOwner)
{
	UILogListWnd.SetTextColor(0xFF00CCCC);
	UILogListWnd.AddItem(pInvOwner->CharacterInfo().Name());
	AddMessageToLog(msg, 0xFF00FFFF);
}
void CUIPdaDialogWnd::AddOthersMessageToLog(EPdaMsg msg, CInventoryOwner* pInvOwner)
{
	UILogListWnd.SetTextColor(0xFFCCCC00);
	UILogListWnd.AddItem(pInvOwner->CharacterInfo().Name());
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
	UIPhrasesListWnd.AddItem(IAMLEAVE_MSG, NULL, ePdaMsgMax);
	
}
void CUIPdaDialogWnd::PhrasesAsk()
{
	UIPhrasesListWnd.RemoveAll();
	UIPhrasesListWnd.AddItem(TRADE_MSG, NULL, ePdaMsgTrade);
	UIPhrasesListWnd.AddItem(HELP_MSG, NULL, ePdaMsgNeedHelp);
	UIPhrasesListWnd.AddItem(GETOUT_MSG, NULL, ePdaMsgGoAway);
	//завершение диалога
	UIPhrasesListWnd.AddItem(IAMLEAVE_MSG, NULL, ePdaMsgMax);
}