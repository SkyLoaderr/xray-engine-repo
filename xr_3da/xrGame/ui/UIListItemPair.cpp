// File:        UIListItemPair.cpp
// Description: This list item designed special for CUIListWndEx to represent
//				two properties of save file - name and date.
//              ______________________________
//             | file_name       |  date/time |
//             |_________________|____________|
// Created:     10.11.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua

// Copyright:   2004 GSC Game World

#include "StdAfx.h"
#include "UIListItemPair.h"
#include "../HUDManager.h"
#include "../level.h"

//-----------------------
// Construction
//-----------------------
CUIListItemPair::CUIListItemPair(){
	// set textures
	this->InitTexture("ui\\hud_map_point");
	this->SetStretchTexture(true);
	this->SetColor(color_argb(0, 0, 0, 0));

	// attach all children
	this->AttachChild(&m_staticFileName);
	this->AttachChild(&m_staticDateTime);

    m_iBorder = 60;

	// file name field
	m_staticFileName.SetFont(HUD().pFontMedium);
	m_staticFileName.SetTextColor(color_argb(255, 255, 255, 255));
	m_staticFileName.SetTextAlign(CGameFont::alLeft);
	m_staticFileName.Init(0,0,0,0);

	// date/time field
	m_staticDateTime.SetFont(HUD().pFontMedium);
	m_staticDateTime.SetTextColor(color_argb(255, 255, 255, 255));
	m_staticDateTime.SetTextAlign(CGameFont::alLeft);
	m_staticDateTime.Init(0,0,0,0);
}

//-----------------------
// Destruction
//-----------------------
CUIListItemPair::~CUIListItemPair(){

}

//-------------------------------------------------

void CUIListItemPair::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){		
	switch (msg)
	{
	case LIST_ITEM_SELECT:
		this->SetColor(color_argb(255, 0, 0, 255));	
		this->Draw();
		break;
	case LIST_ITEM_UNSELECT:
		this->SetColor(color_argb(0, 0, 0, 0));
		this->Draw();
		break;
	}
}

//-------------------------------------------------

void CUIListItemPair::Draw(){	
	CUIListItemEx::Draw();

	int width  = this->GetWidth();
	int height = this->GetHeight();
	const int indent = 10; // indent between 
	
	// get max length for both elements
	int maxFileNameLength = width * this->m_iBorder/100 - indent;
	int maxDateTimeLength = width - maxFileNameLength;

	// get length of elements
	int realFileNameLength = (int)m_staticFileName.GetFont()->SizeOf(m_staticFileName.GetText());
	int realDateTimeLength = (int)m_staticDateTime.GetFont()->SizeOf(m_staticDateTime.GetText());

	// get height of elements
	int fileNameHeight = (int)m_staticFileName.GetFont()->CurrentHeight();
	int dateTimeHeight = (int)m_staticDateTime.GetFont()->CurrentHeight();

	//m_staticFileName.Init
	m_staticFileName.SetWndRect(0,                          (height - fileNameHeight)/2 , maxFileNameLength, height);
	//m_staticDateTime.Init
	m_staticDateTime.SetWndRect(maxFileNameLength + indent, (height - dateTimeHeight)/2 , maxDateTimeLength, height);

	// delete some characters from end (if it nessesary)
	// to fit text length to static rect
	m_staticFileName.PerformTextLengthLimit();	

	m_staticFileName.Draw();
	m_staticDateTime.Draw();
}

void CUIListItemPair::SetHighlightText(bool Highlight){
	m_staticFileName.SetHighlightText(Highlight); 
	m_staticDateTime.SetHighlightText(Highlight); 
}

//-------------------------------------------------

void CUIListItemPair::SetText(LPCSTR file_name, LPCSTR data_time){
	m_staticFileName.SetText(file_name);	
	m_staticDateTime.SetText(data_time);
	
}

//-------------------------------------------------

void CUIListItemPair::SetTextFileName(LPCSTR file_name){
	m_staticFileName.SetText(file_name);	
}

//-------------------------------------------------

void CUIListItemPair::SetTextDateTime(LPCSTR date_time){
	m_staticDateTime.SetText(date_time);
}

//-------------------------------------------------

void CUIListItemPair::SetFontFileName(CGameFont* pFont){
	m_staticFileName.SetFont(pFont);
}

//-------------------------------------------------

void CUIListItemPair::SetFontDateTime(CGameFont* pFont){
	m_staticDateTime.SetFont(pFont);
}

//-------------------------------------------------

int CUIListItemPair::GetBorder(){	
	return m_iBorder;	
}

//---->>

void CUIListItemPair::SetBorder(int iBorder){
	// border = [1, 100]
	m_iBorder = iBorder%100;
}