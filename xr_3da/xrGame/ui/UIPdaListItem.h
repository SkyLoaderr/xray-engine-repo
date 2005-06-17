//////////////////////////////////////////////////////////////////////
// UIPdaListItem.h: элемент окна списка в PDA
// для отображения информации о контакте PDA
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UIListItem.h"
//#include "UIMultiTextStatic.h"
//#include "UIIconedListItem.h"

class CUIFrameWindow;
class CUIIconedListItem;
class CUICharacterInfo;
class CInventoryOwner;

class CUIPdaListItem : public CUIListItem
{
private:
	typedef CUIListItem inherited;
public:
					CUIPdaListItem		();
	virtual			~CUIPdaListItem		();
	virtual void	Init				(float x, float y, float width, float height);
	virtual void	InitCharacter		(CInventoryOwner* pInvOwner);
	virtual void	SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);

protected:
	//информация о персонаже
	CUIFrameWindow*			UIMask;
	CUIIconedListItem*		UIName;
	CUICharacterInfo*		UIInfo;
};