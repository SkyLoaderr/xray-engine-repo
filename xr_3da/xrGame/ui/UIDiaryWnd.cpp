// CUIDiaryWnd.cpp:  дневник и все что с ним связано
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIDiaryWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"


#include "..\\HUDManager.h"





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIDiaryWnd::CUIDiaryWnd()
{
	Init();
	Hide();	
	SetFont(HUD().pFontHeaderRussian);
}

CUIDiaryWnd::~CUIDiaryWnd()
{
}

void CUIDiaryWnd::Init()
{
	inherited::Init(0,0, Device.dwWidth, Device.dwHeight);

	AttachChild(&UIMainDiaryFrame);
	UIMainDiaryFrame.Init("ui\\ui_frame", 100,100, 600, 400);

}

void CUIDiaryWnd::InitDiary()
{
}


void CUIDiaryWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
/*	if(pWnd == &UIDiaryContactsWnd)
	{
			if(msg == CUIDiaryContactsWnd::CONTACT_SELECTED)
			{
				UIDiaryContactsWnd.Hide();
				UIDiaryDialogWnd.ContactRestore();

				InitDiaryDialog();
				UIDiaryDialogWnd.Show();
			}
	}
	else if(pWnd == &UIDiaryDialogWnd)
	{
			if(msg == CUIDiaryDialogWnd::BACK_BUTTON_CLICKED)
			{
				UIDiaryContactsWnd.Show();
				UIDiaryDialogWnd.Hide();
			}
			else if(msg == CUIDiaryDialogWnd::MESSAGE_BUTTON_CLICKED)
			{
				EDiaryMsg pda_msg = eDiaryMsgAccept;
				u32 id_pda_contact = m_pContactInvOwner->GetPDA()->ID();
				
				if(m_pDiary->NeedToAnswer(id_pda_contact))
				{
					switch(UIDiaryDialogWnd.m_iMsgNum)
					{
					case 0:
						pda_msg = eDiaryMsgAccept;
						break;
					case 1:
						pda_msg = eDiaryMsgDecline;
						break;
					case 2:
						pda_msg = eDiaryMsgDeclineRude;
						break;
					}
				}
				else
				{
					switch(UIDiaryDialogWnd.m_iMsgNum)
					{
					case 0:
						pda_msg = eDiaryMsgTrade;
						break;
					case 1:
						pda_msg = eDiaryMsgNeedHelp;
						break;
					case 2:
						pda_msg = eDiaryMsgGoAway;
						break;
					}
				}


				m_pDiary->SendMessageID(id_pda_contact, pda_msg, eDiaryMsgAngerNone);

				UpdateMessageLog();
			}
	}

	inherited::SendMessage(pWnd, msg, pData);*/
}


void CUIDiaryWnd::Update()
{
	inherited::Update();
}

void CUIDiaryWnd::Draw()
{
	inherited::Draw();
}

void CUIDiaryWnd::Show()
{
	InitDiary();
	inherited::Show();
}