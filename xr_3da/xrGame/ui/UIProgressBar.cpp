//////////////////////////////////////////////////////////////////////
// UIProgressBar.h: полоса процента выполнения
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uiprogressbar.h"

CUIProgressBar::CUIProgressBar(void)
{
	m_iMinPos				= 1;
	m_iMaxPos				= 1;

	Enable					(false);

	m_bBackgroundPresent	= false;
	m_bUseColor				= false;

	AttachChild(&m_UIBackgroundItem);
	AttachChild(&m_UIProgressItem);
}

CUIProgressBar::~CUIProgressBar(void)
{
}

void CUIProgressBar::Init(float x, float y, float width, float height, bool bIsHorizontal)
{
	m_bIsHorizontal = bIsHorizontal;
	CUIWindow::Init(x,y, width, height);
	UpdateProgressBar();
}

void CUIProgressBar::UpdateProgressBar()
{
	//относительный размер единичного прокручемого элемента
	float progressbar_unit;
	if(m_iMaxPos==m_iMinPos)
		m_iMaxPos++;

	progressbar_unit = (float)1/(m_iMaxPos-m_iMinPos);

	float fCurrentLength = m_iProgressPos*progressbar_unit;

	//утановить размер и положение каретки
	if(m_bIsHorizontal)	m_iCurrentLength = GetWidth()*fCurrentLength; 	
	else				m_iCurrentLength = GetHeight()*fCurrentLength; 	

	if(m_bUseColor){
		Fcolor curr;
		curr.lerp							(m_minColor,m_maxColor,fCurrentLength);
		m_UIProgressItem.GetStaticItem()->SetColor			(curr);
//		m_UIBackgroundItem.SetColor			(0x70000000);
	}
}

bool CUIProgressBar::ProgressDec()
{
	if(m_iProgressPos>m_iMinPos){
		--m_iProgressPos;
		UpdateProgressBar();
		return true;
	}

	return false;
}

bool CUIProgressBar::ProgressInc()
{
	if(m_iProgressPos<m_iMaxPos){
		++m_iProgressPos;
		UpdateProgressBar();
		return true;
	}

	return false;	
}

void CUIProgressBar::Draw()
{
	Frect rect = GetAbsoluteRect();

	if(m_bBackgroundPresent){
		UI()->PushScissor	(rect);		
		m_UIBackgroundItem.Draw();
		UI()->PopScissor	();
	}

	Frect progress_rect;

	if(m_bIsHorizontal){
		progress_rect.set	(0, 0, m_iCurrentLength, GetHeight());
	}else{
		progress_rect.set	(0, GetHeight()-m_iCurrentLength,
							GetWidth(), GetHeight());
	}
	
	if(m_iCurrentLength>0){
		Fvector2 pos = m_UIProgressItem.GetWndPos();	
		progress_rect.add	(rect.left + pos.x,rect.top + pos.y);

		UI()->PushScissor	(progress_rect);
		m_UIProgressItem.Draw();
		UI()->PopScissor	();
	}
}