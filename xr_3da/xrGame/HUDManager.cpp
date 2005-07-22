// XR_Interface.cpp: implementation of the CHUD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HUDManager.h"
#include "hudcursor.h"

#include "actor.h"
//#include "car.h"
//#include "spectator.h"
#include "../igame_level.h"
#include "clsid_game.h"


CFontManager::CFontManager()
{
	Device.seqDeviceReset.Add(this,REG_PRIORITY_HIGH);

	m_all_fonts.push_back(&pFontConsole				);
	m_all_fonts.push_back(&pFontStartup				);
	m_all_fonts.push_back(&pFontSmall				);
	m_all_fonts.push_back(&pFontMedium				);
	m_all_fonts.push_back(&pFontDI					);
	m_all_fonts.push_back(&pFontBigDigit			);
	m_all_fonts.push_back(&pFontHeaderEurope		);
	m_all_fonts.push_back(&pFontHeaderRussian		);
	m_all_fonts.push_back(&pFontArial14				);
	m_all_fonts.push_back(&pArialN21Russian			);
	m_all_fonts.push_back(&pFontGraffiti19Russian	);
	m_all_fonts.push_back(&pFontGraffiti22Russian	);
	m_all_fonts.push_back(&pFontLetterica16Russian	);
	m_all_fonts.push_back(&pFontLetterica18Russian	);
	m_all_fonts.push_back(&pFontGraffiti32Russian	);
	m_all_fonts.push_back(&pFontGraffiti50Russian	);
	m_all_fonts.push_back(&pFontLetterica25			);
	m_all_fonts.push_back(&pFontStat				);

	FONTS_VEC_IT it		= m_all_fonts.begin();
	FONTS_VEC_IT it_e	= m_all_fonts.end();
	for(;it!=it_e;++it)
		(**it) = NULL;

	InitializeFonts();

}

void CFontManager::InitializeFonts()
{

	InitializeFont(pFontConsole				,"console_font",				CGameFont::fsGradient|CGameFont::fsDeviceIndependent);
	InitializeFont(pFontStartup				,"startup_font",				CGameFont::fsGradient|CGameFont::fsDeviceIndependent);
	InitializeFont(pFontSmall				,"hud_font_small"				);
	InitializeFont(pFontMedium				,"hud_font_medium"				);
	InitializeFont(pFontDI					,"hud_font_di",					CGameFont::fsGradient|CGameFont::fsDeviceIndependent);
	InitializeFont(pFontBigDigit			,"hud_font_big_digit"			);
	InitializeFont(pFontHeaderEurope		,"ui_font_header_europe"		);
	InitializeFont(pFontHeaderRussian		,"ui_font_header_russian"		);
	InitializeFont(pFontArial14				,"ui_font_arial_14"				);
	InitializeFont(pArialN21Russian			,"ui_font_arial_n_21_russian"	);
	InitializeFont(pFontGraffiti19Russian	,"ui_font_graffiti19_russian"	);
	InitializeFont(pFontGraffiti22Russian	,"ui_font_graffiti22_russian"	);
	InitializeFont(pFontLetterica16Russian	,"ui_font_letterica16_russian"	);
	InitializeFont(pFontLetterica18Russian	,"ui_font_letterica18_russian"	);
	InitializeFont(pFontGraffiti32Russian	,"ui_font_graff_32"				);
	InitializeFont(pFontGraffiti50Russian	,"ui_font_graff_50"				);
	InitializeFont(pFontLetterica25			,"ui_font_letter_25"			);
	InitializeFont(pFontStat				,"stat_font"					);

}
LPCSTR CFontManager::GetFontTexName (LPCSTR section)
{
	u32 w = Device.dwWidth;

	static char* tex_names[]={"texture800","texture","texture1600"};
	int def_idx		= 1;//default 1024x768
	int idx			= def_idx;

	if(w<=800)		idx = 0;
	else if(w<=1024)idx = 1;
	else 			idx = 2;

	while(idx>=0){
		if( pSettings->line_exist(section,tex_names[idx]) )
			return pSettings->r_string(section,tex_names[idx]);
		--idx;
	}
	return pSettings->r_string(section,tex_names[def_idx]);
}

void CFontManager::InitializeFont(CGameFont*& F, LPCSTR section, u32 flags)
{
	LPCSTR font_tex_name = GetFontTexName(section);
	R_ASSERT(font_tex_name);

	if(!F)
		F = xr_new<CGameFont> ("font", font_tex_name, flags);
	else
		F->Initialize("font",font_tex_name);

	if (pSettings->line_exist(section,"size")){
		float sz = pSettings->r_float(section,"size");
		if (flags&CGameFont::fsDeviceIndependent)	F->SetSizeI(sz);
		else										F->SetSize(sz);
	}
	if (pSettings->line_exist(section,"interval"))
		F->SetInterval(pSettings->r_fvector2(section,"interval"));

}

CFontManager::~CFontManager()
{
	Device.seqDeviceReset.Remove(this);
	FONTS_VEC_IT it		= m_all_fonts.begin();
	FONTS_VEC_IT it_e	= m_all_fonts.end();
	for(;it!=it_e;++it)
		xr_delete(**it);
}

void CFontManager::Render()
{
	FONTS_VEC_IT it		= m_all_fonts.begin();
	FONTS_VEC_IT it_e	= m_all_fonts.end();
	for(;it!=it_e;++it)
		(**it)->OnRender			();
}
void CFontManager::OnDeviceReset()
{
	InitializeFonts();
}

//--------------------------------------------------------------------
CHUDManager::CHUDManager()
{ 
	// #pragma todo("Yura, what is this and why? Oles.")
	// Level().pHUD	= this;
	
	pUI						= 0;
	// if (Device.bReady) OnDeviceCreate();

	m_pHUDCursor			= xr_new<CHUDCursor>();
}
//--------------------------------------------------------------------
CHUDManager::~CHUDManager()
{
	xr_delete			(pUI);
	xr_delete			(m_pHUDCursor);
}

//--------------------------------------------------------------------

void CHUDManager::Load()
{
	xr_delete			(pUI);
	pUI				= xr_new<CUI> (this);
	pUI->Load		();
}
//--------------------------------------------------------------------
void CHUDManager::OnFrame()
{
//	if(	Font().m_curW != Device.dwWidth ||	Font().m_curH != Device.dwHeight )
//		Font().InitializeFonts();

	if (pUI) pUI->UIOnFrame();
	m_pHUDCursor->CursorOnFrame();
}
//--------------------------------------------------------------------

ENGINE_API extern float psHUD_FOV;

void CHUDManager::Render_First()
{
	if (!psHUD_Flags.is(HUD_WEAPON|HUD_WEAPON_RT))return;
	if (0==pUI)						return;
	CObject*	O					= g_pGameLevel->CurrentViewEntity();
	if (0==O)						return;
	CActor*		A					= smart_cast<CActor*> (O);
	if (!A)							return;
	if (A && !A->HUDview())			return;

	// only shadow 
	::Render->set_Invisible			(TRUE);
	::Render->set_Object			(O->H_Root());
	O->renderable_Render			();
	::Render->set_Invisible			(FALSE);
}

void CHUDManager::Render_Last()
{
	if (!psHUD_Flags.is(HUD_WEAPON|HUD_WEAPON_RT))return;
	if (0==pUI)						return;
	CObject*	O					= g_pGameLevel->CurrentViewEntity();
	if (0==O)						return;
	CActor*		A					= smart_cast<CActor*> (O);
	if (A && !A->HUDview())			return;
//	CCar*		C					= smart_cast<CCar*>	(O);
//	if (C)							return;
	if(O->CLS_ID == CLSID_CAR)
		return;

//	CSpectator*	S					= smart_cast<CSpectator*>	(O);
//	if (S)							return;
	if(O->CLS_ID == CLSID_SPECTATOR)
		return;

	// hud itself
	::Render->set_HUD				(TRUE);
	::Render->set_Object			(O->H_Root());
	O->OnHUDDraw					(this);
	::Render->set_HUD				(FALSE);
}

//отрисовка элементов интерфейса
void  CHUDManager::RenderUI()
{
	BOOL bAlready					= FALSE;
	if (psHUD_Flags.test(HUD_DRAW))
	{
		// draw hit marker
		HitMarker.Render			();

		// UI
		bAlready					= ! (pUI && !pUI->Render());
		//Font
		Font().Render();

		//render UI cursor
//		if(pUI && GetUICursor() && GetUICursor()->IsVisible())
//			GetUICursor()->Render();
	}
	if (psHUD_Flags.is(HUD_CROSSHAIR|HUD_CROSSHAIR_RT) && !bAlready)	
		m_pHUDCursor->Render();

	// Recalc new scale factor if resolution was changed
//	OnDeviceCreate();
}

//--------------------------------------------------------------------
void CHUDManager::OnEvent(EVENT E, u64 P1, u64 P2)
{
}
//--------------------------------------------------------------------
void __cdecl CHUDManager::outMessage(u32 C, LPCSTR from, LPCSTR msg, ...)
{
	char		buffer	[256];

	va_list		p;
	va_start	(p,msg);
	vsprintf	(buffer,msg,p);
	R_ASSERT	(xr_strlen(buffer)<256);
	va_end		(p);

	GetUI()->AddMessage	(from,buffer,C);
	Msg			("- %s: %s",from,buffer);
}
void __cdecl CHUDManager::outMessage(u32 C, const shared_str& from, LPCSTR msg, ...)
{
	string256	buffer;

	va_list		p;
	va_start	(p,msg);
	_vsnprintf	(buffer,sizeof(buffer)-1,msg,p); buffer[sizeof(buffer)-1] = 0;
	va_end		(p);

	GetUI()->AddMessage	(*from,buffer,C);
	Msg			("- %s: %s",from,buffer);
}

collide::rq_result&	CHUDManager::GetCurrentRayQuery	() 
{
	return m_pHUDCursor->RQ;
}

void CHUDManager::SetCrosshairDisp	(float disp)
{	
	m_pHUDCursor->HUDCrosshair.SetDispersion(psHUD_Flags.test(HUD_CROSSHAIR_DYNAMIC) ? disp : 0.f);
}

void  CHUDManager::ShowCrosshair	(bool show)
{
	m_pHUDCursor->m_bShowCrosshair = show;
}

//////////////////////////////////////////////////////////////////////////

