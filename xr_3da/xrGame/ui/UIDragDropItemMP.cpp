//=============================================================================
//  Filename:   UIDragDropItemMP.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ���������� ������������� ���� � ���� ������� ������ � �������������.
//	�� �������������� �� �������� ��������� �������������� �������
//=============================================================================

#include "stdafx.h"
#include "UIDragDropItemMP.h"
#include "UIBuyWeaponWnd.h"
#include "../MainUI.h"
#include "../HUDManager.h"

//////////////////////////////////////////////////////////////////////////

void CUIDragDropItemMP::AttachDetachAddon(AddonIDs iAddonIndex, bool bAttach, bool bRealRepresentationSet)
{
	R_ASSERT(iAddonIndex >= 0 && iAddonIndex < 3);
	if (m_AddonInfo[iAddonIndex].iAttachStatus != -1)
	{
		CUIBag *this_inventory = smart_cast<CUIBag*>(GetOwner()->GetMessageTarget());
		R_ASSERT(this_inventory);

		CUIDragDropItemMP *pPossibleAddon = this_inventory->GetAddonByID(this, iAddonIndex);

		AttachDetachAddon(pPossibleAddon, bAttach, bRealRepresentationSet);
	}
}

void CUIDragDropItemMP::AttachDetachAddon(CUIDragDropItemMP *pPossibleAddon, bool bAttach, bool bRealRepresentationSet)
{
	// ��������� �������� �� pPossibleAddon ������������� ����� �������
	AddonIDs ID = IsOurAddon(pPossibleAddon);
	if (ID != ID_NONE)
	{
		CUIBuyWeaponWnd *this_inventory = smart_cast<CUIBuyWeaponWnd*>(GetOwner()->GetMessageTarget()->GetParent());
		R_ASSERT(this_inventory);

		if (bAttach)
		{
			if (m_AddonInfo[ID].iAttachStatus != 1 && pPossibleAddon->GetCost() <= this_inventory->GetMoneyAmount())
			{
				m_pAddon[ID] = pPossibleAddon;
				m_pAddon[ID]->GetParent()->DetachChild(m_pAddon[ID]);
				GetParent()->AttachChild(m_pAddon[ID]);
				m_pAddon[ID]->Show(false);
				m_pAddon[ID]->EnableDragDrop(false);
				m_pAddon[ID]->Enable(false);
				m_pAddon[ID]->m_bHasRealRepresentation = bRealRepresentationSet;
				m_pAddon[ID]->Rescale(  smart_cast<CUIDragDropList*>(m_pAddon[ID]->GetParent())->GetItemsScaleX(),
										smart_cast<CUIDragDropList*>(m_pAddon[ID]->GetParent())->GetItemsScaleY());

				// �������� �������
				this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() - 
					static_cast<int>(m_pAddon[ID]->GetCost() * (m_pAddon[ID]->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
				m_pAddon[ID]->m_bAlreadyPaid = true;
			}
		}
		else
		{
			if (m_AddonInfo[ID].iAttachStatus != 0)
			{
				GetParent()->DetachChild(m_pAddon[ID]);
				m_pAddon[ID]->GetOwner()->AttachChild(m_pAddon[ID]);
				m_pAddon[ID]->Show(true);
				m_pAddon[ID]->EnableDragDrop(true);
				m_pAddon[ID]->Enable(true);
				m_pAddon[ID]->Rescale(  smart_cast<CUIDragDropList*>(m_pAddon[ID]->GetParent())->GetItemsScaleX(),
										smart_cast<CUIDragDropList*>(m_pAddon[ID]->GetParent())->GetItemsScaleY());
				// ���������� �������
				if (m_pAddon[ID]->GetColor() != cUnableToBuy)
				{
					this_inventory->SetMoneyAmount(this_inventory->GetMoneyAmount() + 
						static_cast<int>(m_pAddon[ID]->GetCost() * (m_pAddon[ID]->m_bHasRealRepresentation ? fRealItemSellMultiplier : 1)));
					m_pAddon[ID]->m_bAlreadyPaid = true;
				}

				m_pAddon[ID] = NULL;
			}
		}
//		this_inventory->CheckBuyAvailabilityInShop();
	}

	m_AddonInfo[ID].iAttachStatus = bAttach ? 1 : 0;
	//		m_pAddon[iAddonIndex]->GetUIStaticItem().SetColor(bAttach ? cAttached : cDetached);
	//		m_pAddon[iAddonIndex]->EnableDragDrop(!bAttach);
	//		
}

//////////////////////////////////////////////////////////////////////////

void CUIDragDropItemMP::AttachDetachAllAddons(bool bAttach)
{
	for (int i = 0; i < 3; ++i)
	{
		if (m_pAddon[i])
			AttachDetachAddon(static_cast<AddonIDs>(i), bAttach, m_pAddon[i]->m_bHasRealRepresentation);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIDragDropItemMP::ClipperOff()
{
	inherited::ClipperOff();

	for (u8 i = 0; i < CUIDragDropItemMP::NUM_OF_ADDONS; ++i)
	{
		if(1 == m_AddonInfo[i].iAttachStatus) inherited::ClipperOff(m_pAddon[i]->GetUIStaticItem());
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIDragDropItemMP::ClipperOn()
{
	inherited::ClipperOn();

	for (u8 i = 0; i < CUIDragDropItemMP::NUM_OF_ADDONS; ++i)
	{
		if(1 == m_AddonInfo[i].iAttachStatus) this->TextureClipper(0,0,NULL, m_pAddon[i]->GetUIStaticItem());
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIDragDropItemMP::Draw()
{
	inherited::Draw();

	Irect rect = GetAbsoluteRect();

	//�������������� ������������ ������ �� ������ �� ����
	int right_offset = (GetWidth()-m_UIStaticItem.GetOriginalRectScaled().width())/2;
	int down_offset = (GetHeight()-m_UIStaticItem.GetOriginalRectScaled().height())/2;

	// ������������ ������ �� �������
	if (bAddonsAvailable)
	{
		CUIDragDropItemMP		*pDDItemMP = NULL;
		AddonInfo				nfo;
		for (int i = 0; i < CUIDragDropItemMP::NUM_OF_ADDONS; ++i)
		{

			if(0 == m_AddonInfo[i].iAttachStatus || !m_pAddon[i])
				continue;

			pDDItemMP	= m_pAddon[i];
			nfo			= m_AddonInfo[i];

			// � ������ ���������
			pDDItemMP->GetUIStaticItem().SetPos(rect.left + right_offset + iFloor(0.5f+(float)nfo.x * GetTextureScaleX()),
				rect.top + down_offset + iFloor(0.5f+(float)nfo.y * GetTextureScaleY()));

			if(m_bClipper) TextureClipper(right_offset + iFloor(0.5f+(float)nfo.x * GetTextureScaleX()),
				down_offset +  iFloor(0.5f+(float)nfo.y * GetTextureScaleY()), 
				NULL, pDDItemMP->GetUIStaticItem());

//			if (pDDItemMP->m_bHasRealRepresentation)
//				pDDItemMP->GetUIStaticItem().SetColor(cAbleToBuyOwned);
//			else
//				pDDItemMP->GetUIStaticItem().SetColor(cDetached);

			pDDItemMP->GetUIStaticItem().Render();
//			pDDItemMP->GetUIStaticItem().SetColor(cAttached);

			pDDItemMP = NULL;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

CUIDragDropItemMP::AddonIDs CUIDragDropItemMP::IsOurAddon(CUIDragDropItemMP * pPossibleAddon)
{
	if (!pPossibleAddon) return ID_NONE;

	for (int i = 0; i < NUM_OF_ADDONS; ++i)
	{
		if (m_AddonInfo[i].iAttachStatus != -1								&&
			pPossibleAddon->GetSectionName() == m_AddonInfo[i].strAddonName)
		{
			return static_cast<AddonIDs>(i);
		}
	}

	return ID_NONE;
}

//////////////////////////////////////////////////////////////////////////

void CUIDragDropItemMP::SetSlot(int slot)
{
	R_ASSERT(slot < MP_SLOTS_NUM || slot == static_cast<u32>(-1) || slot == WEAPON_BOXES_SLOT);
	slotNum = slot; 
}

void WpnDrawIndex(CUIDragDropItem *pDDItem)
{
	CUIDragDropItemMP *pDDItemMP = smart_cast<CUIDragDropItemMP*>(pDDItem);
	R_ASSERT(pDDItemMP);
	if (!pDDItemMP) return;

	int left	= pDDItemMP->GetUIStaticItem().GetPosX();
	int bottom	= pDDItemMP->GetUIStaticItem().GetPosY() + pDDItemMP->GetUIStaticItem().GetRect().height();

	pDDItemMP->GetFont()->SetColor(0xffffffff);
	UI()->OutText(pDDItem->GetFont(), pDDItemMP->GetSelfClipRect(), float(left), 
		float(bottom - pDDItemMP->GetFont()->CurrentHeight()),
		"%d", pDDItemMP->GetPosInSectionsGroup() + 1);

	pDDItemMP->GetFont()->OnRender();
}