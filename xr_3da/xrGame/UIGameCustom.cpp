#include "stdafx.h"
#include "UIGameCustom.h"

CUIGameCustom::CUIGameCustom()
{
	m_Parent=NULL;
	uFlags=0;
	m_pUserMenu=NULL;

	shedule.t_min = 5;
	shedule.t_max = 20;
	shedule_register();
}

CUIGameCustom::~CUIGameCustom()
{
	shedule_unregister();
}

bool CUIGameCustom::IR_OnMouseMove(int dx,int dy)
{
	if(m_pUserMenu)
	{
		if(m_pUserMenu->IR_OnMouseMove(dx, dy)) 
			return true;
	}

	return false;
}

void CUIGameCustom::StartStopMenu(CUIDialogWnd* pDialog)
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

float CUIGameCustom::shedule_Scale		() 
{
	return 0.5f;
};

void CUIGameCustom::shedule_Update		(u32 dt)
{
	inherited::shedule_Update(dt);

	if(m_pUserMenu)
	{
		m_pUserMenu->Update();
	}
}

void CUIGameCustom::OnFrame() 
{
	if(m_pUserMenu)
	{
	//	m_pUserMenu->Update();
	}

}

void CUIGameCustom::Render()
{
	if(m_pUserMenu)
	{
		m_pUserMenu->Draw();
	}
}

bool CUIGameCustom::IR_OnKeyboardPress(int dik) 
{
	if(m_pUserMenu)
	{
		if(m_pUserMenu->IR_OnKeyboardPress(dik)) 
			return true;
	}
	return false;
}

bool CUIGameCustom::IR_OnKeyboardRelease(int dik) 
{
	if(m_pUserMenu)
	{
		if(m_pUserMenu->IR_OnKeyboardRelease(dik)) 
			return true;
	}
	return false;
}