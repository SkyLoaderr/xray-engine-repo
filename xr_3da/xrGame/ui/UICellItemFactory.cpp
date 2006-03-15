#include "stdafx.h"
#include "UICellItemFactory.h"
#include "UICellItem.h"
#include "../inventory_item.h"

#define INV_GRID_WIDTHF			50.0f
#define INV_GRID_HEIGHTF		50.0f

class CUIInventoryCellItem :public CUICellItem
{
	typedef  CUICellItem	inherited;
	CInventoryItem*			m_object;
public:
								CUIInventoryCellItem		(CInventoryItem* itm);
	virtual		CUIDragItem*	CreateDragItem				();
	virtual		bool			EqualTo						(CUICellItem* itm);
};

CUIInventoryCellItem::CUIInventoryCellItem(CInventoryItem* itm)
:m_object(itm)
{
	inherited::InitTexture							("ui\\ui_icon_equipment");

	m_grid_size.set									(itm->GetGridWidth(),itm->GetGridHeight());
	Frect rect; 
	rect.lt.set										(	INV_GRID_WIDTHF*itm->GetXPos(), 
														INV_GRID_HEIGHTF*itm->GetYPos() );

	rect.rb.set										(	rect.lt.x+INV_GRID_WIDTHF*m_grid_size.x, 
														rect.lt.y+INV_GRID_HEIGHTF*m_grid_size.y);

	inherited::SetOriginalRect						(rect);
	inherited::SetStretchTexture					(true);
}

CUIDragItem* CUIInventoryCellItem::CreateDragItem()
{
	return inherited::CreateDragItem();
}
#include "../physicsShellHolder.h"
bool CUIInventoryCellItem::EqualTo(CUICellItem* itm)
{
	CUIInventoryCellItem* ci = smart_cast<CUIInventoryCellItem*>(itm);
	if(!itm)				return false;
	return					m_object->object().cNameSect() == ci->m_object->object().cNameSect();
}



//------------------------------------------------------
CUICellItem*	create_cell_item(CInventoryItem* itm)
{
	return xr_new<CUIInventoryCellItem>(itm);
}
