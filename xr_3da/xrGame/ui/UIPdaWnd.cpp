// UIPdaWnd.cpp:  диалог PDA
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIPdaWnd.h"
#include "..\\Pda.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"


#include "..\\HUDManager.h"


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

	Show(false);
	Enable(false);
	
	m_pInvOwner = NULL;
	m_pPda = NULL;

	SetFont(HUD().pFontHeaderRussian);
}

CUIPdaWnd::~CUIPdaWnd()
{
}

void CUIPdaWnd::Init()
{
	inherited::Init(0,0, Device.dwWidth, Device.dwHeight);

	AttachChild(&UIMainPdaFrame);
	UIMainPdaFrame.Init("ui\\ui_frame", 100,100, 600, 400);

	UIMainPdaFrame.AttachChild(&UIPdaDialogWnd);
	UIPdaDialogWnd.Init(10,10,550,350);
	UIPdaDialogWnd.Hide();
	

	UIMainPdaFrame.AttachChild(&UIPdaContactsWnd);
	UIPdaContactsWnd.Init(10,10, 550, 350);
	UIPdaContactsWnd.Show();
}

void CUIPdaWnd::InitPDA()
{
	//CActor *pInvOwner = dynamic_cast<CActor*>(Level().CurrentEntity());
	m_pInvOwner = dynamic_cast<CInventoryOwner*>(Level().CurrentEntity());
	if(!m_pInvOwner) return;
	
	m_pPda = m_pInvOwner->GetPDA();

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

bool CUIPdaWnd::IR_OnKeyboardPress(int dik)
{
	if(!IsEnabled()) return false;
			
	//mouse click
	if(dik==MOUSE_1)
	{
		OnMouse(HUD().GetUI()->GetCursor()->GetPos().x,
				HUD().GetUI()->GetCursor()->GetPos().y,
                CUIWindow::LBUTTON_DOWN);
		return true;
	}
	
	
	return 	OnKeyboard(dik,	CUIWindow::KEY_PRESSED);
}
bool CUIPdaWnd::IR_OnKeyboardRelease(int dik)
{
	if(!IsEnabled()) return false;
			
	//mouse click
	if(dik==MOUSE_1)
	{
			OnMouse(HUD().GetUI()->GetCursor()->GetPos().x,
			 	    HUD().GetUI()->GetCursor()->GetPos().y,	
					CUIWindow::LBUTTON_UP);
		return true;
	}

	return 	OnKeyboard(dik,	CUIWindow::KEY_RELEASED);
}
bool CUIPdaWnd::IR_OnMouseMove(int dx, int dy)
{
	if(!IsEnabled()) return false;
	
	if (HUD().GetUI()->GetCursor()->IsVisible())
	{ 
		HUD().GetUI()->GetCursor()->MoveBy(dx, dy);
		OnMouse(HUD().GetUI()->GetCursor()->GetPos().x, 
				HUD().GetUI()->GetCursor()->GetPos().y, 
				CUIWindow::MOUSE_MOVE);
	}

	return true;
}

void CUIPdaWnd::Update()
{
	//обновлять список контактов вне зависимости
	//от того показано окно контактов или нет
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
/////////////////////////////////////////
// Функции работы с котактами
////////////////////////////////////////
void CUIPdaWnd::InitPdaContacts()
{
	UIPdaContactsWnd.UIListWnd.RemoveAll();

	PDA_LIST_it it;
	for(it = m_pPda->m_PDAList.begin(); it!=m_pPda->m_PDAList.end(); it++)
	{
		CObject* pObject =  Level().Objects.net_Find((*it)->GetOriginalOwnerID());
		UIPdaContactsWnd.UIListWnd.AddItem((char*)pObject->cName(), pObject);
	}
}



//обновление списка активных контактов PDA
void CUIPdaWnd::UpdatePdaContacts()
{
	PDA_LIST_it it;	

	//удалить из списка все PDA ушедшие из зоны досягаемости
	for(it = m_pPda->m_DeletedPDAList.begin(); it!=m_pPda->m_DeletedPDAList.end(); it++)
	{	
		CObject* pObject =  Level().Objects.net_Find((*it)->GetOriginalOwnerID());
		UIPdaContactsWnd.UIListWnd.RemoveItem(UIPdaContactsWnd.UIListWnd.FindItem(pObject));
		
		//текущий контак вышел из зоны досягаемости!
		if(m_pContactInvOwner == (*it)->GetOriginalOwner())
		{
			UIPdaDialogWnd.ContactLoss();
		}
	}

	//добавить новые
	for(it = m_pPda->m_NewPDAList.begin(); it!=m_pPda->m_NewPDAList.end(); it++)
	{	
		CObject* pObject =  Level().Objects.net_Find((*it)->GetOriginalOwnerID());
		//только если объекта еще нет в списке
		if(UIPdaContactsWnd.UIListWnd.FindItem(pObject)==-1)
			UIPdaContactsWnd.UIListWnd.AddItem((char*)pObject->cName(), pObject);

		//текущий контак снова вошел в зону досягаемости
		if(m_pContactInvOwner == (*it)->GetOriginalOwner())
		{
			UIPdaDialogWnd.ContactRestore();
		}

	}
}



/////////////////////////////////////////
// Функции работы для поддержания диалога
////////////////////////////////////////

void CUIPdaWnd::InitPdaDialog()
{
	m_idContact = UIPdaContactsWnd.GetContactID();

	m_pContactObject =  Level().Objects.net_Find(m_idContact);
	
	R_ASSERT2(m_pContactObject, "wrong ID");
	m_pContactInvOwner = dynamic_cast<CInventoryOwner*>(m_pContactObject);
	R_ASSERT2(m_pContactInvOwner, "can't cast to inventory owner");

	//что за глюк?! херит память если выводить m_pContactObject->cName()

//	sprintf(UIPdaDialogWnd.m_sContactName, "%s", m_pContactObject->cName());
//	UIPdaDialogWnd.UIStaticContactName.SetText(UIPdaDialogWnd.m_sContactName);

	m_pInvOwner  = dynamic_cast<CInventoryOwner*>(Level().CurrentEntity());;
	R_ASSERT2(m_pInvOwner, "wrong inventory owner");


	UpdateMessageLog();
	UpdateMsgButtons();
}

//заполнить окно логами сообщений с текущим контактом
void CUIPdaWnd::UpdateMessageLog()
{
	UIPdaDialogWnd.UILogListWnd.RemoveAll();
	
	u32 id_pda_contact;
	
	if(m_pContactInvOwner)
		id_pda_contact = m_pContactInvOwner->GetPDA()->ID();

	//котактов еще не было
	if(m_pPda->m_mapPdaLog.find(id_pda_contact) == m_pPda->m_mapPdaLog.end()) return;

	for(CPda::PDA_MESSAGE_LIST_it it = m_pPda->m_mapPdaLog[id_pda_contact].begin();
							      it!= m_pPda->m_mapPdaLog[id_pda_contact].end();
							      it++)
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


//оновления состояния диалога с контактом:
//ожидание ответа, задавание вопроса,
//ответ на вопрос
//и обновление кнопок ответа
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

