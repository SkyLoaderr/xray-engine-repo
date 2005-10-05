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
						CUIPdaMsgListItem				(void);
						CUIPdaMsgListItem				(int iDelay);
	virtual				~CUIPdaMsgListItem				(void);

	virtual void		Init							(float x, float y, float width, float height);
	virtual void		InitCharacter					(CInventoryOwner* pInvOwner);
	virtual bool		IsTimeToDestroy					();
	virtual void		SetDelay						(int iDelay);
	virtual void		SetTextColor					(u32 color);
	virtual void		SetFont							(CGameFont* pFont);
	
	virtual void		Draw							();
	virtual void		Update							();

	//информация о персонаже
	CUIStatic			UIIcon;
	CUIStatic			UIName;
	CUIStatic			UIMsgText;
protected:
	int					m_iDelay;
	ALife::_TIME_ID		m_timeBegin;	
};