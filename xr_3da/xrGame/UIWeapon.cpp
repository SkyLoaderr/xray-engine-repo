#include "stdafx.h"
#include "uiweapon.h"
#include "hudmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUIWeaponItem::CUIWeaponItem(LPCSTR tex_name){
	Init		(tex_name,"font",697,540,128,32,alRight|alBottom);
	SetRect		(0,0,90,32);
}

CUIWeaponItem::~CUIWeaponItem(){
}

//--------------------------------------------------------------------
CUIWeapon::CUIWeapon()
{
}
//--------------------------------------------------------------------

void CUIWeapon::Init(){
	Level().HUD()->ClientToScreen(694.f, 525.f, position, alRight|alBottom);
	back.Init	("ui\\hud_weapon_back","font",694.f,525.f,128,64,alRight|alBottom);
	back.SetRect(0,0,90,61);
	weapons["M134"]	= new CUIWeaponItem("ui\\hud_kulemet");
	current		= weapons["M134"];
}
//--------------------------------------------------------------------

CUIWeapon::~CUIWeapon()
{
	for (WItmIt it=weapons.begin(); it!=weapons.end(); it++)
		_DELETE(it->second);
}
//--------------------------------------------------------------------

void CUIWeapon::Render()
{
	back.Render	();
	current->Render	();
}
//--------------------------------------------------------------------

void CUIWeapon::Out(LPCSTR wpn_name, int ammo1, int ammo1_total, int ammo2, int ammo2_total){
	current		= weapons[wpn_name];
	float sc	= Level().HUD()->GetScale();
	CFontHUD* H	= Level().HUD()->pHUDFont;
	H->Color	(0xffffffff);
	H->Out		(position.x+6,	position.y+15,"%s",wpn_name);
	H->Out		(position.x+3,	position.y+2,"%d/%d",ammo1,ammo1_total);
	H->Out		(position.x+34,	position.y+50,"%d/%d",ammo2,ammo2_total);
}
