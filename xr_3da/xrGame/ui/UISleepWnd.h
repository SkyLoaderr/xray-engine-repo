// UISleepWnd.h:  окошко для выбора того, сколько спать
//////////////////////////////////////////////////////////////////////

#pragma once

#include "uiframewindow.h"
#include "uibutton.h"
#include "UIStatic.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"

class CUISleepWnd: public CUIStatic
{
private:
	typedef CUIStatic inherited;
public:
	CUISleepWnd();
	virtual ~CUISleepWnd();


	virtual void Init(int x, int y, int width, int height);
	virtual void Init();

//	typedef enum{CLOSE_BUTTON_CLICKED,
//				 PERFORM_BUTTON_CLICKED} E_MESSAGE;
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	// Изменяем текущее установленное время отдыха на дельта-значения
	void ModifyRestTime(s8 dHours, s8 dMinutes);
	// Сбрасываем время в 0
	void ResetTime() { SetRestTime(0, 0); }
protected:
	// Устанавливаем на отображение время для сна
	void SetRestTime(u8 hours, u8 minutes);
	// Текущее запоменное время отдыха
	s8 m_Hours, m_Minutes;
	// Время индицирующее утро и вечер
	s8 m_MorningH, m_EveningH, m_MorningM, m_EveningM;
	// Текущее время
	u8 m_CurrMins, m_CurrHours;

	// Контролы
	CUIStatic UIStaticRestAmount;
	CUIButton UIPlusBtn, UIMinusBtn;
	CUIButton UIRestBtn;
};