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

	m_bBackgroundPresent = false;
}

CUIProgressBar::~CUIProgressBar(void)
{
}

void CUIProgressBar::Init(int x, int y, int length, int broad, bool bIsHorizontal)
{
	m_bIsHorizontal = bIsHorizontal;


	if(m_bIsHorizontal)
	{
	//	m_UIStaticItem.Init("ui\\ui_scb_scroll_box", "hud\\default",
	//						x,y, alNone);

		CUIWindow::Init(x,y, length, broad);
		
/*		m_DecButton.Init("ui\\ui_scb_left_arrow", 0, 0, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_IncButton.Init("ui\\ui_scb_rigth_arrow",length-SCROLLBAR_WIDTH, 0,
						SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_ScrollBox.Init(SCROLLBAR_WIDTH, 0, length/2, SCROLLBAR_HEIGHT, m_bIsHorizontal);*/
	}
	else
	{
	//	m_UIStaticItem.Init("ui\\ui_scb_scroll_box", "hud\\default",
	//						x,y, alNone);

		CUIWindow::Init(x,y, broad, length);
/*		CUIWindow::Init(x,y, SCROLLBAR_WIDTH, length);
		m_DecButton.Init(0, 0, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
		m_IncButton.Init(0, length-SCROLLBAR_HEIGHT, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
	
	m_ScrollBox.Init(0, SCROLLBAR_HEIGHT, length/2, SCROLLBAR_WIDTH, m_bIsHorizontal);*/
	}


	UpdateProgressBar();
}

void CUIProgressBar::SetProgressTexture(const char* tex_name, 
											int progress_length, bool tile, u32 color)
{
	m_bProgressTile = tile;
	m_iProgressLength = progress_length;
	
	m_UIStaticItem.Init(tex_name, "hud\\default", 0, 0, alNone);
	m_UIStaticItem.SetColor(color);
}
void CUIProgressBar::SetBackgroundTexture(const char* tex_name, 
													int left_offset, int up_offset)
{
	m_bBackgroundPresent = true;
	m_iBackgroundLeftOffset = left_offset;
	m_iBackgroundUpOffset = up_offset;

	m_UIBackgroundItem.Init(tex_name, "hud\\default", 0, 0, alNone);
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

	//нарисовать подложку
	if(m_bBackgroundPresent)
	{
		m_UIBackgroundItem.SetPos(rect.left - m_iBackgroundLeftOffset, 
								  rect.top - m_iBackgroundUpOffset);
		m_UIBackgroundItem.Render();
	}



	m_UIStaticItem.SetPos(rect.left, rect.top);

	if(m_bIsHorizontal)
	{
		if(m_bProgressTile)
		{
			m_UIStaticItem.SetTile(m_iCurrentLength/(m_iProgressLength*2),1,	
							m_iCurrentLength%m_iProgressLength,0);
		}
		else
		{
			m_UIStaticItem.SetRect(0, 0, m_iCurrentLength, GetHeight());	
		}
	}
	else
	{
		if(m_bProgressTile)
		{
			m_UIStaticItem.SetTile(1,GetHeight()/(m_iProgressLength*2),0,
								GetHeight()%m_iProgressLength);
		}
		else
		{
			//m_UIStaticItem.SetRect(health_rect.x1,health_rect.y1,iFloor(float(health_rect.x2)*val),health_rect.y2);
			m_UIStaticItem.SetRect(0, m_iProgressLength-m_iCurrentLength,
											GetWidth(), m_iProgressLength);
		}
	}
	
	if(m_iCurrentLength>0)
		m_UIStaticItem.Render();
}