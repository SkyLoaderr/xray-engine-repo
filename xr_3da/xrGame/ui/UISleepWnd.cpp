// UISleepWnd.cpp:  окошко для выбора того, сколько спать
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UISleepWnd.h"
#include "../alife_space.h"
#include "../level.h"

const char * const SLEEP_DIALOG_XML = "sleep_dialog.xml";

CUISleepWnd::CUISleepWnd()
	: m_Hours		(0),
	  m_Minutes		(0),
	  m_MorningH	(0),
	  m_EveningH	(0),
	  m_EveningM	(0),
	  m_MorningM	(0)
{
}

CUISleepWnd::~CUISleepWnd()
{
}

void CUISleepWnd::Init(int x, int y, int width, int height)
{
	inherited::Init("ui\\ui_frame", x, y, width, height);
}

void CUISleepWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", SLEEP_DIALOG_XML);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit	xml_init;

	// Statics
	AttachChild(&UIStaticCurrTime);
	xml_init.InitStatic(uiXml, "curr_time_static", 0, &UIStaticCurrTime);

	AttachChild(&UIStaticRestAmount);
	xml_init.InitStatic(uiXml, "rest_amount_static", 0, &UIStaticRestAmount);

	// Plus, minus time
	AttachChild(&UIPlusBtn);
	xml_init.InitButton(uiXml, "plus_button", 0, &UIPlusBtn);

	AttachChild(&UIMinusBtn);
	xml_init.InitButton(uiXml, "minus_button", 0, &UIMinusBtn);

	// Add fixed amount of rest time
	AttachChild(&UIIncRestTime1Btn);
	xml_init.InitButton(uiXml, "inc_button", 0, &UIIncRestTime1Btn);

	AttachChild(&UIIncRestTime2Btn);
	xml_init.InitButton(uiXml, "inc_button", 1, &UIIncRestTime2Btn);

	AttachChild(&UIIncRestTime3Btn);
	xml_init.InitButton(uiXml, "inc_button", 2, &UIIncRestTime3Btn);

	// Perform sleep
	AttachChild(&UIRestBtn);
	xml_init.InitButton(uiXml, "rest_button", 0, &UIRestBtn);

	// Close dialog
	AttachChild(&UICloseBtn);
	xml_init.InitButton(uiXml, "close_button", 0, &UICloseBtn);

	// Rest until morning
	AttachChild(&UIRestUntilMorningBtn);
	xml_init.InitButton(uiXml, "to_morning_button", 0, &UIRestUntilMorningBtn);
	m_MorningH		= static_cast<s8>(uiXml.ReadAttribInt("to_morning_button", 0, "to_hour"));
	m_MorningM		= static_cast<s8>(uiXml.ReadAttribInt("to_morning_button", 0, "to_min"));

	// Rest until evening
	AttachChild(&UIRestUntilEveningBtn);
	xml_init.InitButton(uiXml, "to_evening_button", 0, &UIRestUntilEveningBtn);
	m_EveningH		= static_cast<s8>(uiXml.ReadAttribInt("to_evening_button", 0, "to_hour"));
	m_EveningM		= static_cast<s8>(uiXml.ReadAttribInt("to_evening_button", 0, "to_min"));

	// Update timerest meter
	ResetTime();
}
void CUISleepWnd::InitSleepWnd()
{
}

void CUISleepWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	const s8 deltaMinutes = 30;

	if(pWnd == &UIRestBtn && msg == CUIButton::BUTTON_CLICKED)
	{
		u32 restMsec = (m_Hours * 3600 + m_Minutes * 60) * 1000;
		GetMessageTarget()->SendMessage(this, PERFORM_BUTTON_CLICKED, reinterpret_cast<void*>(&restMsec));
	}
	else if(pWnd == &UICloseBtn && msg == CUIButton::BUTTON_CLICKED)
	{
		GetMessageTarget()->SendMessage(this, CLOSE_BUTTON_CLICKED);
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
	else if(pWnd == &UIIncRestTime1Btn && msg == CUIButton::BUTTON_CLICKED)
	{
		// Отдохнуть 1 час
		u32 restMsec = 3600 * 1000;
		GetMessageTarget()->SendMessage(this, PERFORM_BUTTON_CLICKED, reinterpret_cast<void*>(&restMsec));
	}
	else if(pWnd == &UIIncRestTime2Btn && msg == CUIButton::BUTTON_CLICKED)
	{
		// Отдохнуть 4 час
		u32 restMsec = 4 * 3600 * 1000;
		GetMessageTarget()->SendMessage(this, PERFORM_BUTTON_CLICKED, reinterpret_cast<void*>(&restMsec));
	}
	else if(pWnd == &UIIncRestTime3Btn && msg == CUIButton::BUTTON_CLICKED)
	{
		// Отдохнуть 8 час
		u32 restMsec = 8 * 3600 * 1000;
		GetMessageTarget()->SendMessage(this, PERFORM_BUTTON_CLICKED, reinterpret_cast<void*>(&restMsec));
	}
	else if(pWnd == &UIRestUntilMorningBtn && msg == CUIButton::BUTTON_CLICKED)
	{
		// Отдохнуть до утра
		u32 deltaH = 0, deltaM = 0;
		if (m_CurrHours > m_MorningH)
		{
			deltaH = 24 - m_CurrHours + m_MorningH;
		}
		else
		{
			deltaH = m_MorningH - m_CurrHours;
		}

		deltaM		= m_CurrMins - m_MorningM;

		u32 restMsec = deltaH * 3600 * 1000 - deltaM * 60 * 1000;

		GetMessageTarget()->SendMessage(this, PERFORM_BUTTON_CLICKED, reinterpret_cast<void*>(&restMsec));
	}
	else if(pWnd == &UIRestUntilEveningBtn && msg == CUIButton::BUTTON_CLICKED)
	{
		// Отдохнуть до утра
		u32 deltaH = 0, deltaM = 0;
		if (m_CurrHours > m_EveningH)
		{
			deltaH = 24 - m_CurrHours + m_EveningH;
		}
		else
		{
			deltaH = m_EveningH - m_CurrHours;
		}

		deltaM		= m_CurrMins - m_EveningM;

		u32 restMsec = deltaH * 3600 * 1000 - deltaM * 60 * 1000;

		GetMessageTarget()->SendMessage(this, PERFORM_BUTTON_CLICKED, reinterpret_cast<void*>(&restMsec));
	}

	inherited::SendMessage(pWnd, msg, pData);
}


void CUISleepWnd::Show()
{
	ResetAll();
	inherited::Show(true);
	inherited::Enable(true);
}
void CUISleepWnd::Hide()
{
	ResetTime();
	inherited::Show(false);
	inherited::Enable(false);
}

void CUISleepWnd::Update()
{
	UpdateCurrentTime();
	inherited::Update();
}

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

void CUISleepWnd::SetRestTime(u8 hours, u8 minutes)
{
	string32	buf;

	m_Minutes	= minutes;
	m_Hours		= hours;

	sprintf(buf, "Rest:%02i:%02i", hours, minutes);
	UIStaticRestAmount.SetText(buf);
}

void CUISleepWnd::UpdateCurrentTime()
{
	string32 buf;
	ALife::_TIME_ID currMsec = Level().GetGameTime();

	m_CurrMins		= static_cast<u8>(currMsec / (1000 * 60) % 60 & 0xFF);
	m_CurrHours		= static_cast<u8>(currMsec / (1000 * 3600) % 24 & 0xFF);
	sprintf(buf, "CUR. TIME:%02i:%02i", m_CurrHours, m_CurrMins);

	UIStaticCurrTime.SetText(buf);
}
