//////////////////////////////////////////////////////////////////////
// UIPdaMsgListItem.h: элемент окна списка в основном 
// экране для сообщений PDA
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UIListItem.h"
#include "..\InventoryOwner.h"

class CUIPdaMsgListItem : public CUIListItem
{
private:
	typedef CUIListItem inherited;
public:
	CUIPdaMsgListItem(void);
	virtual ~CUIPdaMsgListItem(void);

	virtual void Init(int x, int y, int width, int height);
	virtual void InitCharacter(CInventoryOwner* pInvOwner);
	
	virtual void Draw();
	virtual void Update();

	//информация о персонаже
	CUIStatic UIIcon;
	CUIStatic UIName;
	//текст сообщения
	CUIStatic UIMsgText;
protected:
};