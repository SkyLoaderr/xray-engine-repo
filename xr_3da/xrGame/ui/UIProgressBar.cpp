//////////////////////////////////////////////////////////////////////
// UIProgressBar.h: полоса процента выполнения
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiprogressbar.h"

CUIProgressBar::CUIProgressBar(void)
{
	m_iMinPos = 1;
	m_iMaxPos = 1;

	Enable(false);
}

CUIProgressBar::~CUIProgressBar(void)
{
}

void CUIProgressBar::Init(int x, int y, int length, bool bIsHorizontal)
{
	m_bIsHorizontal = bIsHorizontal;


	if(m_bIsHorizontal)
	{
		m_UIStaticItem.Init("ui\\ui_scb_scroll_box", "hud\\default",
							x,y, alNone);

		CUIWindow::Init(x,y, length, PROGRESSBAR_HEIGHT);
		
/*		m_DecButton.Init("ui\\ui_scb_left_arrow", 0, 0, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_IncButton.Init("ui\\ui_scb_rigth_arrow",length-SCROLLBAR_WIDTH, 0,
						SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_ScrollBox.Init(SCROLLBAR_WIDTH, 0, length/2, SCROLLBAR_HEIGHT, m_bIsHorizontal);*/
	}
	else
	{
		m_UIStaticItem.Init("ui\\ui_scb_scroll_box", "hud\\default",
							x,y, alNone);

		CUIWindow::Init(x,y, PROGRESSBAR_WIDTH, length);
/*		CUIWindow::Init(x,y, SCROLLBAR_WIDTH, length);
		m_DecButton.Init(0, 0, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_IncButton.Init(0, length-SCROLLBAR_HEIGHT, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
	
	m_ScrollBox.Init(0, SCROLLBAR_HEIGHT, length/2, SCROLLBAR_WIDTH, m_bIsHorizontal);*/
	}


	UpdateProgressBar();
}


void CUIProgressBar::UpdateProgressBar()
{
	//относительный размер единичного прокручемого элемента
	float progressbar_unit;
	progressbar_unit = (float)1/(m_iMaxPos-m_iMinPos);

	float fCurrentLength = m_iProgressPos*progressbar_unit;

	//утановить размер и положение каретки
	if(m_bIsHorizontal)
	{	
		m_iCurrentLength = (int)(GetWidth()*fCurrentLength); 	
	}
	else
	{
		m_iCurrentLength = (int)(GetHeight()*fCurrentLength); 	
	}
}



bool CUIProgressBar::ProgressDec()
{
	if(m_iProgressPos>m_iMinPos)
	{
		m_iProgressPos--;
		UpdateProgressBar();
		return true;
	}

	return false;
}


bool CUIProgressBar::ProgressInc()
{
	if(m_iProgressPos<m_iMaxPos)
	{
		m_iProgressPos++;
		UpdateProgressBar();
		return true;
	}

	return false;	
}


void CUIProgressBar::Draw()
{
	RECT rect = GetAbsoluteRect();

	m_UIStaticItem.SetPos(rect.left, rect.top);

	if(m_bIsHorizontal)
	{
		m_UIStaticItem.SetTile(m_iCurrentLength/4,1,m_iCurrentLength%2,0);
	}
	else
	{
		m_UIStaticItem.SetTile(1,GetHeight()/4,0,GetHeight()%2);
	}

	if(m_iCurrentLength>0)
		m_UIStaticItem.Render();
}