// UISleepWnd.h:  ������ ��� ������ ����, ������� �����
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
	virtual void SetText(LPCSTR str);

//	typedef enum{CLOSE_BUTTON_CLICKED,
//				 PERFORM_BUTTON_CLICKED} E_MESSAGE;
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	// �������� ������� ������������� ����� ������ �� ������-��������
	void ModifyRestTime(s8 dHours, s8 dMinutes);
	// ���������� ����� � 0
	void ResetTime() { SetRestTime(0, 0); }
protected:
	// ������������� �� ����������� ����� ��� ���
	void SetRestTime(u8 hours, u8 minutes);
	// ������� ���������� ����� ������
	s8 m_Hours, m_Minutes;
	// ����� ������������ ���� � �����
	s8 m_MorningH, m_EveningH, m_MorningM, m_EveningM;
	// ������� �����
	u8 m_CurrMins, m_CurrHours;

	// ��������
	CUIStatic UIStaticRestAmount;
	CUIButton UIPlusBtn, UIMinusBtn;
	CUIButton UIRestBtn;
};