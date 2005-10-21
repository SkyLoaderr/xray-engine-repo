//////////////////////////////////////////////////////////////////////
// UIPdaListItem.h: ������� ���� ������ � PDA
// ��� ����������� ���������� � �������� PDA
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
	//���������� � ���������
	CUIFrameWindow*			UIMask;
	CUIIconedListItem*		UIName;
	CUICharacterInfo*		UIInfo;
};