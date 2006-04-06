// file:		UISpinNum.cpp
// description:	Spin Button with numerical data (unlike text data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#include "StdAfx.h"
#include "UISpinNum.h"
#include "UILines.h"

CUISpinNum::CUISpinNum()
:	m_iVal(0),
	m_iMin(0),
	m_iMax(100),
	m_iStep(1)
{

}

CUISpinNum::~CUISpinNum(){

}

void CUISpinNum::SetCurrentValue(){
	GetOptIntegerValue(m_iVal, m_iMin, m_iMax);
	SetValue();
}

void CUISpinNum::SaveValue(){
	CUIOptionsItem::SaveValue();
	SaveOptIntegerValue(m_iVal);
}

bool CUISpinNum::IsChanged(){
	int val, min, max;
	GetOptIntegerValue(val, min, max);
    return m_iVal != val;
}

void CUISpinNum::Init(float x, float y, float width, float height){
	CUICustomSpin::Init(x,y,width,height);
	SetValue();
}

void CUISpinNum::OnBtnUpClick(){	
	if (CanPressUp())
		m_iVal += m_iStep;

	SetValue();

	CUICustomSpin::OnBtnUpClick();
}

void CUISpinNum::OnBtnDownClick(){	
	if (CanPressDown())
		m_iVal -= m_iStep;

	SetValue();

	CUICustomSpin::OnBtnDownClick();
}

void CUISpinNum::SetValue(){
	char	buff[8];
	m_pLines->SetText(itoa(m_iVal, buff, 10)); 
}

bool CUISpinNum::CanPressUp(){
	return m_iVal + m_iStep <= m_iMax;

}
bool CUISpinNum::CanPressDown(){
	return m_iVal - m_iStep >= m_iMin;
}
