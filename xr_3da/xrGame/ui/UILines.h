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
	 friend class CUIEditBox;
public:
	CUILines();
	virtual ~CUILines();

	// IUITextControl methods
	virtual void			SetText(const char* text);
	virtual const char*		GetText();
			void			AddChar(const char ch);
			void			AddChar(const char ch, int pos);
			void			DelChar(int i);
	virtual void			SetTextColor(u32 color)						{m_dwTextColor = color; uFlags.set(flNeedReparse, true);}
	virtual u32				GetTextColor()								{return m_dwTextColor;}
	u32&					GetTextColorRef()							{return m_dwTextColor;}
	virtual void			SetFont(CGameFont* pFont)					{m_pFont = pFont; uFlags.set(flNeedReparse, true);}
	virtual CGameFont*		GetFont()									{return m_pFont;}
	virtual void			SetTextAlignment(ETextAlignment al)			{m_eTextAlign = al;}
	virtual ETextAlignment	GetTextAlignment()							{return m_eTextAlign;}
			void			SetVTextAlignment(EVTextAlignment al)		{m_eVTextAlign = al;}
			EVTextAlignment GetVTextAlignment()							{return m_eVTextAlign;}

	// additional methos
			void			SetTextComplexMode(bool mode = true);
			bool			GetTextComplexMode();
			void			SetPasswordMode(bool mode = true);

    // IUISimpleWindow methods
	virtual void			Init(float x, float y, float width, float height);
	virtual void			Draw();
	virtual void			Draw(float x, float y);
	virtual void			Update();
IC			void			SetWndSize_inline(const Fvector2& wnd_size);


    // CDeviceResetNotifier methods
	virtual void			OnDeviceReset();

	// own methods
			void			Reset();
			void			ParseText();
			float				GetVisibleHeight()					const;
			void			SetLinesInterval(float f)					{ m_interval = f; }
			float			GetLinesInterval()					const	{ return m_interval; }


protected:
				// %c<255,255,255,255>
		u32		GetColorFromText(const xr_string& str)									const;
		float	GetIndentByAlign(float length)											const;
		float	GetVIndentByAlign()														const;
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
		flNeedReparse = (1<<0),
		flComplexMode = (1<<1),
		flPasswordMode = (1<<2)
	};
	Flags32			uFlags;
private:
	float			m_oldWidth;

};
