#include "stdafx.h"
#include "uigamesp.h"

#include "actor.h"

CUIGameSP::CUIGameSP(CUI* parent) : CUIGameCustom(parent) 
{
	m_pUserMenu = NULL;
}

CUIGameSP::~CUIGameSP(void) 
{
}

void CUIGameSP::OnFrame() 
{
	if(m_pUserMenu)
	{
		m_pUserMenu->Update();
	}
}

void CUIGameSP::Render()
{
	if(m_pUserMenu)
	{
		m_pUserMenu->Draw();
	}
}


bool CUIGameSP::IR_OnKeyboardPress(int dik) 
{
	if(m_pUserMenu)
	{
		if(m_pUserMenu->IR_OnKeyboardPress(dik)) 
			return true;
	}


	switch (dik)
	{
	case DIK_I: 
		StartStopMenu(&InventoryMenu);
		return true;
		break;
	case DIK_T:
		StartStopMenu(&TalkMenu);
		return true;
		break;
	case DIK_P:
		StartStopMenu(&PdaMenu);
		return true;
		break;
	case DIK_M:
		StartStopMenu(&MapMenu);
		return true;
		break;
	case DIK_N:
		StartStopMenu(&DiaryMenu);
		return true;
		break;
	case DIK_ESCAPE:
		if(m_pUserMenu)
		{
			StartStopMenu(m_pUserMenu);
			return true;
		}
		break;
	}
	
	
	//временно! пока, по убыванию сил до критического
	//уровня персонаж не засыпает, а просто переходит
	//в меню инвенторя
	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(pActor && pActor->g_Alive() && !pActor->GetPower())
	{
			if(m_pUserMenu == NULL)
			{
				//start the inventory menu
				StartStopMenu(&InventoryMenu);
				return true;
			}
	}
	

	return false;
}

bool CUIGameSP::IR_OnKeyboardRelease(int dik) 
{
	if(m_pUserMenu)
	{
		if(m_pUserMenu->IR_OnKeyboardRelease(dik)) 
			return true;
	}

	return false;
}

bool CUIGameSP::IR_OnMouseMove(int dx,int dy)
{
	if(m_pUserMenu)
	{
		if(m_pUserMenu->IR_OnMouseMove(dx, dy)) 
			return true;
	}

	return false;
}

void CUIGameSP::StartStopMenu(CUIDialogWnd* pDialog)
{
	if(m_pUserMenu == NULL)
	{
		//show menu 
		m_pUserMenu = pDialog;
		m_pUserMenu->Show();
	}
	else
	{
        if(m_pUserMenu == pDialog)
		{
			//hide menu 
            m_pUserMenu->Hide();
			m_pUserMenu = NULL;
		}
	}
}
void CUIGameSP::StartTalk()
{
	StartStopMenu(&TalkMenu);
}