////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Retsky and Dmitriy Iassenev
//	Description : Server objects items for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALife_ItemsH
#define xrServer_Objects_ALife_ItemsH

#include "xrServer_Objects_ALife.h"

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItem,CSE_ALifeDynamicObjectVisual)
	float							m_fMass;
	u32								m_dwCost;
	s32								m_iHealthValue;
	
									CSE_ALifeItem	(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_Abstract(caSection)
	{
		m_fMass						= pSettings->r_float(caSection, "inv_weight");
		m_dwCost					= pSettings->r_u32(caSection, "cost");
		if (pSettings->line_exist(caSection, "health_value"))
			m_iHealthValue			= pSettings->r_s32(caSection, "health_value");
		else
			m_iHealthValue			= 0;
	};

	bool							bfAttached		()
	{
		return						(ID < 0xffff);
	};
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemTorch,CSE_ALifeItem)
	u32								color;
	string64						animator;
	string64						spot_texture;
	float							spot_range;
	float							spot_cone_angle;
    float							spot_brightness;
									CSE_ALifeItemTorch	(LPCSTR caSection);
    virtual							~CSE_ALifeItemTorch();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemAmmo,CSE_ALifeItem)
	u16								a_elapsed;
	u16								m_boxSize;
							
									CSE_ALifeItemAmmo	(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemWeapon,CSE_ALifeItem)
	u32								timestamp;
	u8								flags;
	u8								state;

	u16								a_current;
	u16								a_elapsed;

									CSE_ALifeItemWeapon(LPCSTR caSection);

	virtual void					OnEvent			(NET_Packet& P, u16 type, u32 time, u32 sender );

	u8								get_slot		();
	u16								get_ammo_limit	();
	u16								get_ammo_total	();
	u16								get_ammo_elapsed();
	u16								get_ammo_magsize();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemCar,CSE_ALifeItem)
									CSE_ALifeItemCar(LPCSTR caSection) : CSE_ALifeItem(caSection), CSE_Abstract(caSection)
	{
		if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
    	    set_visual				(pSettings->r_string(caSection,"visual"));
	};
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemDetector,CSE_ALifeItem)
									CSE_ALifeItemDetector(LPCSTR caSection) : CSE_ALifeItem(caSection), CSE_Abstract(caSection)
	{
	};
SERVER_ENTITY_DECLARE_END

#endif