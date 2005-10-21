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

class CUIPdaListItem : public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
					CUIPdaListItem		();
	virtual			~CUIPdaListItem		();
	virtual void	Init				(float x, float y, float width, float height);
	virtual void	InitCharacter		(CInventoryOwner* pInvOwner);
	
	void*					m_data;
protected:
	//информация о персонаже
	CUIFrameWindow*			UIMask;
	CUIIconedListItem*		UIName;
	CUICharacterInfo*		UIInfo;
};