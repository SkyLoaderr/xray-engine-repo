// UIOutfitSlot.h:  слот костюма в диалоге инвентаря
// 
//////////////////////////////////////////////////////////////////////

#include "uidragdroplist.h"

#pragma once

class CUIOutfitSlot: public CUIDragDropList
{
private:
	typedef CUIDragDropList inherited;
public:
	CUIOutfitSlot();
	virtual ~CUIOutfitSlot();

	virtual void Init(int x, int y, int width, int height);

	virtual void AttachChild(CUIDragDropItem* pChild);
	virtual void DetachChild(CUIDragDropItem* pChild);

	virtual void DropAll();

	virtual void Draw();
	virtual void Update();

protected:
	//иконка с изображение сталкера в полный рост
	CUIStatic UIOutfitIcon;

	int m_iNoOutfitX;
	int m_iNoOutfitY;
};