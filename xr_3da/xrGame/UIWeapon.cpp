#include "stdafx.h"
#include "uiweapon.h"
#include "hudmanager.h"
#include "weapon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------
CUIWeapon::CUIWeapon()
{
	cur_shader	= 0;
}
//--------------------------------------------------------------------

void CUIWeapon::Init(){
	Level().HUD()->ClientToScreen(694.f, 525.f, position, alRight|alBottom);
	// back frame with tex
	back.Init		("ui\\hud_weapon_back","font",694.f,525.f,128,64,alRight|alBottom);
	back.SetRect	(0,0,90,61);
	// weapon frame
	weapon.Init		(697,540,128,32,alRight|alBottom);
	weapon.SetRect	(0,0,90,32);
}
//--------------------------------------------------------------------

CUIWeapon::~CUIWeapon()
{
}
//--------------------------------------------------------------------

void CUIWeapon::Render()
{
	back.Render		();
	if (cur_shader) 
		weapon.Render(cur_shader);
}
//--------------------------------------------------------------------

void CUIWeapon::Out(CWeapon* W){
	if (W){
		float sc	= Level().HUD()->GetScale();
		CFontHUD* H	= Level().HUD()->pHUDFont;
		H->Color	(0xffffffff);
		H->Out		(position.x+6,	position.y+15,"%s",W->GetName());
		H->Out		(position.x+3,	position.y+2,"%d/%d",W->GetAmmoElapsed(),W->GetAmmoLimit());
		H->Out		(position.x+34,	position.y+50,"%d/%d",0,0);
		cur_shader	= W->GetUIIcon();
	}else{
		cur_shader	= 0;
	}
}
