///////////////////////////////////////////////////////////
/// –ежим диалога в PDA
///////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIPdaWnd.h"

#include "../Pda.h"
#include "../InventoryOwner.h"
#include "../HUDManager.h"
#include "../character_info.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"

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


	// ƒекоративное оформление
	AttachChild(&UICharIconFrame);
	xml_init.InitFrameWindow(uiXml, "chicon_frame_window", 0, &UICharIconFrame);

	AttachChild(&UIPhrasesFrame);
	xml_init.InitFrameWindow(uiXml, "ph_frame_window", 0, &UIPhrasesFrame);

	AttachChild(&UIMsglogFrame);
	xml_init.InitFrameWindow(uiXml, "msglog_frame_window", 0, &UIMsglogFrame);

	UIMsglogFrame.AttachChild(&UILogListWnd);
	UILogListWnd.SetMessageTarget(this);
	xml_init.InitListWnd(uiXml, "list", 0, &UILogListWnd);
	UILogListWnd.ActivateList(false);
	UILogListWnd.EnableScrollBar(true);

	UIPhrasesFrame.AttachChild(&UIPhrasesListWnd);
	UIPhrasesListWnd.SetMessageTarget(this);
	xml_init.InitListWnd(uiXml, "list", 1, &UIPhrasesListWnd);
	UIPhrasesListWnd.EnableScrollBar(true);

	UICharIconFrame.AttachChild(&UICharacterWindow);
	UICharacterWindow.SetMessageTarget(this);
	xml_init.InitWindow(uiXml, "character_info", 0, &UICharacterWindow);
	
	UICharacterWindow.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0,UICharacterWindow.GetWidth(), 
							 UICharacterWindow.GetHeight(), 
							 PDA_DIALOG_CHAR_XML);

	//Ёлементы автоматического добавлени€
//	xml_init.InitAutoStatic(uiXml, "auto_static", this);

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

//добавление сообщени€ в лог
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
	UILogListWnd.AddItem(CPda::m_PdaMsgStr[msg]);
}

void  CUIPdaDialogWnd::PhrasesAnswer()
{	
	UIPhrasesListWnd.RemoveAll();
	UIPhrasesListWnd.AddItem(CPda::m_PdaMsgStr[ePdaMsgAccept], NULL, ePdaMsgAccept);
	UIPhrasesListWnd.AddItem(CPda::m_PdaMsgStr[ePdaMsgDecline], NULL, ePdaMsgDecline);
	UIPhrasesListWnd.AddItem(CPda::m_PdaMsgStr[ePdaMsgDeclineRude], NULL, ePdaMsgDeclineRude);
	//завершение диалога
	UIPhrasesListWnd.AddItem(CPda::m_PdaMsgStr[ePdaMsgILeave], NULL,ePdaMsgMax);
	
}
void CUIPdaDialogWnd::PhrasesAsk()
{
	UIPhrasesListWnd.RemoveAll();
	UIPhrasesListWnd.AddItem(CPda::m_PdaMsgStr[ePdaMsgTrade]	, NULL, ePdaMsgTrade);
	UIPhrasesListWnd.AddItem(CPda::m_PdaMsgStr[ePdaMsgNeedHelp]	, NULL, ePdaMsgNeedHelp);
	UIPhrasesListWnd.AddItem(CPda::m_PdaMsgStr[ePdaMsgGoAway]	, NULL, ePdaMsgGoAway);
	//завершение диалога
	UIPhrasesListWnd.AddItem(CPda::m_PdaMsgStr[ePdaMsgILeave], NULL, ePdaMsgMax);
}