// UIOutfitSlot.cpp:  слот костюма в диалоге инвентаря
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIOutfitSlot.h"

#include "..\\CustomOutfit.h"


#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;



CUIOutfitSlot::CUIOutfitSlot()
{
}
CUIOutfitSlot::~CUIOutfitSlot()
{
}

void CUIOutfitSlot::Init(int x, int y, int width, int height)
{
	inherited::Init(x, y, width, height);
	
	inherited::AttachChild(&UIOutfitIcon);
	UIOutfitIcon.Init(0,0,width,height);

	m_iNoOutfitX = pSettings->r_u32("without_outfit", "full_scale_icon_x");
	m_iNoOutfitY = pSettings->r_u32("without_outfit", "full_scale_icon_y");

	UIOutfitIcon.SetShader(GetCharIconsShader());
	UIOutfitIcon.GetUIStaticItem().SetOriginalRect(
					m_iNoOutfitX*ICON_GRID_WIDTH,
					m_iNoOutfitY*ICON_GRID_HEIGHT,
					m_iNoOutfitX+CHAR_ICON_FULL_WIDTH*ICON_GRID_WIDTH,
					m_iNoOutfitY+CHAR_ICON_FULL_HEIGHT*ICON_GRID_HEIGHT);

	UIOutfitIcon.Show(true);
	UIOutfitIcon.Enable(false);
	UIOutfitIcon.ClipperOn();
}

void CUIOutfitSlot::AttachChild(CUIDragDropItem* pChild)
{
	PIItem pInvItem = (PIItem)pChild->GetData();

	if(!pInvItem) 
	{
		inherited::AttachChild(pChild);
		return;
	}

	CCustomOutfit* pOutfit = dynamic_cast<CCustomOutfit*>(pInvItem);
	//в этот слот могут помещаться только костюмы
	R_ASSERT(pOutfit);


	UIOutfitIcon.SetShader(GetCharIconsShader());
	UIOutfitIcon.GetUIStaticItem().SetOriginalRect(
					pOutfit->GetIconX()*ICON_GRID_WIDTH,
					pOutfit->GetIconY()*ICON_GRID_HEIGHT,
					pOutfit->GetIconX()+CHAR_ICON_FULL_WIDTH*ICON_GRID_WIDTH,
					pOutfit->GetIconY()+CHAR_ICON_FULL_HEIGHT*ICON_GRID_HEIGHT);


	inherited::AttachChild(pChild);
}
void CUIOutfitSlot::DetachChild(CUIDragDropItem* pChild)
{
	inherited::DetachChild(pChild);
}


void CUIOutfitSlot::Draw()
{
	inherited::Draw();
}
void CUIOutfitSlot::Update()
{
	inherited::Update();
}

void CUIOutfitSlot::DropAll()
{
	inherited::DropAll();
	inherited::AttachChild(&UIOutfitIcon);
}