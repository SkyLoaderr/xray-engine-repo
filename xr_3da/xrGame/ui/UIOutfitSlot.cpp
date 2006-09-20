#include "stdafx.h"
#include "UIOutfitSlot.h"
#include "UIStatic.h"
#include "UICellItem.h"
#include "../CustomOutfit.h"
#include "../actor.h"
#include "UIInventoryUtilities.h"

CUIOutfitDragDropList::CUIOutfitDragDropList()
{
	m_background				= xr_new<CUIStatic>();
	m_background->SetAutoDelete	(true);
	AttachChild					(m_background);
	m_default_outfit			= "without_outfit";
}

CUIOutfitDragDropList::~CUIOutfitDragDropList()
{
}

#include "../level.h"
#include "../game_base_space.h"

void CUIOutfitDragDropList::SetOutfit(CUICellItem* itm)
{
	static float fNoOutfitX				= pSettings->r_float(m_default_outfit, "full_scale_icon_x");
	static float fNoOutfitY				= pSettings->r_float(m_default_outfit, "full_scale_icon_y");
	
	Frect			r;
	r.x1			= fNoOutfitX*ICON_GRID_WIDTH;
	r.y1			= fNoOutfitY*ICON_GRID_HEIGHT;
	r.x2			= r.x1+CHAR_ICON_FULL_WIDTH*ICON_GRID_WIDTH;
	r.y2			= r.y1+CHAR_ICON_FULL_HEIGHT*ICON_GRID_HEIGHT;
	
	m_background->Init					(0,0, GetWidth(), GetHeight());
	m_background->SetStretchTexture		(true);


	if ((GameID() != GAME_SINGLE) && !itm){
		CObject *pActor = NULL;

        pActor = smart_cast<CActor*>(Level().CurrentEntity());

		xr_string a;
		if (pActor)
			a = *pActor->cNameVisual();
		else
			a = *m_default_outfit;

		xr_string::iterator it = std::find(a.rbegin(), a.rend(), '\\').base(); 

		// Cut leading full path
		if (it != a.begin())
			a.erase(a.begin(), it);
		// Cut trailing ".ogf"
		R_ASSERT(xr_strlen(a.c_str()) > 4);
		if ('.' == a[a.size() - 4])
			a.erase(a.size() - 4);

		if (a != "without_outfit")
            m_background->InitTexture(a.c_str());
		else{
			m_background->SetShader				(InventoryUtilities::GetCharIconsShader());
			m_background->SetOriginalRect		(r);
		}
	}
	else {
		if(itm)
		{
			PIItem _iitem	= (PIItem)itm->m_pData;
			CCustomOutfit* pOutfit = smart_cast<CCustomOutfit*>(_iitem); VERIFY(pOutfit);

			r.x1			= float(pOutfit->GetIconX())*ICON_GRID_WIDTH;
			r.y1			= float(pOutfit->GetIconY())*ICON_GRID_HEIGHT;
		}
		r.x2			= r.x1+CHAR_ICON_FULL_WIDTH*ICON_GRID_WIDTH;
		r.y2			= r.y1+CHAR_ICON_FULL_HEIGHT*ICON_GRID_HEIGHT;

		m_background->SetShader				(InventoryUtilities::GetCharIconsShader());
        m_background->SetOriginalRect		(r);
	}



	m_background->TextureAvailable		(true);
	m_background->TextureOn				();
//	m_background->RescaleRelative2Rect	(r);
}

void CUIOutfitDragDropList::SetDefaultOutfit(LPCSTR default_outfit){
	m_default_outfit = default_outfit;
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