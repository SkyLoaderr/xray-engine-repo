// file:		UISpinNum.cpp
// description:	Spin Button with text data (unlike numerical data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#include "StdAfx.h"
#include "UISpinText.h"

CUISpinText::CUISpinText(){

}

CUISpinText::~CUISpinText(){

}

void CUISpinText::AddItem(xr_string& item){
	m_list.push_back(item);
}

void CUISpinText::SetItem(){
    
}