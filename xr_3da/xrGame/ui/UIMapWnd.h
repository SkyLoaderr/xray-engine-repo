// CUIMapWnd.h:  диалог итерактивной карты
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\inventory.h"

#include "UIPdaContactsWnd.h"
#include "UIPdaDialogWnd.h"

#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"


class CUIMapWnd: public CUIWindow  
{
private:
	typedef CUIWindow inherited;
public:
	CUIMapWnd();
	virtual ~CUIMapWnd();

	virtual void Init();
	virtual void InitMap();

	virtual bool IR_OnKeyboardPress(int dik);
	virtual bool IR_OnKeyboardRelease(int dik);
	virtual bool IR_OnMouseMove(int dx, int dy);

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void Draw();
	virtual void Update();

protected:

	CUIFrameWindow		UIMainMapFrame;
};
