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
	virtual void OnMouse(int x, int y, EUIMessages mouse_action);

	virtual void AttachChild(CUIWindow *pChild);
	virtual void DetachChild(CUIWindow *pChild);

	virtual void DropAll();
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	virtual void Draw();
	virtual void Update();

	// Установка изображения персонажа без спецкостюма
	void SetOriginalOutfit();

	// Возвращение указателя на текущий костюм. NULL если нет.
	CUIDragDropItem * GetCurrentOutfit();

	// Для мультиплеера отдельная процедрура назначения иконки персвонажа
	void	SetMPOutfit();

//	typedef enum{UNDRESS_OUTFIT = 6000, OUTFIT_RETURNED_BACK} E_OUTFIT_ACTION;

protected:
	//иконка с изображение сталкера в полный рост
	CUIStatic UIOutfitIcon;

	int m_iNoOutfitX;
	int m_iNoOutfitY;
};