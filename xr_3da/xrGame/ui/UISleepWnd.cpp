// UISleepWnd.cpp:  ������ ��� ������ ����, ������� �����
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UISleepWnd.h"

const char * const SLEEP_DIALOG_XML = "sleep_dialog.xml";

CUISleepWnd::CUISleepWnd()
	: m_Hours(0), m_Minutes(0)
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
		// ��������� 1 ���
		u32 restMsec = 3600 * 1000;
		GetMessageTarget()->SendMessage(this, PERFORM_BUTTON_CLICKED, reinterpret_cast<void*>(&restMsec));
	}
	else if(pWnd == &UIIncRestTime2Btn && msg == CUIButton::BUTTON_CLICKED)
	{
		// ��������� 4 ���
		u32 restMsec = 4 * 3600 * 1000;
		GetMessageTarget()->SendMessage(this, PERFORM_BUTTON_CLICKED, reinterpret_cast<void*>(&restMsec));
	}
	else if(pWnd == &UIIncRestTime3Btn && msg == CUIButton::BUTTON_CLICKED)
	{
		// ��������� 8 ���
		u32 restMsec = 8 * 3600 * 1000;
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

	// �������� �� ������� ��������
	R_ASSERT(dMinutes < 61);

	s8 oldMin = m_Minutes;

	// ����������� ������
	m_Minutes = m_Minutes + dMinutes;

	// ��������� ������ �� ������� �������
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

	// ���� ����� � ��� �����
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

	u8 mins		= static_cast<u8>(currMsec / (1000 * 60) % 60 & 0xFF);
	u8 hours	= static_cast<u8>(currMsec / (1000 * 3600) % 24 & 0xFF);
	sprintf(buf, "CUR. TIME:%02i:%02i", hours, mins);

	UIStaticCurrTime.SetText(buf);
}
