//////////////////////////////////////////////////////////////////////
// UIPdaListItem.h: элемент окна списка в PDA
// для отображения информации о контакте PDA
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UIListItem.h"
#include "UICharacterInfo.h"
#include "..\InventoryOwner.h"

class CUIPdaListItem : public CUIListItem
{
private:
	typedef CUIListItem inherited;
public:
	CUIPdaListItem(void);
	virtual ~CUIPdaListItem(void);

	virtual void Init(int x, int y, int width, int height);
	virtual void InitCharacter(CInventoryOwner* pInvOwner);
	
	virtual void Draw();
	virtual void Update();

protected:
	//информация о персонаже
	CUICharacterInfo UICharacterInfo;
};