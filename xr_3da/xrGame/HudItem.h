//////////////////////////////////////////////////////////////////////
// HudItem.h: класс предок для всех предметов имеющих
//			  собственный HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Inventory.h"


class CWeaponHUD;

class CHudItem: virtual public CInventoryItem
{
private:
	typedef CInventoryItem inherited;
protected: //чтоб нельзя было вызвать на прямую
	CHudItem(void);
	virtual ~CHudItem(void);
public:
	void		Load				(LPCSTR section);

	IC void			SetHUDmode			(BOOL H)		{	hud_mode = H;								}
	IC BOOL			GetHUDmode			()				{	return hud_mode;							}
	
	virtual bool	IsPending			()				{   return m_bPending;}

	void		renderable_Render	();

protected:
	//TRUE - оружие занято, выполнением некоторого действия
	bool					m_bPending;

	CWeaponHUD*				m_pHUD;
	BOOL					hud_mode;
	ref_str					hud_sect;

	//кадры момента пересчета XFORM и FirePos
	u32						dwFP_Frame;
	u32						dwXF_Frame;

};