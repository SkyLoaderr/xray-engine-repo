//-----------------------------------------------------------------------------/
//  ���������������� ��������� PDA
//-----------------------------------------------------------------------------/

#include "stdafx.h"
#include "UIPdaCommunication.h"
#include "../Pda.h"
#include "UIPdaWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "../HUDManager.h"
#include "../actor.h"
#include "../level.h"
#include "../character_info.h"

#include "../PhraseDialog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIPdaCommunication::CUIPdaCommunication()
{
	Show(false);

	m_pInvOwner = NULL;
	m_pPda = NULL;

	SetFont(HUD().Font().pFontMedium);

//	m_pOurDialogManager = m_pOthersDialogManager = NULL;

//	ToTopicMode();

//	m_bNeedToUpdateQuestions = false;
}

CUIPdaCommunication::~CUIPdaCommunication()
{
}

void CUIPdaCommunication::Init()
{
	CUIXml uiXml;
	uiXml.Init(CONFIG_PATH, UI_PATH,PDA_XML);
	CUIXmlInit xml_init;

	inherited::Init(0,0, float(Device.dwWidth), float(Device.dwHeight));

	//���� ��������� �� PDA
//	AttachChild(&UIPdaDialogWnd);
//	UIPdaDialogWnd.Init(0, 0, GetWidth(), GetHeight());
//	UIPdaDialogWnd.Show(false);

	//������ ���������
	AttachChild(&UIPdaContactsWnd);
	UIPdaContactsWnd.Init(0,0, GetWidth(), GetHeight());
	UIPdaContactsWnd.Show(false);

	//�������� ��������������� ����������
	xml_init.InitAutoStatic(uiXml, "auto_static", this);
}

void CUIPdaCommunication::InitPDACommunication()
{
	m_pInvOwner = smart_cast<CInventoryOwner*>(Level().CurrentEntity());
	if(!m_pInvOwner) return;

	m_pPda = m_pInvOwner->GetPDA();

	if(!m_pPda)	return;

	InitPdaContacts();

//	UIPdaDialogWnd.Show(false);
	UIPdaContactsWnd.Show(true);
}


void CUIPdaCommunication::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
/*	if(pWnd == &UIPdaContactsWnd)
	{
		if(msg == PDA_CONTACTS_WND_CONTACT_SELECTED)
		{
			UIPdaContactsWnd.Show(false);
			UIPdaDialogWnd.ContactRestore();

			InitPdaDialog();
			UIPdaDialogWnd.Show(true);
		}
	}*/
/*	else if(pWnd == &UIPdaDialogWnd)
	{
//		if(msg == CUIPdaDialogWnd::BACK_BUTTON_CLICKED)
//		{
//			UIPdaContactsWnd.Show(true);
//			UIPdaDialogWnd.Show(false);
//		}
		else 
		
		if(msg == PDA_DIALOG_WND_MESSAGE_BUTTON_CLICKED)
		{
			EPdaMsg pda_msg = ePdaMsgDialog;
			u32 id_pda_contact = m_pContactPda->ID();
//			
//			if(m_pPda->NeedToAnswer(id_pda_contact))
//			{
//				switch(UIPdaDialogWnd.m_iMsgNum)
//				{
//				case 0:
//					pda_msg = ePdaMsgAccept;
//					break;
//				case 1:
//					pda_msg = ePdaMsgDecline;
//					break;
//				case 2:
//					pda_msg = ePdaMsgDeclineRude;
//					break;
//				}
//			}
//			else
//			{
//				switch(UIPdaDialogWnd.m_iMsgNum)
//				{
//				case 0:
//					pda_msg = ePdaMsgTrade;
//					break;
//				case 1:
//					pda_msg = ePdaMsgGoAway;					
//					break;
//				case 2:
//					pda_msg = ePdaMsgNeedHelp;
//					break;
//				}
//			}
			m_pPda->SendMessageID(id_pda_contact, pda_msg, NULL);
			
			AskQuestion();
			UpdateMessageLog();
		}

	}*/
	inherited::SendMessage(pWnd, msg, pData);
}


void CUIPdaCommunication::Update()
{
	if(!m_pPda) return;

	//��������� ������ ��������� ��� �����������
	//�� ���� �������� ���� ��������� ��� ���
	UpdatePdaContacts();

	/*
	if(UIPdaDialogWnd.IsShown() && 
		m_pPda->IsNewMessage())
	{
		UpdateMessageLog();
		UpdateMsgButtons();

		m_pPda->NoNewMessage();
	}*/

	inherited::Update();
}

void CUIPdaCommunication::Draw()
{
	inherited::Draw();
}

void CUIPdaCommunication::Show(bool status)
{
	inherited::Show(status);
	if (status)
	{
		InitPDACommunication();
	}
}


/////////////////////////////////////////
// ������� ������ � ���������
////////////////////////////////////////
void CUIPdaCommunication::InitPdaContacts()
{
	UIPdaContactsWnd.RemoveAll();

	xr_vector<CPda*> pda_list;
	m_pPda->ActiveContacts(pda_list);

	xr_vector<CPda*>::iterator it = pda_list.begin();

	for(; it!=pda_list.end();++it){
		UIPdaContactsWnd.AddContact(*it);
	}
}

//���������� ������ �������� ��������� PDA
void CUIPdaCommunication::UpdatePdaContacts()
{
	xr_vector<CPda*> pda_list;
	m_pPda->ActiveContacts(pda_list);

	xr_vector<CPda*>::iterator it = pda_list.begin();

	for(; it!=pda_list.end();++it){
		CPda* pPda = (*it);
		if(!UIPdaContactsWnd.IsInList(pPda))
			UIPdaContactsWnd.RemoveContact(pPda);
	}
}
