// UISleepWnd.cpp:  окошко для выбора того, сколько спать
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UISleepWnd.h"
#include "../alife_space.h"
#include "../level.h"

//////////////////////////////////////////////////////////////////////////

const char * const SLEEP_DIALOG_XML = "sleep_dialog_new.xml";

//////////////////////////////////////////////////////////////////////////

CUISleepWnd::CUISleepWnd()
	: m_Hours		(0),
	  m_Minutes		(0),
	  m_MorningH	(0),
	  m_EveningH	(0),
	  m_EveningM	(0),
	  m_MorningM	(0)
{
}

//////////////////////////////////////////////////////////////////////////

CUISleepWnd::~CUISleepWnd()
{
}

//////////////////////////////////////////////////////////////////////////

void CUISleepWnd::Init(int x, int y, int width, int height)
{
	inherited::Init(x, y, width, height);
}

//////////////////////////////////////////////////////////////////////////

void CUISleepWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", SLEEP_DIALOG_XML);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit	xml_init;

	// Statics
	AttachChild(&UIStaticRestAmount);
	xml_init.InitStatic(uiXml, "rest_amount_static", 0, &UIStaticRestAmount);

	// Plus, minus time
	AttachChild(&UIPlusBtn);
	xml_init.InitButton(uiXml, "plus_button", 0, &UIPlusBtn);

	AttachChild(&UIMinusBtn);
	xml_init.InitButton(uiXml, "minus_button", 0, &UIMinusBtn);

	// Perform sleep
	AttachChild(&UIRestBtn);
	xml_init.InitButton(uiXml, "rest_button", 0, &UIRestBtn);

	// Update timerest meter
	ResetTime();
}

//////////////////////////////////////////////////////////////////////////

void CUISleepWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	const s8 deltaMinutes = 30;

	if(pWnd == &UIRestBtn && msg == CUIButton::BUTTON_CLICKED)
	{
		u32 restMsec = (m_Hours * 3600 + m_Minutes * 60) * 1000;
		if (restMsec != 0)
			GetMessageTarget()->SendMessage(this, PERFORM_BUTTON_CLICKED, reinterpret_cast<void*>(&restMsec));

	}
	else if(pWnd == &UIPlusBtn && msg == CUIButton::BUTTON_CLICKED)
	{
		// Add fixed amount of minutes and hours
		ModifyRestTime(0, deltaMinutes);
	}
	else if(pWnd == &UIMinusBtn && msg == CUIButton::BUTTON_CLICKED)
	{
		// Add fixed amount of minutes and hours
		ModifyRestTime(0, -deltaMinutes);
	}
	else if ((&UIPlusBtn == pWnd || &UIMinusBtn == pWnd || &UIRestBtn == pWnd) && CUIButton::BUTTON_DOWN == msg)
	{
		CUIButton *pBtn = dynamic_cast<CUIButton*>(pWnd);
		R_ASSERT(pBtn);

		pBtn->EnableTextHighlighting(false);
	}
	
	if ((&UIPlusBtn == pWnd || &UIMinusBtn == pWnd || &UIRestBtn == pWnd) && CUIButton::BUTTON_CLICKED == msg)
	{
		CUIButton *pBtn = dynamic_cast<CUIButton*>(pWnd);
		R_ASSERT(pBtn);

		pBtn->EnableTextHighlighting(true);
	}

	inherited::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////

void CUISleepWnd::ModifyRestTime(s8 dHours, s8 dMinutes)
{

	// Проверка на влидный интервал
	R_ASSERT(dMinutes < 61);

	s8 oldMin = m_Minutes;

	// Увеличиваем минуты
	m_Minutes = m_Minutes + dMinutes;

	// Проверяем выходы за пределы допуска
	if (m_Minutes > 59)
	{
		++m_Hours;
		m_Minutes = m_Minutes - 60;
	}
	else if (m_Minutes < 0)
	{
		--m_Hours;
		m_Minutes = m_Minutes + 60;
	}

	// Тоже самое и для часов
	m_Hours = m_Hours + dHours;
	if (m_Hours > 99)
	{
		m_Hours = 99;
		m_Minutes = oldMin;
	}
	else if (m_Hours < 0)
	{
		m_Minutes = oldMin;
		m_Hours = 0;
	}

	SetRestTime(m_Hours, m_Minutes);
}

//////////////////////////////////////////////////////////////////////////

void CUISleepWnd::SetRestTime(u8 hours, u8 minutes)
{
	string32	buf;

	m_Minutes	= minutes;
	m_Hours		= hours;

	sprintf(buf, "%02i:%02i", hours, minutes);
	UIStaticRestAmount.SetText(buf);
}