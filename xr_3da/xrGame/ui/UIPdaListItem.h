//////////////////////////////////////////////////////////////////////
// UIPdaListItem.h: элемент окна списка в PDA
// для отображения информации о контакте PDA
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UIListItem.h"

class CUIPdaListItem : public CUIListItem
{
private:
	typedef CUIListItem inherited;
public:
	CUIPdaListItem(void);
	virtual ~CUIPdaListItem(void);

	virtual void Init(int x, int y, int width, int height);
	
	virtual void Draw();
	virtual void Update();

protected:
	CUIStatic m_UIStaticIcon;
	CUIStatic m_UIStaticText;
};