// UIWpnDragDropItem.cpp: специальный класс перетаскиваемой картинки
//						  переделанный для оружия с апгрейцдами
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIWpnDragDropItem.h"
#include "UIInventoryUtilities.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIWpnDragDropItem:: CUIWpnDragDropItem()
{
	m_pWeapon = NULL;
}

CUIWpnDragDropItem::~ CUIWpnDragDropItem()
{
}


void CUIWpnDragDropItem::Draw()
{
	inherited::Draw();

	if(!m_pWeapon)	return;

	RECT rect = GetAbsoluteRect();



	//отцентрировать родительскую иконку по центру ее окна
	int right_offset = (GetWidth()-m_UIStaticItem.GetOriginalRectScaled().width())/2;
	int down_offset = (GetHeight()-m_UIStaticItem.GetOriginalRectScaled().height())/2;

		
	if(m_pWeapon->ScopeAttachable() && m_pWeapon->IsScopeAttached())
	{
		m_UIStaticScope.SetPos(rect.left + right_offset + iFloor(0.5f+(float)m_pWeapon->GetScopeX()*GetTextureScale()),
			                   rect.top + down_offset + iFloor(0.5f+(float)m_pWeapon->GetScopeY()*GetTextureScale()));
		if(m_bClipper) TextureClipper(right_offset + iFloor(0.5f+(float)m_pWeapon->GetScopeX()*GetTextureScale()),
									  down_offset + iFloor(0.5f+(float)m_pWeapon->GetScopeY()*GetTextureScale()),
									  NULL, m_UIStaticScope);
		m_UIStaticScope.Render();
	}
	if(m_pWeapon->SilencerAttachable() && m_pWeapon->IsSilencerAttached())
	{
		m_UIStaticSilencer.SetPos(rect.left + right_offset + iFloor(0.5f+(float)m_pWeapon->GetSilencerX()*GetTextureScale()),
								  rect.top + down_offset + iFloor(0.5f+(float)m_pWeapon->GetSilencerY()*GetTextureScale()));

		if(m_bClipper) TextureClipper(right_offset + iFloor(0.5f+(float)m_pWeapon->GetSilencerX()*GetTextureScale()),
									  down_offset +  iFloor(0.5f+(float)m_pWeapon->GetSilencerY()*GetTextureScale()), 
									  NULL, m_UIStaticSilencer);

		m_UIStaticSilencer.Render();
	}
	if(m_pWeapon->GrenadeLauncherAttachable() && m_pWeapon->IsGrenadeLauncherAttached())
	{
		m_UIStaticGrenadeLauncher.SetPos(rect.left + right_offset + iFloor(0.5f+(float)m_pWeapon->GetGrenadeLauncherX()*GetTextureScale()),
										 rect.top + down_offset + iFloor(0.5f+(float)m_pWeapon->GetGrenadeLauncherY()*GetTextureScale()));
		if(m_bClipper) TextureClipper(right_offset + iFloor(0.5f+(float)m_pWeapon->GetGrenadeLauncherX()*GetTextureScale()),
									  down_offset +  iFloor(0.5f+(float)m_pWeapon->GetGrenadeLauncherY()*GetTextureScale()), 
									  NULL, m_UIStaticGrenadeLauncher);
		m_UIStaticGrenadeLauncher.Render();

	}
}

void CUIWpnDragDropItem::SetData(void* pData)
{
	inherited::SetData(pData);

	CInventoryItem* pItem = (CInventoryItem*)pData;
	m_pWeapon = smart_cast<CWeapon*>(pItem);

	if(m_pWeapon)
	{
		if(m_pWeapon->GrenadeLauncherAttachable())
		{
			m_UIStaticGrenadeLauncher.SetShader(InventoryUtilities::GetEquipmentIconsShader());

			const char* section  = *m_pWeapon->GetGrenadeLauncherName();
			u32 iGridWidth = pSettings->r_u32(section, "inv_grid_width");
			u32 iGridHeight = pSettings->r_u32(section, "inv_grid_height");
			u32 iXPos = pSettings->r_u32(section, "inv_grid_x");
			u32 iYPos = pSettings->r_u32(section, "inv_grid_y");

			m_UIStaticGrenadeLauncher.SetOriginalRect(
				iXPos*INV_GRID_WIDTH, iYPos*INV_GRID_HEIGHT,
				iGridWidth*INV_GRID_WIDTH, iGridHeight*INV_GRID_HEIGHT);
		}
		if(m_pWeapon->ScopeAttachable())
		{
			m_UIStaticScope.SetShader(InventoryUtilities::GetEquipmentIconsShader());

			const char* section  = *m_pWeapon->GetScopeName();
			u32 iGridWidth = pSettings->r_u32(section, "inv_grid_width");
			u32 iGridHeight = pSettings->r_u32(section, "inv_grid_height");
			u32 iXPos = pSettings->r_u32(section, "inv_grid_x");
			u32 iYPos = pSettings->r_u32(section, "inv_grid_y");

			m_UIStaticScope.SetOriginalRect(
				iXPos*INV_GRID_WIDTH, iYPos*INV_GRID_HEIGHT,
				iGridWidth*INV_GRID_WIDTH, iGridHeight*INV_GRID_HEIGHT);
			
		}
		if(m_pWeapon->SilencerAttachable())
		{
			m_UIStaticSilencer.SetShader(InventoryUtilities::GetEquipmentIconsShader());

			const char* section  = *m_pWeapon->GetSilencerName();
			u32 iGridWidth = pSettings->r_u32(section, "inv_grid_width");
			u32 iGridHeight = pSettings->r_u32(section, "inv_grid_height");
			u32 iXPos = pSettings->r_u32(section, "inv_grid_x");
			u32 iYPos = pSettings->r_u32(section, "inv_grid_y");

			m_UIStaticSilencer.SetRect(0,0,30,30);
			m_UIStaticSilencer.SetOriginalRect(
				iXPos*INV_GRID_WIDTH, iYPos*INV_GRID_HEIGHT,
				iGridWidth*INV_GRID_WIDTH, iGridHeight*INV_GRID_HEIGHT);

		}
	}
}

void CUIWpnDragDropItem::ClipperOff()
{
	inherited::ClipperOff();

	if(!m_pWeapon) return;


	if(m_pWeapon->IsScopeAttached())inherited::ClipperOff(m_UIStaticScope);
	if(m_pWeapon->IsSilencerAttached()) inherited::ClipperOff(m_UIStaticSilencer);
	if(m_pWeapon->IsGrenadeLauncherAttached()) inherited::ClipperOff(m_UIStaticGrenadeLauncher);
}

void CUIWpnDragDropItem::ClipperOn()
{
	inherited::ClipperOn();

	if(!m_pWeapon) return;

	if(m_pWeapon->IsScopeAttached()) this->TextureClipper(0,0,NULL, m_UIStaticScope);
	if(m_pWeapon->IsSilencerAttached())  this->TextureClipper(0,0,NULL, m_UIStaticSilencer);
	if(m_pWeapon->IsGrenadeLauncherAttached())  this->TextureClipper(0,0,NULL, m_UIStaticGrenadeLauncher);

}

void CUIWpnDragDropItem::SetTextureScale(float new_scale)
{
	inherited::SetTextureScale(new_scale);

	if(!m_pWeapon) return;

	m_UIStaticGrenadeLauncher.SetScale(new_scale);
	m_UIStaticScope.SetScale(new_scale);
	m_UIStaticSilencer.SetScale(new_scale);
}
