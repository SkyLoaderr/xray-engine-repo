//////////////////////////////////////////////////////////////////////
// UIPdaMsgListItem.h: ������� ���� ������ � �������� 
// ������ ��� ��������� PDA
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

	//���������� � ���������
	CUIStatic UIIcon;
	CUIStatic UIName;
	//����� ���������
	CUIStatic UIMsgText;
protected:
};