#include "stdafx.h"
#include "uigamesp.h"

#include "actor.h"

CUIGameSP::CUIGameSP(CUI* parent) : CUIGameCustom(parent) 
{
	m_bUserMenuShown = false;
}

CUIGameSP::~CUIGameSP(void) {
}

void CUIGameSP::OnFrame() {
	if (InventoryMenu.Visible()) 
		InventoryMenu.OnFrame();
	else if (TradeMenu.IsShown()) 
		TradeMenu.Update();
	else if (PdaMenu.IsShown())
		PdaMenu.Update();
}

void CUIGameSP::Render()
{
	if (InventoryMenu.Visible()) 
		InventoryMenu.Render();
	else if (TradeMenu.IsShown()) 
		TradeMenu.Draw();
	else if (PdaMenu.IsShown())
		PdaMenu.Draw();
}


bool CUIGameSP::IR_OnKeyboardPress(int dik) 
{
	if (InventoryMenu.Visible())
	{
		if(InventoryMenu.IR_OnKeyboardPress(dik)) 
			return true;
	}
	else if(TradeMenu.IsEnabled())
	{
		if(TradeMenu.IR_OnKeyboardPress(dik))
			return true;
	}
	else if(PdaMenu.IsEnabled())
	{
		if(PdaMenu.IR_OnKeyboardPress(dik))
			return true;
	}


	
	switch (dik)
	{
	case DIK_I: 
		if(InventoryMenu.Visible())
		{
			m_bUserMenuShown = false;

			InventoryMenu.Hide();
			m_Parent->HideCursor();
			m_Parent->ShowIndicators();
			return true;
		}
		else
		{	
			if(!m_bUserMenuShown)
			{
				m_bUserMenuShown = true;

				//start the inventory menu
				InventoryMenu.Show();
				m_Parent->ShowCursor();
				m_Parent->HideIndicators();
				InventoryMenu.Init();
				return true;
			}
		}
		break;
	case DIK_T:
		//start trade menu (just a test)
		if(TradeMenu.IsShown())
		{
			m_bUserMenuShown  = false;

			TradeMenu.Show(false);
			TradeMenu.Enable(false);
			m_Parent->HideCursor();
			m_Parent->ShowIndicators();
		}
		else
		{
			if(!m_bUserMenuShown)
			{
				m_bUserMenuShown  = true;

				TradeMenu.Show(true);
				TradeMenu.Enable(true);
				m_Parent->ShowCursor();
				m_Parent->HideIndicators();
			}
		}
		return true;
		break;
	case DIK_P:
		//start trade PDA menu 
		if(PdaMenu.IsShown())
		{
			m_bUserMenuShown  = false;
			
			PdaMenu.Show(false);
			PdaMenu.Enable(false);
			m_Parent->HideCursor();
			m_Parent->ShowIndicators();
			
		}
		else
		{
			if(!m_bUserMenuShown)
			{
				m_bUserMenuShown  = true;

				PdaMenu.Show(true);
				PdaMenu.Enable(true);
				m_Parent->ShowCursor();
				m_Parent->HideIndicators();

				PdaMenu.InitPDA();
			}
		}
		return true;
		break;
	}
	
	
	//временно! пока, по убыванию сил до критического
	//уровня персонаж не засыпает, а просто переходит
	//в меню инвенторя
	CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(l_pA && l_pA->g_Alive() && !l_pA->GetPower())
	{
			if(!m_bUserMenuShown)
			{
				m_bUserMenuShown = true;

				//start the inventory menu
				InventoryMenu.Show();
				m_Parent->ShowCursor();
				m_Parent->HideIndicators();
				InventoryMenu.Init();
				return true;
			}
	}
	

	return false;
}

bool CUIGameSP::IR_OnKeyboardRelease(int dik) {

	if (InventoryMenu.Visible())
	{
		if(InventoryMenu.IR_OnKeyboardRelease(dik)) 
			return true;
	}
	else if(TradeMenu.IsEnabled())
	{ 
		if(TradeMenu.IR_OnKeyboardRelease(dik))
			return true;
	}
	else if(PdaMenu.IsEnabled())
	{ 
		if(PdaMenu.IR_OnKeyboardRelease(dik))
			return true;
	}
	return false;
}

bool CUIGameSP::IR_OnMouseMove(int dx,int dy)
{
	if (InventoryMenu.Visible())
	{
		if(InventoryMenu.IR_OnMouseMove(dx, dy)) 
			return true;
	}
	else if(TradeMenu.IsEnabled())
	{
		if(TradeMenu.IR_OnMouseMove(dx, dy))
			return true;
	}
	else if(PdaMenu.IsEnabled())
	{ 
		if(PdaMenu.IR_OnMouseMove(dx, dy))
			return true;
	}


	return false;
}