#pragma once
#include "uilistitem.h"

class CUIListItemEx :
	public CUIListItem
{
private:
	typedef CUIListItem inherited;

public:
	CUIListItemEx(void);
	virtual ~CUIListItemEx(void);
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData);
	virtual void Draw();
	        void SetPerformTextLimit(bool bPerform);

protected:
	bool m_bPerformTextLimit;

};
