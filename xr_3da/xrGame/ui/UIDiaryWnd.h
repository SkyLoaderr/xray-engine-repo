// CUIDiaryWnd.h:  ������� � ��� ��� � ��� �������
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"


///////////////////////////////////////
// �������
///////////////////////////////////////

class CUIDiaryWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUIDiaryWnd();
	virtual ~CUIDiaryWnd();

	virtual void Init();
	virtual void InitDiary();

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void Draw();
	virtual void Update();
	
	virtual void Show();

protected:
	CUIFrameWindow UIMainDiaryFrame;
};
