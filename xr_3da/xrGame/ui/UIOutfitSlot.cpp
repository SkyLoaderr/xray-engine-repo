#include "stdafx.h"
#include "UIOutfitSlot.h"
#include "UIStatic.h"
#include "UICellItem.h"
#include "../CustomOutfit.h"
#include "UIInventoryUtilities.h"

CUIOutfitDragDropList::CUIOutfitDragDropList()
{
	m_background				= xr_new<CUIStatic>();
	m_background->SetAutoDelete	(true);
	AttachChild					(m_background);
}

CUIOutfitDragDropList::~CUIOutfitDragDropList()
{
}

void CUIOutfitDragDropList::SetOutfit(CUICellItem* itm)
{
	static float fNoOutfitX				= pSettings->r_float("without_outfit", "full_scale_icon_x");
	static float fNoOutfitY				= pSettings->r_float("without_outfit", "full_scale_icon_y");
	
	Frect			r;
	r.x1			= fNoOutfitX*ICON_GRID_WIDTH;
	r.y1			= fNoOutfitY*ICON_GRID_HEIGHT;
	if(itm)
	{
		PIItem _iitem	= (PIItem)itm->m_pData;
		CCustomOutfit* pOutfit = smart_cast<CCustomOutfit*>(_iitem); VERIFY(pOutfit);

		r.x1			= float(pOutfit->GetIconX())*ICON_GRID_WIDTH;
		r.y1			= float(pOutfit->GetIconY())*ICON_GRID_HEIGHT;
	}
	r.x2				= r.x1+CHAR_ICON_FULL_WIDTH*ICON_GRID_WIDTH;
	r.y2				= r.y1+CHAR_ICON_FULL_HEIGHT*ICON_GRID_HEIGHT;
	
	m_background->Init(0,0, GetWidth(), GetHeight());
	m_background->SetShader				(InventoryUtilities::GetCharIconsShader());

	m_background->SetOriginalRect		(r);

	m_background->TextureAvailable		(true);
	m_background->TextureOn				();
	m_background->RescaleRelative2Rect	(r);
}

void CUIOutfitDragDropList::SetItem(CUICellItem* itm)
{
	if(itm)	inherited::SetItem			(itm);
	SetOutfit							(itm);
}

void CUIOutfitDragDropList::SetItem(CUICellItem* itm, Fvector2 abs_pos)
{
	if(itm)	inherited::SetItem			(itm, abs_pos);
	SetOutfit							(itm);
}

void CUIOutfitDragDropList::SetItem(CUICellItem* itm, Ivector2 cell_pos)
{
	if(itm)	inherited::SetItem			(itm, cell_pos);
	SetOutfit							(itm);
}

CUICellItem* CUIOutfitDragDropList::RemoveItem(CUICellItem* itm, bool force_root)
{
	VERIFY								(!force_root);
	CUICellItem* ci						= inherited::RemoveItem(itm, force_root);
	SetOutfit							(NULL);
	return								ci;
}


void CUIOutfitDragDropList::Draw()
{
	m_background->Draw					();
//.	inherited::Draw						();
}

/*
#include "../game_cl_base.h"
#include "../Level.h"
#include "UITextureMaster.h"
#include "UISkinSelector.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;



CUIOutfitSlot::CUIOutfitSlot()
{
}
CUIOutfitSlot::~CUIOutfitSlot()
{
}

void CUIOutfitSlot::Init(float x, float y, float width, float height)
{
	inherited::Init(x, y, width, height);
	
	inherited::AttachChild(&UIOutfitIcon);
	UIOutfitIcon.Init				(0,0,width,height);

	m_iNoOutfitX					= pSettings->r_float("without_outfit", "full_scale_icon_x");
	m_iNoOutfitY					= pSettings->r_float("without_outfit", "full_scale_icon_y");

	SetOriginalOutfit				();

	UIOutfitIcon.SetStretchTexture	(true);
	UIOutfitIcon.Show				(true);
	UIOutfitIcon.Enable				(false);
	UIOutfitIcon.ClipperOn			();
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
		// warning! rect using in two different interpretations
		UIOutfitIcon.SetShader(GetCharIconsShader());
		Frect r;
		r.set(float(pOutfit->GetIconX()*ICON_GRID_WIDTH),float(pOutfit->GetIconY()*ICON_GRID_HEIGHT),
			CHAR_ICON_FULL_WIDTH*ICON_GRID_WIDTH,CHAR_ICON_FULL_HEIGHT*ICON_GRID_HEIGHT);
		UIOutfitIcon.GetUIStaticItem().SetOriginalRect(r.x1,r.y1,r.x2,r.y2);
		r.x2 += r.x1;
		r.y2 += r.y1;
		UIOutfitIcon.RescaleRelative2Rect(r);
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
		// warning! rect using in two different interpretations
		Frect r;
		r.set(m_iNoOutfitX*ICON_GRID_WIDTH, m_iNoOutfitY*ICON_GRID_HEIGHT,
			CHAR_ICON_FULL_WIDTH*ICON_GRID_WIDTH,CHAR_ICON_FULL_HEIGHT*ICON_GRID_HEIGHT);
		UIOutfitIcon.GetUIStaticItem().SetOriginalRect(r.x1, r.y1, r.x2, r.y2);
		r.x2 += r.x1;
		r.y2 += r.y1;
		UIOutfitIcon.RescaleRelative2Rect(r);
	}
}

void CUIOutfitSlot::DropAll()
{
	inherited::DropAll();
	inherited::AttachChild(&UIOutfitIcon);
}

bool CUIOutfitSlot::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if (WINDOW_LBUTTON_DB_CLICK == mouse_action)
	{
		GetMessageTarget()->SendMessage(this, UNDRESS_OUTFIT, NULL);
		return true;
	}
	else if (WINDOW_LBUTTON_DOWN == mouse_action && GetCurrentOutfit())
	{
		CUIDragDropItem *pDDItem = GetCurrentOutfit();
		pDDItem->Show(true);
		return true;
	}
	else if (WINDOW_LBUTTON_UP == mouse_action && GetCurrentOutfit())
	{
		CUIDragDropItem *pDDItem = GetCurrentOutfit();
		pDDItem->Show(false);
		return true;
	}

	return inherited::OnMouse(x, y, mouse_action);
}

CUIDragDropItem * CUIOutfitSlot::GetCurrentOutfit()
{
	if (!m_DragDropItemsList.empty())
		return *m_DragDropItemsList.begin();
	return NULL;
}

void CUIOutfitSlot::SetMPOutfit()
{
	CObject *pInvOwner = smart_cast<CObject*>(Level().CurrentEntity());
	if (!pInvOwner) return;
	if (!pInvOwner->cNameVisual().size() ) return;

	xr_string a = *pInvOwner->cNameVisual();
	xr_string::iterator it = std::find(a.rbegin(), a.rend(), '\\').base(); 

	// Cut leading full path
	if (it != a.begin())
		a.erase(a.begin(), it);
	// Cut trailing ".ogf"
	R_ASSERT(xr_strlen(a.c_str()) > 4);
	if ('.' == a[a.size() - 4])
		a.erase(a.size() - 4);

	UIOutfitIcon.InitTexture(a.c_str());
	UIOutfitIcon.RescaleRelative2Rect(CUITextureMaster::GetTextureRect(a.c_str()));
}
*/