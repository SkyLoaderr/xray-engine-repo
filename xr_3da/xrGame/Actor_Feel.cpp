#include "stdafx.h"
#include "actor.h"
#include "weapon.h"
#include "targetassault.h"
#include "mercuryball.h"
#include "targetcsbase.h"
#include "targetcs.h"
#include "targetcscask.h"
#include "inventory.h"
#include "hudmanager.h"

#define PICKUP_INFO_COLOR 0xFFAAAAAA

void CActor::feel_touch_new				(CObject* O)
{
}

void CActor::feel_touch_delete	(CObject* O)
{
}

BOOL CActor::feel_touch_contact	(CObject* O)
{	
	CInventoryItem* item = dynamic_cast<CInventoryItem*>(O);
	CInventoryOwner* inventory_owner = dynamic_cast<CInventoryOwner*>(O);
	if(item && item->Useful() && !item->H_Parent()) 
		return TRUE;
	else if(inventory_owner && inventory_owner != dynamic_cast<CInventoryOwner*>(this))
		return TRUE;
	else
		return FALSE;
}


void CActor::PickupModeOn()
{
	m_bPickupMode = true;
}

void CActor::PickupModeOff()
{
	m_bPickupMode = false;
}
void CActor::PickupModeUpdate()
{
	if(!m_bPickupMode) return;

	if(inventory().m_pTarget)
	{
		inventory().Action(kUSE, CMD_START);
	}

	feel_touch_update(Position(), /*inventory().GetTakeDist()*/m_fPickupInfoRadius);

	for(xr_vector<CObject*>::iterator it = feel_touch.begin();
							it != feel_touch.end(); it++)
	{
		PickupInfoDraw(*it);
	}
}

void CActor::PickupInfoDraw(CObject* object)
{
	LPCSTR draw_str = NULL;
	
	CInventoryItem* item = dynamic_cast<CInventoryItem*>(object);
	CInventoryOwner* inventory_owner = dynamic_cast<CInventoryOwner*>(object);
	
	VERIFY(item || inventory_owner);

	Fmatrix res;
	res.mul(Device.mFullTransform,object->XFORM());

	Fvector4 v_res;

	Fvector shift;

	if(item) 
	{
		draw_str = item->NameComplex();
		shift.set(0,0,0);
	}
	else if(inventory_owner)
	{
		draw_str = inventory_owner->GetGameName();
		shift.set(0,1.2f,0);
	}


	res.transform(v_res,shift);

	if (v_res.z < 0 || v_res.w < 0)	return;
	if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f) return;

	float x = (1.f + v_res.x)/2.f * (Device.dwWidth);
	float y = (1.f - v_res.y)/2.f * (Device.dwHeight);


	HUD().pFontMedium->SetAligment(CGameFont::alCenter);
	HUD().pFontMedium->SetColor(PICKUP_INFO_COLOR);
	HUD().pFontMedium->OutSet(x,y);
	HUD().pFontMedium->OutNext(draw_str);
}