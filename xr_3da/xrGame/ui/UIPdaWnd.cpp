// UIPdaWnd.cpp:  ������ PDA
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIPdaWnd.h"
#include "../Pda.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"


#include "../HUDManager.h"


#define YES_MSG "Yes"
#define NO_MSG "No"
#define GETLOST_MSG "Get Lost!"
#define TRADE_MSG "Trade?"
#define HELP_MSG "Need Help!"
#define GETOUT_MSG "Get Out Of Here!"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIPdaWnd::CUIPdaWnd()
{
	Init();

	Hide();
	
	m_pInvOwner = NULL;
	m_pPda = NULL;

	SetFont(HUD().pFontHeaderRussian);
}

CUIPdaWnd::~CUIPdaWnd()
{
}

void CUIPdaWnd::Init()
{
	CUIXml uiXml;
	uiXml.Init("$game_data$","pda.xml");
	CUIXmlInit xml_init;

	inherited::Init(0,0, Device.dwWidth, Device.dwHeight);

	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_top_background", 0,0,1024,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);


	AttachChild(&UIMainPdaFrame);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIMainPdaFrame);
	//UIMainPdaFrame.Init("ui\\ui_frame", 100,100, 600, 400);

	UIMainPdaFrame.AttachChild(&UIPdaDialogWnd);
	UIPdaDialogWnd.Init(10,10,550,350);
	UIPdaDialogWnd.Hide();
	

	UIMainPdaFrame.AttachChild(&UIPdaContactsWnd);
	UIPdaContactsWnd.Init(10,10, 550, 350);
	UIPdaContactsWnd.Show();
}

void CUIPdaWnd::InitPDA()
{
	m_pInvOwner = dynamic_cast<CInventoryOwner*>(Level().CurrentEntity());
	if(!m_pInvOwner) 
	{
		Hide();
		return;
	}
	
	m_pPda = m_pInvOwner->GetPDA();

	if(!m_pPda)
	{
		Hide();
		return;
	}

	InitPdaContacts();


	UIPdaDialogWnd.Hide();
	UIPdaContactsWnd.Show();
}


void CUIPdaWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == &UIPdaContactsWnd)
	{
			if(msg == CUIPdaContactsWnd::CONTACT_SELECTED)
			{
				UIPdaContactsWnd.Hide();
				UIPdaDialogWnd.ContactRestore();

				InitPdaDialog();
				UIPdaDialogWnd.Show();
			}
	}
	else if(pWnd == &UIPdaDialogWnd)
	{
			if(msg == CUIPdaDialogWnd::BACK_BUTTON_CLICKED)
			{
				UIPdaContactsWnd.Show();
				UIPdaDialogWnd.Hide();
			}
			else if(msg == CUIPdaDialogWnd::MESSAGE_BUTTON_CLICKED)
			{
				EPdaMsg pda_msg = ePdaMsgAccept;
				u32 id_pda_contact = m_pContactInvOwner->GetPDA()->ID();
				
				if(m_pPda->NeedToAnswer(id_pda_contact))
				{
					switch(UIPdaDialogWnd.m_iMsgNum)
					{
					case 0:
						pda_msg = ePdaMsgAccept;
						break;
					case 1:
						pda_msg = ePdaMsgDecline;
						break;
					case 2:
						pda_msg = ePdaMsgDeclineRude;
						break;
					}
				}
				else
				{
					switch(UIPdaDialogWnd.m_iMsgNum)
					{
					case 0:
						pda_msg = ePdaMsgTrade;
						break;
					case 1:
						pda_msg = ePdaMsgNeedHelp;
						break;
					case 2:
						pda_msg = ePdaMsgGoAway;
						break;
					}
				}


				m_pPda->SendMessageID(id_pda_contact, pda_msg, ePdaMsgAngerNone);

				UpdateMessageLog();
			}
	}

	inherited::SendMessage(pWnd, msg, pData);
}


void CUIPdaWnd::Update()
{
	//��������� ������ ��������� ��� �����������
	//�� ���� �������� ���� ��������� ��� ���
	UpdatePdaContacts();


	if(UIPdaDialogWnd.IsShown() && 
		m_pPda->IsNewMessage())
	{
		UpdateMessageLog();
		UpdateMsgButtons();
	
		m_pPda->NoNewMessage();
	}
	
	inherited::Update();
}

void CUIPdaWnd::Draw()
{
	inherited::Draw();
}

void CUIPdaWnd::Show()
{
	inherited::Show();
	InitPDA();
}


/////////////////////////////////////////
// ������� ������ � ���������
////////////////////////////////////////
void CUIPdaWnd::InitPdaContacts()
{
	UIPdaContactsWnd.RemoveAll();

	PDA_LIST_it it;
	for(it = m_pPda->m_PDAList.begin(); m_pPda->m_PDAList.end() != it; ++it)
	{
		UIPdaContactsWnd.AddContact((*it)->GetOwnerObject());
	}
}

//���������� ������ �������� ��������� PDA
void CUIPdaWnd::UpdatePdaContacts()
{
	PDA_LIST_it it;	

	//������� �� ������ ��� PDA ������� �� ���� ������������
	for(it = m_pPda->m_DeletedPDAList.begin(); m_pPda->m_DeletedPDAList.end() != it; ++it)
	{	
		UIPdaContactsWnd.RemoveContact((*it)->GetOwnerObject());
		
		//������� ������ ����� �� ���� ������������!
		if(m_pContactInvOwner == (*it)->GetOriginalOwner())
		{
			UIPdaDialogWnd.ContactLoss();
		}
	}

	//�������� �����
	for(it = m_pPda->m_NewPDAList.begin(); m_pPda->m_NewPDAList.end() != it; ++it)
	{	
		//������ ���� ������� ��� ��� � ������
		if(UIPdaContactsWnd.IsInList((*it)->GetOwnerObject())==false)
			UIPdaContactsWnd.AddContact((*it)->GetOwnerObject());

		//������� ������ ����� ����� � ���� ������������
		if(m_pContactInvOwner == (*it)->GetOriginalOwner())
		{
			UIPdaDialogWnd.ContactRestore();
		}
	}
}



/////////////////////////////////////////
// ������� ������ ��� ����������� �������
////////////////////////////////////////

void CUIPdaWnd::InitPdaDialog()
{
	m_idContact = UIPdaContactsWnd.GetContactID();

	m_pContactObject =  Level().Objects.net_Find(m_idContact);
	
	R_ASSERT2(m_pContactObject, "wrong ID");
	m_pContactInvOwner = dynamic_cast<CInventoryOwner*>(m_pContactObject);
	R_ASSERT2(m_pContactInvOwner, "can't cast to inventory owner");

	//��� �� ����?! ����� ������ ���� �������� m_pContactObject->cName()

//	sprintf(UIPdaDialogWnd.m_sContactName, "%s", m_pContactObject->cName());
//	UIPdaDialogWnd.UIStaticContactName.SetText(UIPdaDialogWnd.m_sContactName);

	m_pInvOwner  = dynamic_cast<CInventoryOwner*>(Level().CurrentEntity());;
	R_ASSERT2(m_pInvOwner, "wrong inventory owner");


	UpdateMessageLog();
	UpdateMsgButtons();
}

//��������� ���� ������ ��������� � ������� ���������
void CUIPdaWnd::UpdateMessageLog()
{
	UIPdaDialogWnd.UILogListWnd.RemoveAll();
	
	u32 id_pda_contact;
	
	if(m_pContactInvOwner)
		id_pda_contact = m_pContactInvOwner->GetPDA()->ID();

	//�������� ��� �� ����
	if(m_pPda->m_mapPdaLog.find(id_pda_contact) == m_pPda->m_mapPdaLog.end()) return;

	for(CPda::PDA_MESSAGE_LIST_it it = m_pPda->m_mapPdaLog[id_pda_contact].begin();
							      m_pPda->m_mapPdaLog[id_pda_contact].end() != it;
							      ++it)
	{
		switch((*it).msg)
		{
		case ePdaMsgAccept:
			UIPdaDialogWnd.UILogListWnd.AddItem(YES_MSG);
			break;
		case ePdaMsgDecline:
			UIPdaDialogWnd.UILogListWnd.AddItem(NO_MSG);
			break;
		case ePdaMsgDeclineRude:
			UIPdaDialogWnd.UILogListWnd.AddItem(GETLOST_MSG);
			break;
		case ePdaMsgTrade:
			UIPdaDialogWnd.UILogListWnd.AddItem(TRADE_MSG);
			break;
		case ePdaMsgNeedHelp:
			UIPdaDialogWnd.UILogListWnd.AddItem(HELP_MSG);
			break;			
		case ePdaMsgGoAway:
			UIPdaDialogWnd.UILogListWnd.AddItem(GETOUT_MSG);
			break;		
		default:
			UIPdaDialogWnd.UILogListWnd.AddItem("unknown message");
		}
	}
}


//��������� ��������� ������� � ���������:
//�������� ������, ��������� �������,
//����� �� ������
//� ���������� ������ ������
void CUIPdaWnd::UpdateMsgButtons()
{
	u32 id_pda_contact = 0xffff;
	
	if(m_pContactInvOwner)
		id_pda_contact = m_pContactInvOwner->GetPDA()->ID();
	

	if(m_pPda->WaitForReply(id_pda_contact))
	{
		UIPdaDialogWnd.ContactWaitForReply();
	}
	else
	{
		UIPdaDialogWnd.ContactRestore();
	}


	if(m_pPda->NeedToAnswer(id_pda_contact))
	{
		UIPdaDialogWnd.UIMsgButton1.SetText(YES_MSG);
		UIPdaDialogWnd.UIMsgButton2.SetText(NO_MSG);
		UIPdaDialogWnd.UIMsgButton3.SetText(GETLOST_MSG);
	}
	else
	{
		UIPdaDialogWnd.UIMsgButton1.SetText(TRADE_MSG);
		UIPdaDialogWnd.UIMsgButton2.SetText(HELP_MSG);
		UIPdaDialogWnd.UIMsgButton3.SetText(GETOUT_MSG);
	}
}