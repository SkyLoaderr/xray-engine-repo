//////////////////////////////////////////////////////////////////////
// UIPdaListItem.h: ������� ���� ������ � PDA
// ��� ����������� ���������� � �������� PDA
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
	CUIPdaListItem(void);
	virtual ~CUIPdaListItem(void);

	virtual void	Init(int x, int y, int width, int height);
	virtual void	InitCharacter(CInventoryOwner* pInvOwner);

	virtual void	SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	
	virtual void	Draw();
	virtual void	Update();

protected:
	//���������� � ���������
	CUIStatic				UIIcon;
	CUIFrameWindow			UIMask;
	CUIIconedListItem		UIName;

	// ��� ��������� ������� � �������� � UICharText ������ ���������
	enum 
	{
		eCommunity	= 1,
		eRank		= 3,
		eRelation	= 5
	};

	CUIMultiTextStatic		UICharText;
};