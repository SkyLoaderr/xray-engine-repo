#include "uilistitem.h"

class CUIListItemEx :
	public CUIListItem
{
private:
	typedef CUIButton inherited;

public:
	CUIListItemEx(void);
	virtual ~CUIListItemEx(void);
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData);

};
