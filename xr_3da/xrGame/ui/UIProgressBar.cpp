//////////////////////////////////////////////////////////////////////
// UIProgressBar.h: ������ �������� ����������
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uiprogressbar.h"
#include "..\MainUI.h"

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

void CUIProgressBar::Init(int x, int y, int length, int broad, bool bIsHorizontal)
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
											int progress_length, 
											int x, int y,
											int width, int height,
											u32 color)
{
	m_iProgressLength			= progress_length;
	
	m_UIProgressItem.Init		(tex_name, "hud\\default", x, y, alNone);
	m_UIProgressItem.SetColor	(color);

	if (width!=0 && height!=0)
		m_UIProgressItem.SetOriginalRect(x, y, width, height);

	if(m_bIsHorizontal){
		m_UIProgressItem.SetRect(0, 0, m_iProgressLength, GetHeight());	
	}else{
		m_UIProgressItem.SetRect(0, 0, GetWidth(), m_iProgressLength);
	}
}
void CUIProgressBar::SetBackgroundTexture(LPCSTR tex_name, int x, int y, int width, int height, int offs_x, int offs_y)
{
	if (tex_name&&tex_name[0]){
		m_bBackgroundPresent = true;

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
	//������������� ������ ���������� ������������ ��������
	float progressbar_unit;
	progressbar_unit = (float)1/(m_iMaxPos-m_iMinPos);

	float fCurrentLength = m_iProgressPos*progressbar_unit;

	//��������� ������ � ��������� �������
	if(m_bIsHorizontal)	m_iCurrentLength = (int)(GetWidth()*fCurrentLength); 	
	else				m_iCurrentLength = (int)(GetHeight()*fCurrentLength); 	

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
	Irect rect = GetAbsoluteRect();

	//���������� ��������
	if(m_bBackgroundPresent){
		m_UIBackgroundItem.SetPos(rect.left+m_BackgroundOffset.x, rect.top+m_BackgroundOffset.y);
		m_UIBackgroundItem.Render();
	}

	Irect progress_rect;
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