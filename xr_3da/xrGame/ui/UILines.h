// File:		UILines.h
// Description:	Multilines Text Control
// Created:		11.03.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#pragma once

#include "UILine.h"
#include "../UI.h"
#include "uiabstract.h"

class CUILines : public IUITextControl,
				 public CUISimpleWindow,
				 public CDeviceResetNotifier {
	 friend class CUICustomEdit;
	 friend class CUICDkey;
public:
	CUILines();
	virtual ~CUILines();

	// IUITextControl methods
	virtual void			SetText(const char* text);
	virtual const char*		GetText();
	virtual void			SetTextColor(u32 color);
	virtual u32				GetTextColor()								{return m_dwTextColor;}
			u32&			GetTextColorRef()							{return m_dwTextColor;}
	virtual void			SetFont(CGameFont* pFont);
	virtual CGameFont*		GetFont()									{return m_pFont;}
	virtual void			SetTextAlignment(ETextAlignment al)			{m_eTextAlign = al;}
	virtual ETextAlignment	GetTextAlignment()							{return m_eTextAlign;}
			void			SetVTextAlignment(EVTextAlignment al)		{m_eVTextAlign = al;}
			EVTextAlignment GetVTextAlignment()							{return m_eVTextAlign;}

	// additional
			void			AddCharAtCursor(const char ch);
			void			DelChar();
			void			DelLeftChar();
			void			MoveCursorToEnd();

			void			SetTextComplexMode(bool mode = true);
			bool			GetTextComplexMode();
			void			SetPasswordMode(bool mode = true);

    // IUISimpleWindow methods
	virtual void			Init(float x, float y, float width, float height);
	virtual void			Draw();
			void			DrawCursor(float x, float y);
	virtual void			Draw(float x, float y);
	virtual void			Update();
IC			void			SetWndSize_inline(const Fvector2& wnd_size);


    // CDeviceResetNotifier methods
	virtual void			OnDeviceReset();

	// own methods
			void			Reset();
			void			ParseText();
			float			GetVisibleHeight();

	// cursor control
			Ivector2		m_cursor_pos;
			int				m_iCursorPos;
			void			IncCursorPos();
			void			DecCursorPos();
protected:
			void			UpdateCursor();
IC		int		GetAbsCursorPos();
		float	GetDrawCursorPos();
				// %c<255,255,255,255>
		u32		GetColorFromText(const xr_string& str)									const;
		float	GetIndentByAlign(float length)											const;
		float	GetVIndentByAlign();
		void	CutFirstColoredTextEntry(xr_string& entry, u32& color,xr_string& text)	const;
	CUILine*	ParseTextToColoredLine(const xr_string& str)							const;

	// IUITextControl data
	typedef xr_string						Text;
	typedef xr_vector<CUILine>				LinesVector;
	typedef xr_vector<CUILine>::iterator	LinesVector_it;
	LinesVector		m_lines;	// parsed text
	float			m_interval; // interval

	xr_string		m_text;

	ETextAlignment	m_eTextAlign;
	EVTextAlignment	m_eVTextAlign;
	u32				m_dwTextColor;

	CGameFont*		m_pFont;

	enum {
		flNeedReparse = 8,
		flComplexMode = 2,
		flPasswordMode = 4
	};	
private:
	Flags32			uFlags;
	float			m_oldWidth;
};

class CUILinesOwner : public IUITextControl {
public:
	virtual ~CUILinesOwner() {}

	// IUIFontControl{
	virtual void			SetTextColor(u32 color)						{m_lines.SetTextColor(color);}
	virtual u32				GetTextColor()								{return m_lines.GetTextColor();}
	virtual void			SetFont(CGameFont* pFont)					{m_lines.SetFont(pFont);}
	virtual CGameFont*		GetFont()									{return m_lines.GetFont();}
	virtual void			SetTextAlignment(ETextAlignment alignment)	{m_lines.SetTextAlignment(alignment);}
	virtual ETextAlignment	GetTextAlignment()							{return m_lines.GetTextAlignment();}

	// IUITextControl : public IUIFontControl{
	virtual void			SetText(const char* text)					{m_lines.SetText(text);}
	virtual const char*		GetText()									{return m_lines.GetText();}

	// own
	virtual void			SetTextPosX(float x)						{m_textPos.x = x;}
	virtual void			SetTextPosY(float y)						{m_textPos.y = y;}

protected:
	Fvector2	m_textPos;
	CUILines	m_lines;
};
