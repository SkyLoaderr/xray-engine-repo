// UISleepWnd.h:  окошко для выбора того, сколько спать
//////////////////////////////////////////////////////////////////////

#pragma once

#include "uiframewindow.h"
#include "uibutton.h"


class CUISleepWnd: public CUIFrameWindow
{
private:
	typedef CUIFrameWindow inherited;
public:
	CUISleepWnd();
	virtual ~CUISleepWnd();


	virtual void Init(int x, int y, int width, int height);

	typedef enum{CLOSE_BUTTON_CLICKED,
				 PERFORM_BUTTON_CLICKED} E_MESSAGE;
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	void InitSleepWnd();

	void Show();
	void Hide();

	virtual void Update();

protected:
	CUIButton UIPerformButton;
	CUIButton UICloseButton;
};