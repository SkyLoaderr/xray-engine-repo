////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects items for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALife_ItemsH
#define xrServer_Objects_ALife_ItemsH

#include "xrServer_Objects_ALife.h"

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItem,CSE_ALifeDynamicObjectVisual)
	float							m_fMass;
	int								m_iVolume;
	u32								m_dwCost;
	s32								m_iHealthValue;
	s32								m_iFoodValue;
	float							m_fDeteriorationValue;
	int								m_iGridWidth;
	int								m_iGridHeight;
	u64								m_qwGridBitMask;

									CSE_ALifeItem	(LPCSTR caSection);
	IC	bool						bfAttached		()
	{
		return						(ID_Parent < 0xffff);
	}
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
	float							m_fHitPower;
	EHitType						m_tHitType;
	LPCSTR							m_caAmmoSections;
	u32								m_dwAmmoAvailable;
	u32								m_dwSlot;

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

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemArtefact,CSE_ALifeItem)
	float							m_fAnomalyValue;

									CSE_ALifeItemArtefact(LPCSTR caSection) : CSE_ALifeItem(caSection), CSE_Abstract(caSection)
	{
		m_fAnomalyValue				= 100.f;
	};
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemPDA,CSE_ALifeItem)

									CSE_ALifeItemPDA(LPCSTR caSection) : CSE_ALifeItem(caSection), CSE_Abstract(caSection)
	{
	};
SERVER_ENTITY_DECLARE_END

#endif