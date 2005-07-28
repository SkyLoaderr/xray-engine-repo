#pragma once

#include "UIFrameLineWnd.h"
#include "UILines.h"
class CLAItem;

class CUILabel : public CUIFrameLineWnd,IUITextControl 
{
	CLAItem*				m_lanim;
	float					m_lainm_start_time;

public:
	//IUISimpleWindow
	virtual void SetWidth(float width);
	virtual void SetHeight(float height);
    // CUIFrameLineWnd
	virtual void			Init(float x, float y, float width, float height);
	virtual void			Draw();
	virtual void			Update			();
	
	// IUIFontControl{
	virtual void			SetTextColor(u32 color)						{m_lines.SetTextColor(color);}
	virtual u32				GetTextColor()								{return m_lines.GetTextColor();}
	virtual void			SetFont(CGameFont* pFont)					{m_lines.SetFont(pFont);}
	virtual CGameFont*		GetFont()									{return m_lines.GetFont();}
	virtual void			SetTextAlignment(ETextAlignment alignment)	{m_lines.SetTextAlignment(alignment);}
	virtual ETextAlignment	GetTextAlignment()							{return m_lines.GetTextAlignment();}

	// IUITextControl : public IUIFontControl{
	virtual void			SetText(const char* text)								{m_lines.SetText(text);}
	virtual const char*		GetText()										{return m_lines.GetText();}

	// own
	CUILabel();

	virtual void			SetTextPosX(float x);
	virtual void			SetTextPosY(float y);
			void			SetLightAnim			(LPCSTR lanim);

protected:
	CUILines m_lines;
	Fvector2 m_textPos;
};