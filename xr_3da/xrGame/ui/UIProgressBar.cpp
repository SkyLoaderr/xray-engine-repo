//////////////////////////////////////////////////////////////////////
// UIProgressBar.h: полоса процента выполнения
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uiprogressbar.h"
#include "UITextureMaster.h"

CUIProgressBar::CUIProgressBar(void)
{
	m_iMinPos				= 1;
	m_iMaxPos				= 1;

	Enable					(false);

	m_bBackgroundPresent	= false;
	m_bUseColor				= false;
}

CUIProgressBar::~CUIProgressBar(void)
{
}

void CUIProgressBar::Init(float x, float y, float length, float broad, bool bIsHorizontal)
{
	m_bIsHorizontal = bIsHorizontal;

	if(m_bIsHorizontal){
		CUIWindow::Init(x,y, length, broad);
	}else{
		CUIWindow::Init(x,y, broad, length);
	}

	UpdateProgressBar();
}

void CUIProgressBar::SetProgressTexture(LPCSTR tex_name, 
											float progress_length, 
											float x, float y,
											float width, float height,
											u32 color)
{
	m_iProgressLength			= progress_length;

	CUITextureMaster::InitTexture(tex_name, "hud\\default", &m_UIProgressItem);
	m_UIProgressItem.SetPos(x,y);
	
//	m_UIProgressItem.Init		(tex_name, "hud\\default", x, y, alNone);
	m_UIProgressItem.SetColor	(color);

	if (!fis_zero(width) && !fis_zero(height))
		m_UIProgressItem.SetOriginalRect(x, y, width, height);

	if(m_bIsHorizontal){
		m_UIProgressItem.SetRect(0, 0, m_iProgressLength, GetHeight());	
	}else{
		m_UIProgressItem.SetRect(0, 0, GetWidth(), m_iProgressLength);
	}
}
void CUIProgressBar::SetBackgroundTexture(LPCSTR tex_name, float x, float y, float width, float height, float offs_x, float offs_y)
{
	if (tex_name&&tex_name[0]){
		m_bBackgroundPresent = true;

//		CUITextureMaster::InitTexture(tex_name, "hud\\default", &m_UIBackgroundItem);

		m_UIBackgroundItem.Init		(tex_name, "hud\\default", 0, 0, alNone);
		m_BackgroundOffset.set		(offs_x,offs_y);

		if (width!=0 && height!=0)
			m_UIBackgroundItem.SetOriginalRect(x, y, width, height);

		if(m_bIsHorizontal){
			m_UIBackgroundItem.SetRect(0, 0, m_iProgressLength, GetHeight());	
		}else{
			m_UIBackgroundItem.SetRect(0, 0, GetWidth(), m_iProgressLength);
		}
	}
}

void CUIProgressBar::UpdateProgressBar()
{
	//относительный размер единичного прокручемого элемента
	float progressbar_unit;
	progressbar_unit = (float)1/(m_iMaxPos-m_iMinPos);

	float fCurrentLength = m_iProgressPos*progressbar_unit;

	//утановить размер и положение каретки
	if(m_bIsHorizontal)	m_iCurrentLength = GetWidth()*fCurrentLength; 	
	else				m_iCurrentLength = GetHeight()*fCurrentLength; 	

	if(m_bUseColor){
		Fcolor curr;
		curr.lerp							(m_minColor,m_maxColor,fCurrentLength);
		m_UIProgressItem.SetColor			(curr);
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

	//нарисовать подложку
	if(m_bBackgroundPresent){
		m_UIBackgroundItem.SetPos(rect.left+m_BackgroundOffset.x, rect.top+m_BackgroundOffset.y);
		m_UIBackgroundItem.Render();
	}

	Frect progress_rect;
	m_UIProgressItem.SetPos	(rect.left, rect.top);

	if(m_bIsHorizontal){
		progress_rect.set	(0, 0, m_iCurrentLength, GetHeight());
	}else{
		progress_rect.set	(0, m_iProgressLength-m_iCurrentLength,
							GetWidth(), m_iProgressLength);
	}
	
	if(m_iCurrentLength>0){
		progress_rect.add	(rect.left,rect.top);
		UI()->PushScissor	(progress_rect);
		m_UIProgressItem.Render();
		UI()->PopScissor	();
	}
}