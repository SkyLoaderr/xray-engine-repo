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

void CUIOutfitSlot::Init(int x, int y, int width, int height)
{
	inherited::Init(x, y, width, height);
	
	inherited::AttachChild(&UIOutfitIcon);
	UIOutfitIcon.Init(0,0,width,height);

	m_iNoOutfitX = pSettings->r_u32("without_outfit", "full_scale_icon_x");
	m_iNoOutfitY = pSettings->r_u32("without_outfit", "full_scale_icon_y");

	SetOriginalOutfit();

	// ������� ���������� ������ ������������� �������� ������ ���������, ����� �����������
	// ��������� ������� � ������ �������. ���������� �������� ������� ����. :( ������ ��������
	// ���� �� ����.
	UIOutfitIcon.SetTextureScale(0.67f);
	UIOutfitIcon.Show(true);
	UIOutfitIcon.Enable(false);
	UIOutfitIcon.ClipperOn();
}

void CUIOutfitSlot::AttachChild(CUIWindow *pChild)
{
	CUIDragDropItem *pDDItem = dynamic_cast<CUIDragDropItem*>(pChild);
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

	CCustomOutfit* pOutfit = dynamic_cast<CCustomOutfit*>(pInvItem);
	//� ���� ���� ����� ���������� ������ �������
	R_ASSERT(pOutfit);

	if (Game().type != GAME_SINGLE)
	{
		UIOutfitIcon.SetShader(GetMPCharIconsShader());
		CObject *pInvOwner = dynamic_cast<CObject*>(Level().CurrentEntity());
		std::string a = *pInvOwner->cNameVisual();
		std::string::iterator it = std::find(a.rbegin(), a.rend(), '\\').base(); 
		if (it != a.begin())
			a.erase(a.begin(), it);

		int m_iSkinX = 0, m_iSkinY = 0;
		sscanf(pSettings->r_string("multiplayer_skins", a.c_str()), "%i,%i", &m_iSkinX, &m_iSkinY);

		UIOutfitIcon.GetUIStaticItem().SetOriginalRect(
			m_iSkinX, m_iSkinY, SKIN_TEX_WIDTH, SKIN_TEX_HEIGHT); 
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
	UIOutfitIcon.SetShader(GetCharIconsShader());
	UIOutfitIcon.GetUIStaticItem().SetOriginalRect(
		m_iNoOutfitX*ICON_GRID_WIDTH,
		m_iNoOutfitY*ICON_GRID_HEIGHT,
		m_iNoOutfitX+CHAR_ICON_FULL_WIDTH*ICON_GRID_WIDTH,
		m_iNoOutfitY+CHAR_ICON_FULL_HEIGHT*ICON_GRID_HEIGHT);
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

void CUIOutfitSlot::OnMouse(int x, int y, CUIWindow::E_MOUSEACTION mouse_action)
{
	if (CUIWindow::LBUTTON_DB_CLICK == mouse_action)
	{
		GetMessageTarget()->SendMessage(this, UNDRESS_OUTFIT, NULL);
	}
//	else if (CUIWindow::LBUTTON_DOWN == mouse_action && GetCurrentOutfit())
//	{
//		CUIDragDropItem *pDDItem = GetCurrentOutfit();
//		pDDItem->Show(true);
//	}
	else if (CUIWindow::LBUTTON_UP == mouse_action && GetCurrentOutfit())
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
