//////////////////////////////////////////////////////////////////////
// UIPdaListItem.h: элемент окна списка в PDA
// для отображения информации о контакте PDA
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UIListItem.h"
#include "UICharacterInfo.h"
#include "..\InventoryOwner.h"
#include "UIMultiTextStatic.h"
#include "UIIconedListItem.h"

class CUIPdaListItem : public CUIListItem
{
private:
	typedef CUIListItem inherited;
public:
					CUIPdaListItem		();
	virtual			~CUIPdaListItem		();
	virtual void	Init				(int x, int y, int width, int height);
	virtual void	InitCharacter		(CInventoryOwner* pInvOwner);
	virtual void	SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);

protected:
	//информация о персонаже
	CUIFrameWindow			UIMask;
	CUIIconedListItem		UIName;
	CUICharacterInfo		UIInfo;
};