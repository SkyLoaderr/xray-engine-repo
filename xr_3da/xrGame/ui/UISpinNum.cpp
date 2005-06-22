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
	SaveOptIntegerValue(m_iVal);
}

void CUISpinNum::Init(float x, float y, float width, float height){
	CUICustomSpin::Init(x,y,width,height);
	SetValue();
}

void CUISpinNum::OnBtnUpClick(){
	if (m_iVal + m_iStep <= m_iMax)
		m_iVal += m_iStep;

	SetValue();
}

void CUISpinNum::OnBtnDownClick(){
	if (m_iVal - m_iStep >= m_iMin)
		m_iVal -= m_iStep;

	SetValue();
}

void CUISpinNum::SetValue(){
	char	buff[8];
	m_pLines->SetText(itoa(m_iVal, buff, 10)); 
}


