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
				 public IUISimpleWindow,
				 public CDeviceResetNotifier {
public:
	CUILines();
	virtual ~CUILines();

	// IUITextControl methods
	virtual void			SetText(const char* text);
	virtual const char*		GetText()							const;
	virtual void			SetTextColor(u32 color)						{m_dwTextColor = color;}
	virtual u32				GetTextColor()						const	{return m_dwTextColor;}
	virtual void			SetFont(CGameFont* pFont)					{m_pFont = pFont;}
	virtual CGameFont*		GetFont()							const	{return m_pFont;}
	virtual void			SetTextAlignment(ETextAlignment alignment)	{m_eTextAlign = alignment;}
	virtual ETextAlignment	GetTextAlignment()					const	{return m_eTextAlign;}

    // IUISimpleWindow methods
	virtual void			Draw()								const;
	virtual void			Update();
	virtual void			SetWndPos(const Ivector2& pos)				{m_wndPos = pos;}
	virtual Ivector2		GetWndPos()							const	{return m_wndPos;}
	virtual void			SetWndSize(const Ivector2& size)			{m_wndSize = size;}
	virtual Ivector2		GetWndSize()						const	{return m_wndSize;}
	virtual void			SetWndRect(const Irect& rect);
	virtual Irect			GetWndRect()						const;
	virtual void			SetHeight(int height)						{m_wndSize.y = height;}
	virtual int				GetHeight()							const	{return m_wndSize.y;}
	virtual void			SetWidth(int width)							{m_wndSize.x = width;}
	virtual int				GetWidth()							const	{return m_wndSize.y;}
	virtual void			Show(bool bShow)							{m_bShowMe = bShow;}
	virtual bool			IsShown()							const	{return m_bShowMe;}

    // CDeviceResetNotifier methods
	virtual void			OnDeviceReset();

	// own methods
			void			Reset();
			void			ParseText();
			int				GetVisibleHeight()					const;
			void			SetLinesInterval(int i)						{ m_interval = i; }
			int				GetLinesInterval()					const	{ return m_interval; }


protected:
				// %c<255,255,255,255>
		u32		GetColorFromText(const xr_string& str) const;
		u32		GetIndentByAlign(u32 length) const;
		void	CutFirstColoredTextEntry(xr_string& entry, u32& color,xr_string& text) const;
	CUILine*	ParseTextToColoredLine(const xr_string& str) const;

	// IUITextControl data
	typedef xr_string						Text;
	typedef xr_vector<CUILine>				LinesVector;
	typedef xr_vector<CUILine>::iterator	LinesVector_it;
	LinesVector		m_lines;	// parsed text
	u32				m_interval; // interval

	xr_string		m_text;

	ETextAlignment	m_eTextAlign;
	u32				m_dwTextColor;
	CGameFont*		m_pFont;

    // IUISimpleWindow
	bool			m_bShowMe;
	Ivector2		m_wndPos;
	Ivector2		m_wndSize;
public:

};
