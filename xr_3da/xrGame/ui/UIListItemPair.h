// File:        UIListItemPair.h
// Description: This list item designed special for CUIListWndEx to represent
//				two properties of save-file - name and date.
//              ______________________________
//             | file_name       |  date/time |
//             |_________________|____________|
// Created:     10.11.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua

// Copyright:   2004 GSC Game World

#pragma once

#include "StdAfx.h"
#include "UIListItemEx.h"

class CUIListItemPair : public CUIListItemEx {
public:
	// construction/destruction
	CUIListItemPair();
	virtual ~CUIListItemPair();
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);
	virtual void SetHighlightText(bool Highlight); 
	virtual void SetText(LPCSTR file_name, LPCSTR data_time);
	        void SetTextFileName(LPCSTR file_name);
	        void SetTextDateTime(LPCSTR data_time);
	        void SetFontFileName(CGameFont* pFont);
	        void SetFontDateTime(CGameFont* pFont);
			int  GetBorder();
			void SetBorder(int iBorder);

protected:
	virtual void Draw();

	CUIListItem m_staticFileName;
	CUIListItem m_staticDateTime;
	int       m_iBorder;
};