// UISleepWnd.h:  ������ ��� ������ ����, ������� �����
//////////////////////////////////////////////////////////////////////

#pragma once

#include "uiframewindow.h"
#include "uibutton.h"
#include "UIStatic.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"

class CUISleepWnd: public CUIFrameWindow
{
private:
	typedef CUIFrameWindow inherited;
public:
	CUISleepWnd();
	virtual ~CUISleepWnd();


	virtual void Init(int x, int y, int width, int height);
	virtual void Init();

	typedef enum{CLOSE_BUTTON_CLICKED,
				 PERFORM_BUTTON_CLICKED} E_MESSAGE;
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	void InitSleepWnd();

	void Show();
	void Hide();

	virtual void Update();

	// �������� ������� ������������� ����� ������ �� ������-��������
	void ModifyRestTime(s8 dHours, s8 dMinutes);
	// ���������� ����� � 0
	void ResetTime() { SetRestTime(0, 0); }
protected:
	// ������������� �� ����������� ����� ��� ���
	void SetRestTime(u8 hours, u8 minutes);
	// �������� ������� �����
	void UpdateCurrentTime();
	// ������� ���������� ����� ������
	s8 m_Hours, m_Minutes;

	// ��������
	CUIStatic UIStaticCurrTime;
	CUIStatic UIStaticRestAmount;
	CUIButton UIPlusBtn, UIMinusBtn;
	CUIButton UIIncRestTime1Btn, UIIncRestTime2Btn, UIIncRestTime3Btn;
	CUIButton UIRestBtn, UICloseBtn;
};