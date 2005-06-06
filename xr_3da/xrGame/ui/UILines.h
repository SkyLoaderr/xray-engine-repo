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
	virtual void			SetTextAlignment(ETextAlignment al)			{m_eTextAlign = al;}
	virtual ETextAlignment	GetTextAlignment()					const	{return m_eTextAlign;}
			void			SetVTextAlignment(EVTextAlignment al)		{m_eVTextAlign = al;}
			EVTextAlignment GetVTextAlignment()					const	{return m_eVTextAlign;}

    // IUISimpleWindow methods
	virtual void			Init(int x, int y, int width, int height);
	virtual void			Draw();
	virtual void			Draw(int x, int y);
	virtual void			Update();


    // CDeviceResetNotifier methods
	virtual void			OnDeviceReset();

	// own methods
			void			Reset();
			void			ParseText();
			int				GetVisibleHeight()					const;
			void			SetLinesInterval(float f)					{ m_interval = f; }
			float			GetLinesInterval()					const	{ return m_interval; }


protected:
				// %c<255,255,255,255>
		u32		GetColorFromText(const xr_string& str)									const;
		u32		GetIndentByAlign(u32 length)											const;
		u32		GetVIndentByAlign()														const;
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

private:
	CGameFont*		m_pFont;

 //   // IUISimpleWindow
	//bool			m_bShowMe;
	//Ivector2		m_wndPos;
	//Ivector2		m_wndSize;
public:

};
