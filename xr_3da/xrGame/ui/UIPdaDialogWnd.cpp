///////////////////////////////////////////////////////////
/// ����� ������� � PDA
///////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIPdaWnd.h"

#include "../Pda.h"
#include "../InventoryOwner.h"
#include "../HUDManager.h"
#include "../character_info.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../Level.h"

#define CONTACT_LOST	"The contact has been lost."
#define WAIT_FOR_REPLY	"Waiting for reply."

#define PDA_DIALOG_XML "pda_dialog.xml"
#define PDA_DIALOG_CHAR_XML	"pda_dialog_character.xml"

CUIPdaDialogWnd::CUIPdaDialogWnd()
{
	m_iClickedQuestion = -1;
}
CUIPdaDialogWnd::~CUIPdaDialogWnd()
{
}

void CUIPdaDialogWnd::Init(int x, int y, int width, int height)
{
	CUIXml		uiXml;
	bool xml_result =uiXml.Init("$game_data$",PDA_DIALOG_XML);
	R_ASSERT2(xml_result, "xml file not found");
	CUIXmlInit	xml_init;

	// ������������ ����������
	AttachChild(&UICharIconFrame);
	xml_init.InitFrameWindow(uiXml, "chicon_frame_window", 0, &UICharIconFrame);
	UICharIconFrame.AttachChild(&UICharIconHeader);
	xml_init.InitFrameLine(uiXml, "chicon_frame_line", 0, &UICharIconHeader);
	UICharIconHeader.AttachChild(&UIAnimatedIcon);
	xml_init.InitAnimatedStatic(uiXml, "a_static", 0, &UIAnimatedIcon);

	AttachChild(&UIPhrasesFrame);
	xml_init.InitFrameWindow(uiXml, "ph_frame_window", 0, &UIPhrasesFrame);
	UIPhrasesFrame.AttachChild(&UIPhrasesHeader);
	xml_init.InitFrameLine(uiXml, "ph_frame_line", 0, &UIPhrasesHeader);
	UIPhrasesHeader.UITitleText.SetText(ALL_PDA_HEADER_PREFIX);

	AttachChild(&UIMsglogFrame);
	xml_init.InitFrameWindow(uiXml, "msglog_frame_window", 0, &UIMsglogFrame);
	UIMsglogFrame.AttachChild(&UIMsglogHeader);
	xml_init.InitFrameLine(uiXml, "msglog_frame_line", 0, &UIMsglogHeader);
	UIMsglogHeader.UITitleText.SetText(UICharacterInfo.UIName.GetText());

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
	UICharacterInfo.UIName.Show(false);
	
	xml_init.InitFrameWindow(uiXml, "mask_frame_window", 0, &UIMask);
	UICharacterInfo.UIIcon.SetMask(&UIMask);

	// Some text captions
	UICharacterWindow.AttachChild(&UIMTStatic);
	xml_init.InitMultiTextStatic(uiXml, "charinfo_mt_static", 0, &UIMTStatic);

	//�������� ��������������� ����������
//	xml_init.InitAutoStatic(uiXml, "auto_static", this);

	// ����� ��� ��������� ����� ��������� � ���� ���������
	xml_init.InitFont(uiXml, "font", 0, m_iNameTextColor, m_pNameTextFont);
	CGameFont * pFont = NULL;
	xml_init.InitFont(uiXml, "font", 1, m_uOurReplicsColor, pFont);

	inherited::Init(x, y, width, height);
}

void CUIPdaDialogWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIPhrasesListWnd)
	{
		if(msg == CUIListWnd::LIST_ITEM_CLICKED)
		{
			m_iMsgNum = EPdaMsg(((CUIListItem*)pData)->GetValue());
			m_iClickedQuestion = ((CUIListItem*)pData)->GetValue();
			
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
	UILogListWnd.AddItem<CUIListItem>(CONTACT_LOST);


	UIPhrasesListWnd.Show(false);
	UIPhrasesListWnd.Enable(false);
}

void CUIPdaDialogWnd::ContactWaitForReply()
{
	UILogListWnd.AddItem<CUIListItem>(WAIT_FOR_REPLY);

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

//���������� ��������� � ���
void CUIPdaDialogWnd::AddOurMessageToLog(EPdaMsg msg, CInventoryOwner* pInvOwner)
{
	UILogListWnd.SetTextColor(0xFF00CCCC);
	UILogListWnd.AddItem<CUIListItem>(pInvOwner->CharacterInfo().Name());
	AddMessageToLog(msg, 0xFF00FFFF);
}
void CUIPdaDialogWnd::AddOthersMessageToLog(EPdaMsg msg, CInventoryOwner* pInvOwner)
{
	UILogListWnd.SetTextColor(0xFFCCCC00);
	UILogListWnd.AddItem<CUIListItem>(pInvOwner->CharacterInfo().Name());
	AddMessageToLog(msg, 0xFFFFFF00);
}

void CUIPdaDialogWnd::AddMessageToLog(EPdaMsg msg, u32 color)
{		
	UILogListWnd.SetTextColor(color);
	UILogListWnd.AddItem<CUIListItem>(CPda::m_PdaMsgStr[msg]);
}

void  CUIPdaDialogWnd::PhrasesAnswer()
{	
	UIPhrasesListWnd.RemoveAll();
	UIPhrasesListWnd.AddItem<CUIListItem>(CPda::m_PdaMsgStr[ePdaMsgAccept], 0, NULL, ePdaMsgAccept);
	UIPhrasesListWnd.AddItem<CUIListItem>(CPda::m_PdaMsgStr[ePdaMsgDecline], 0, NULL, ePdaMsgDecline);
	UIPhrasesListWnd.AddItem<CUIListItem>(CPda::m_PdaMsgStr[ePdaMsgDeclineRude], 0, NULL, ePdaMsgDeclineRude);
	//���������� �������
	UIPhrasesListWnd.AddItem<CUIListItem>(CPda::m_PdaMsgStr[ePdaMsgILeave], 0, NULL,ePdaMsgMax);
	
}
void CUIPdaDialogWnd::PhrasesAsk()
{
	UIPhrasesListWnd.RemoveAll();
	UIPhrasesListWnd.AddItem<CUIListItem>(CPda::m_PdaMsgStr[ePdaMsgTrade], 0, NULL, ePdaMsgTrade);
	UIPhrasesListWnd.AddItem<CUIListItem>(CPda::m_PdaMsgStr[ePdaMsgNeedHelp], 0, NULL, ePdaMsgNeedHelp);
	UIPhrasesListWnd.AddItem<CUIListItem>(CPda::m_PdaMsgStr[ePdaMsgGoAway], 0, NULL, ePdaMsgGoAway);
	//���������� �������
	UIPhrasesListWnd.AddItem<CUIListItem>(CPda::m_PdaMsgStr[ePdaMsgILeave], 0, NULL, ePdaMsgMax);
}