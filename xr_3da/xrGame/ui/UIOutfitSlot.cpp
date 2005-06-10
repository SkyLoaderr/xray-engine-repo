// UIOutfitSlot.cpp:  ���� ������� � ������� ���������
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
	//� ���� ���� ����� ���������� ������ �������
	R_ASSERT(pOutfit);

	if (GameID() != GAME_SINGLE)
	{
		SetMPOutfit();
	}
	else
	{
		UIOutfitIcon.SetShader(GetCharIconsShader());
		UIOutfitIcon.GetUIStaticItem().SetOriginalRect(
			float(pOutfit->GetIconX()*ICON_GRID_WIDTH),
			float(pOutfit->GetIconY()*ICON_GRID_HEIGHT),
			float(CHAR_ICON_FULL_WIDTH*ICON_GRID_WIDTH),
			float(CHAR_ICON_FULL_HEIGHT*ICON_GRID_HEIGHT));
	}

	// �������� �����������
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
//  ������������� ������ ������� �� ���������.
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
			CHAR_ICON_FULL_WIDTH*ICON_GRID_WIDTH,
			CHAR_ICON_FULL_HEIGHT*ICON_GRID_HEIGHT);
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

void CUIOutfitSlot::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if (WINDOW_LBUTTON_DB_CLICK == mouse_action)
	{
		GetMessageTarget()->SendMessage(this, UNDRESS_OUTFIT, NULL);
	}
	else if (WINDOW_LBUTTON_DOWN == mouse_action && GetCurrentOutfit())
	{
		CUIDragDropItem *pDDItem = GetCurrentOutfit();
		pDDItem->Show(true);
	}
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

	int m_iSkinX = 0, m_iSkinY = 0;
	
	if( pSettings->line_exist("multiplayer_skins", a.c_str())){
		sscanf(pSettings->r_string("multiplayer_skins", a.c_str()), "%i,%i", &m_iSkinX, &m_iSkinY);

		UIOutfitIcon.GetUIStaticItem().SetOriginalRect(
			float(m_iSkinX), float(m_iSkinY), float(SKIN_TEX_WIDTH), float(SKIN_TEX_HEIGHT)); 
	}
}