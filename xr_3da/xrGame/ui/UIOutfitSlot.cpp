// UIOutfitSlot.cpp:  слот костюма в диалоге инвентаря
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIOutfitSlot.h"

#include "../CustomOutfit.h"
#include "../game_cl_base.h"
#include "../Level.h"
#include "UISkinSelector.h"

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

	SetOriginalOutfit();

	// Немного ублюдочный способ прорескейлить текстуру иконки персонажа, чтобы изображение
	// полностью влазило в статик контрол. Коэфициент подобран методом тыка. :( Других способов
	// пока не вижу.
	UIOutfitIcon.SetTextureScale(0.67f);
	UIOutfitIcon.Show(true);
	UIOutfitIcon.Enable(false);
	UIOutfitIcon.ClipperOn();
}

void CUIOutfitSlot::AttachChild(CUIWindow *pChild)
{
	CUIDragDropItem *pDDItem = smart_cast<CUIDragDropItem*>(pChild);
	if (!pDDItem)
	{
		inherited::AttachChild(pChild);
		return;
	}

	PIItem pInvItem = (PIItem)pDDItem->GetData();

	if(!pInvItem) 
	{
		inherited::AttachChild(pDDItem);
		return;
	}

	CCustomOutfit* pOutfit = smart_cast<CCustomOutfit*>(pInvItem);
	//в этот слот могут помещаться только костюмы
	R_ASSERT(pOutfit);

	if (GameID() != GAME_SINGLE)
	{
		SetMPOutfit();
	}
	else
	{
		UIOutfitIcon.SetShader(GetCharIconsShader());
		UIOutfitIcon.GetUIStaticItem().SetOriginalRect(
			pOutfit->GetIconX()*ICON_GRID_WIDTH,
			pOutfit->GetIconY()*ICON_GRID_HEIGHT,
			pOutfit->GetIconX()+CHAR_ICON_FULL_WIDTH*ICON_GRID_WIDTH,
			pOutfit->GetIconY()+CHAR_ICON_FULL_HEIGHT*ICON_GRID_HEIGHT);
	}

	// Скрываем изображение
	pDDItem->Show(false);
	inherited::AttachChild(pDDItem);
}
void CUIOutfitSlot::DetachChild(CUIWindow *pChild)
{
	SetOriginalOutfit();
	pChild->Show(true);
	inherited::DetachChild(pChild);
}

//-----------------------------------------------------------------------------/
//  Устанавливаем костюм заданый по умолчанию.
//-----------------------------------------------------------------------------/
void CUIOutfitSlot::SetOriginalOutfit()
{
	if (GameID() != GAME_SINGLE)
	{
		SetMPOutfit();
	}
	else
	{
		UIOutfitIcon.SetShader(GetCharIconsShader());
		UIOutfitIcon.GetUIStaticItem().SetOriginalRect(
			m_iNoOutfitX*ICON_GRID_WIDTH,
			m_iNoOutfitY*ICON_GRID_HEIGHT,
			m_iNoOutfitX+CHAR_ICON_FULL_WIDTH*ICON_GRID_WIDTH,
			m_iNoOutfitY+CHAR_ICON_FULL_HEIGHT*ICON_GRID_HEIGHT);
	}
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

void CUIOutfitSlot::OnMouse(int x, int y, EUIMessages mouse_action)
{
	if (WINDOW_LBUTTON_DB_CLICK == mouse_action)
	{
		GetMessageTarget()->SendMessage(this, UNDRESS_OUTFIT, NULL);
	}
//	else if (CUIWindow::LBUTTON_DOWN == mouse_action && GetCurrentOutfit())
//	{
//		CUIDragDropItem *pDDItem = GetCurrentOutfit();
//		pDDItem->Show(true);
//	}
	else if (WINDOW_LBUTTON_UP == mouse_action && GetCurrentOutfit())
	{
		CUIDragDropItem *pDDItem = GetCurrentOutfit();
		pDDItem->Show(false);
	}

	inherited::OnMouse(x, y, mouse_action);
}

CUIDragDropItem * CUIOutfitSlot::GetCurrentOutfit()
{
	if (!m_DragDropItemsList.empty())
		return *m_DragDropItemsList.begin();
	return NULL;
}

void CUIOutfitSlot::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
//	if (OUTFIT_RETURNED_BACK == msg)
//	{
//		pWnd->Show(false);
//	}
	inherited::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////

void CUIOutfitSlot::SetMPOutfit()
{
	UIOutfitIcon.SetShader(GetMPCharIconsShader());
	CObject *pInvOwner = smart_cast<CObject*>(Level().CurrentEntity());
	if (!pInvOwner) return;
	if (pInvOwner->cNameVisual() == NULL) return;

	std::string a = *pInvOwner->cNameVisual();
	std::string::iterator it = std::find(a.rbegin(), a.rend(), '\\').base(); 

	// Cut leading full path
	if (it != a.begin())
		a.erase(a.begin(), it);
	// Cut trailing ".ogf"
	R_ASSERT(xr_strlen(a.c_str()) > 4);
	if ('.' == a[a.size() - 4])
		a.erase(a.size() - 4);

	int m_iSkinX = 0, m_iSkinY = 0;
	
	if( pSettings->line_exist("multiplayer_skins", a.c_str())){
		sscanf(pSettings->r_string("multiplayer_skins", a.c_str()), "%i,%i", &m_iSkinX, &m_iSkinY);

		UIOutfitIcon.GetUIStaticItem().SetOriginalRect(
			m_iSkinX, m_iSkinY, SKIN_TEX_WIDTH, SKIN_TEX_HEIGHT); 
	}
}