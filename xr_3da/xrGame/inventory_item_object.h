////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_item_object.h
//	Created 	: 24.03.2003
//  Modified 	: 27.12.2004
//	Author		: Victor Reutsky, Yuri Dobronravin
//	Description : Inventory item object implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "physic_item.h"
#include "inventory_item.h"

class CInventoryItemObject : 
			public CInventoryItem, 
			public CPhysicItem
{
public:
							CInventoryItemObject	();
	virtual					~CInventoryItemObject	();
	virtual DLL_Pure		*_construct				();

public:
	virtual CInventoryItem	*cast_inventory_item()	{return this;}
	virtual CWeapon			*cast_weapon		()	{return 0;}
	virtual CFoodItem		*cast_food_item		()	{return 0;}
	virtual CMissile		*cast_missile		()	{return 0;}
	virtual CHudItem		*cast_hud_item		()	{return 0;}
	virtual CWeaponAmmo		*cast_weapon_ammo	()	{return 0;}

public:
	virtual void	Load					(LPCSTR section);
	virtual LPCSTR	Name					();
	virtual LPCSTR	NameShort				();
	virtual LPCSTR	NameComplex				();
	virtual	void	Hit						(	float P, Fvector &dir,	
												CObject* who, s16 element,
												Fvector position_in_object_space, 
												float impulse, 
												ALife::EHitType hit_type = ALife::eHitTypeWound);

	virtual void	OnH_B_Independent		();
	virtual void	OnH_B_Chield			();
	virtual void	OnH_A_Chield			();
	virtual void	UpdateCL				();
	virtual void	OnEvent					(NET_Packet& P, u16 type);
	virtual BOOL	net_Spawn				(CSE_Abstract* DC);
	virtual void	net_Destroy				();
	virtual void	net_Import				(NET_Packet& P);					// import from server
	virtual void	net_Export				(NET_Packet& P);					// export to server
	virtual void	save					(NET_Packet &output_packet);
	virtual void	load					(IReader &input_packet);
	virtual BOOL	net_SaveRelevant		()								{return TRUE;}
	virtual void	renderable_Render		();
	virtual void	reload					(LPCSTR section);
	virtual void	reinit					();
	virtual void	activate_physic_shell	();
	virtual void	on_activate_physic_shell();
public:
	////////// network //////////////////////////////////////////////////
	virtual void	make_Interpolation		();
	virtual void	PH_B_CrPr				(); // actions & operations before physic correction-prediction steps
	virtual void	PH_I_CrPr				(); // actions & operations after correction before prediction steps
#ifdef DEBUG
	virtual void	PH_Ch_CrPr				(); // 
#endif
	virtual void	PH_A_CrPr				(); // actions & operations after phisic correction-prediction steps

protected:
#ifdef DEBUG
	virtual void	OnRender				();
#endif
};

#include "inventory_item_inline.h"