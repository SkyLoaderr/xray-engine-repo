#include "stdafx.h"
#include "uiprogressbar.h"

CUIProgressBar::CUIProgressBar(void)
{
	m_MinPos				= 1.0f;
	m_MaxPos				= 1.0f+EPS;

	Enable					(false);

	m_bBackgroundPresent	= false;
	m_bUseColor				= false;

	AttachChild				(&m_UIBackgroundItem);
	AttachChild				(&m_UIProgressItem);
}

CUIProgressBar::~CUIProgressBar(void)
{
}

void CUIProgressBar::Init(float x, float y, float width, float height, bool bIsHorizontal)
{
	m_bIsHorizontal			= bIsHorizontal;
	CUIWindow::Init			(x,y, width, height);
	UpdateProgressBar		();
}

void CUIProgressBar::UpdateProgressBar()
{
	float progressbar_unit;
	if( fsimilar(m_MaxPos,m_MinPos) )
		m_MaxPos	+= EPS;

	progressbar_unit = 1/(m_MaxPos-m_MinPos);

	float fCurrentLength = m_ProgressPos*progressbar_unit;

	if(m_bIsHorizontal)	m_CurrentLength	= GetWidth()*fCurrentLength; 	
	else				m_CurrentLength	= GetHeight()*fCurrentLength; 	

	if(m_bUseColor){
		Fcolor curr;
		curr.lerp							(m_minColor,m_maxColor,fCurrentLength);
		m_UIProgressItem.GetStaticItem		()->SetColor			(curr);
	}
}

void CUIProgressBar::Draw()
{
	Frect					rect;
	GetAbsoluteRect			(rect);

	if(m_bBackgroundPresent){
		UI()->PushScissor	(rect);		
		m_UIBackgroundItem.Draw();
		UI()->PopScissor	();
	}

	Frect progress_rect;

	if(m_bIsHorizontal){
		progress_rect.set	(0, 0, m_CurrentLength, GetHeight());
	}else{
		progress_rect.set	(0, GetHeight()-m_CurrentLength,
							GetWidth(), GetHeight());
	}
	
	if(m_CurrentLength>0){
		Fvector2 pos		= m_UIProgressItem.GetWndPos();	
		progress_rect.add	(rect.left + pos.x,rect.top + pos.y);

		UI()->PushScissor	(progress_rect);
		m_UIProgressItem.Draw();
		UI()->PopScissor	();
	}
}