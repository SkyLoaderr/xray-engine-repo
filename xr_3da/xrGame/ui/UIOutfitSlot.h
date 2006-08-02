#pragma once
#include "UIDragDropListEx.h"

class CUIOutfitDragDropList :public CUIDragDropListEx
{
	typedef CUIDragDropListEx						inherited;
	CUIStatic*										m_background;
	shared_str										m_default_outfit;
	void					SetOutfit				(CUICellItem* itm);
	
public:
							CUIOutfitDragDropList	();
	virtual					~CUIOutfitDragDropList	();

	virtual void			SetItem					(CUICellItem* itm); //auto
	virtual void			SetItem					(CUICellItem* itm, Fvector2 abs_pos);  // start at cursor pos
	virtual void			SetItem					(CUICellItem* itm, Ivector2 cell_pos); // start at cell
	virtual CUICellItem*	RemoveItem				(CUICellItem* itm, bool force_root);
	virtual	void			Draw					();
			void			SetDefaultOutfit		(LPCSTR default_outfit);
};

/*
class CUIOutfitSlot: public CUIDragDropList
{
private:
	typedef CUIDragDropList inherited;
public:
				CUIOutfitSlot		();
	virtual		~CUIOutfitSlot		();

	virtual void Init				(float x, float y, float width, float height);
	virtual bool OnMouse			(float x, float y, EUIMessages mouse_action);

	virtual void AttachChild		(CUIWindow *pChild);
	virtual void DetachChild		(CUIWindow *pChild);

	virtual void DropAll			();

	// Установка изображения персонажа без спецкостюма
	void SetOriginalOutfit			();

	// Возвращение указателя на текущий костюм. NULL если нет.
	CUIDragDropItem * GetCurrentOutfit();

	// Для мультиплеера отдельная процедрура назначения иконки персвонажа
	void	SetMPOutfit				();

protected:
	//иконка с изображение сталкера в полный рост
	CUIStatic						UIOutfitIcon;

	float							m_iNoOutfitX;
	float							m_iNoOutfitY;
};
*/