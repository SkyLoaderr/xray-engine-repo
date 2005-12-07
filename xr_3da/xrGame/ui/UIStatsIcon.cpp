#include "StdAfx.h"
#include "UIStatsIcon.h"
#include "UITextureMaster.h"
#include "UIInventoryUtilities.h"

CUIStatsIcon::TEX_INFO		CUIStatsIcon::m_tex_info[MAX_DEF_TEX];

CUIStatsIcon::CUIStatsIcon(){
	SetStretchTexture(true);
	InitTexInfo();
	m_bAvailableTexture = true;
}

using namespace InventoryUtilities;

void CUIStatsIcon::InitTexInfo(){
	if (m_tex_info[RANK_0].sh)
		return;

	// ranks
	m_tex_info[RANK_0].sh.create("hud\\default",	CUITextureMaster::GetTextureFileName("rank_0"));
	m_tex_info[RANK_0].rect =						CUITextureMaster::GetTextureRect	("rank_0");
	m_tex_info[RANK_1].sh.create("hud\\default",	CUITextureMaster::GetTextureFileName("rank_1"));
	m_tex_info[RANK_1].rect =						CUITextureMaster::GetTextureRect	("rank_1");
	m_tex_info[RANK_2].sh.create("hud\\default",	CUITextureMaster::GetTextureFileName("rank_2"));
	m_tex_info[RANK_2].rect =						CUITextureMaster::GetTextureRect	("rank_2");
	m_tex_info[RANK_3].sh.create("hud\\default",	CUITextureMaster::GetTextureFileName("rank_3"));
	m_tex_info[RANK_3].rect =						CUITextureMaster::GetTextureRect	("rank_3");
	m_tex_info[RANK_4].sh.create("hud\\default",	CUITextureMaster::GetTextureFileName("rank_4"));
	m_tex_info[RANK_4].rect =						CUITextureMaster::GetTextureRect	("rank_4");

	// artefact
	LPCSTR artefact_name = pSettings->r_string("artefacthunt_gamedata", "artefact");
	float fGridWidth	= pSettings->r_float(artefact_name, "inv_grid_width");
	float fGridHeight	= pSettings->r_float(artefact_name, "inv_grid_height");
	float fXPos			= pSettings->r_float(artefact_name, "inv_grid_x");
	float fYPos			= pSettings->r_float(artefact_name, "inv_grid_y");

	m_tex_info[ARTEFACT].sh = GetEquipmentIconsShader();
	m_tex_info[ARTEFACT].rect.set(
		fXPos * INV_GRID_WIDTH, 
		fYPos * INV_GRID_HEIGHT, 
		fGridWidth * INV_GRID_WIDTH, 
		fGridHeight * INV_GRID_HEIGHT);

	// death
	m_tex_info[DEATH].sh.create("hud\\default",	"ui\\ui_mp_icon_kill");
	m_tex_info[DEATH].rect.x1 = 32;
	m_tex_info[DEATH].rect.y1 = 202;
	m_tex_info[DEATH].rect.x2 = m_tex_info[DEATH].rect.x1 + 30;
	m_tex_info[DEATH].rect.y2 = m_tex_info[DEATH].rect.y1 + 30;	
}

void CUIStatsIcon::FreeTexInfo(){
		// ranks
	m_tex_info[RANK_0].sh.destroy();
	m_tex_info[RANK_1].sh.destroy();
	m_tex_info[RANK_2].sh.destroy();
	m_tex_info[RANK_3].sh.destroy();
	m_tex_info[RANK_4].sh.destroy();
	m_tex_info[ARTEFACT].sh.destroy();
	m_tex_info[DEATH].sh.destroy();	
}

void CUIStatsIcon::SetText(LPCSTR str){
	CTimer T;
	T.Start();

	if (0 == str[0])
	{
		SetVisible(false);
		return;
	}
	else
		SetVisible(true);

	if (0 == xr_strcmp(str,"rank_0"))
	{
		GetStaticItem()->SetShader(m_tex_info[RANK_0].sh);
		SetOriginalRect(m_tex_info[RANK_0].rect);
	} 
	else if (0 == xr_strcmp(str,"rank_1"))
	{
		GetStaticItem()->SetShader(m_tex_info[RANK_1].sh);
		SetOriginalRect(m_tex_info[RANK_1].rect);
	}
	else if (0 == xr_strcmp(str,"rank_2"))
	{
		GetStaticItem()->SetShader(m_tex_info[RANK_2].sh);
		SetOriginalRect(m_tex_info[RANK_2].rect);
	}
	else if (0 == xr_strcmp(str,"rank_3"))
	{
		GetStaticItem()->SetShader(m_tex_info[RANK_3].sh);
		SetOriginalRect(m_tex_info[RANK_3].rect);
	}
	else if (0 == xr_strcmp(str,"rank_4"))
	{
		GetStaticItem()->SetShader(m_tex_info[RANK_4].sh);
		SetOriginalRect(m_tex_info[RANK_4].rect);
	}
	else if (0 == xr_strcmp(str,"death"))
	{
		GetStaticItem()->SetShader(m_tex_info[DEATH].sh);
		SetOriginalRect(m_tex_info[DEATH].rect);
	}
	else if (0 == xr_strcmp(str,"artefact"))
	{
		GetStaticItem()->SetShader(m_tex_info[ARTEFACT].sh);
		SetOriginalRect(m_tex_info[ARTEFACT].rect);
	}
	else
	{
		InitTexture(str);		
	}		

	u32 temp = T.GetElapsed_ms();
	temp += 0;
}

