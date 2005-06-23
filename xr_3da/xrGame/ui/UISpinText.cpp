// file:		UISpinNum.cpp
// description:	Spin Button with text data (unlike numerical data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#include "StdAfx.h"
#include "UISpinText.h"
#include "UILines.h"

CUISpinText::CUISpinText(){

}

CUISpinText::~CUISpinText(){

}

void CUISpinText::AddItem(const char* item){
	m_list.push_back(item);
}

void CUISpinText::SetItem(){
	m_pLines->SetText(m_list[m_curItem].c_str());
}

void CUISpinText::SetCurrentValue(){
	xr_token* tok = GetOptToken();

	while (tok->name){
		AddItem(tok->name);
		tok++;
	}
	xr_string val = GetOptTokenValue();

	for (u32 i = 0; i < m_list.size(); i++)
		if (val == m_list[i])
		{
			m_curItem = i;
			break;
		}

	SetItem();
}

void CUISpinText::SaveValue(){
	SaveOptTokenValue(m_pLines->GetText());
}

void CUISpinText::OnBtnUpClick(){
	if (m_curItem < m_list.size() - 1)
	{
		m_curItem ++;
		SetItem();
	}
}

void CUISpinText::OnBtnDownClick(){
	if (m_curItem > 0)
	{
		m_curItem--;
		SetItem();
	}
}