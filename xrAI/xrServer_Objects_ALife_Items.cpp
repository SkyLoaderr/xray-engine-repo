////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Items.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects items for ALife simulator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#ifdef AI_COMPILER
	#include "net_utils.h"
	#include "clsid_game.h"
#endif

#ifdef _EDITOR
	#include "clsid_game.h"
#endif

#include "xrServer_Objects_ALife_Items.h"


////////////////////////////////////////////////////////////////////////////
// CSE_ALifeInventoryItem
////////////////////////////////////////////////////////////////////////////
CSE_ALifeInventoryItem::CSE_ALifeInventoryItem(LPCSTR caSection) : CSE_Abstract(caSection)
{
	m_fMass						= pSettings->r_float(caSection, "inv_weight");
	m_dwCost					= pSettings->r_u32(caSection, "cost");

	if (pSettings->line_exist(caSection, "health_value"))
		m_iHealthValue			= pSettings->r_s32(caSection, "health_value");
	else
		m_iHealthValue			= 0;

	if (pSettings->line_exist(caSection, "food_value"))
		m_iFoodValue			= pSettings->r_s32(caSection, "food_value");
	else
		m_iFoodValue			= 0;

	m_iGridWidth				= pSettings->r_s32(caSection, "inv_grid_width");
	m_iGridHeight				= pSettings->r_s32(caSection, "inv_grid_height");
	R_ASSERT2					((m_iGridWidth <= RUCK_WIDTH) && (m_iGridHeight <= RUCK_HEIGHT),"Invalid inventory grid sizes");
	m_iVolume					= m_iGridWidth*m_iGridHeight;
	m_fDeteriorationValue		= 0;
	m_qwGridBitMask				= 0;
	for (int i=0; i<m_iGridHeight; i++)
		m_qwGridBitMask			|= ((u64(1) << m_iGridWidth) - 1) << (i*RUCK_WIDTH);

	m_tPreviousParentID			= 0xffff;
}

void CSE_ALifeInventoryItem::STATE_Write	(NET_Packet &tNetPacket)
{
}

void CSE_ALifeInventoryItem::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
}

void CSE_ALifeInventoryItem::UPDATE_Write	(NET_Packet &tNetPacket)
{
};

void CSE_ALifeInventoryItem::UPDATE_Read	(NET_Packet &tNetPacket)
{
};

#ifdef _EDITOR
void CSE_ALifeInventoryItem::FillProp		(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,	 values);
}
#endif

bool CSE_ALifeInventoryItem::bfUseful		()
{
	return						(true);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItem
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItem::CSE_ALifeItem(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_ALifeInventoryItem(caSection), CSE_Abstract(caSection)
{
}

void CSE_ALifeItem::STATE_Write				(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
}

void CSE_ALifeItem::STATE_Read				(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	if ((m_tClassID == CLSID_OBJECT_W_BINOCULAR) && (m_wVersion < 37)) {
		tNetPacket.r_u16		();
		tNetPacket.r_u16		();
		tNetPacket.r_u8			();
	}
	inherited2::STATE_Read		(tNetPacket, size);
}

void CSE_ALifeItem::UPDATE_Write			(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
};

void CSE_ALifeItem::UPDATE_Read				(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
};

#ifdef _EDITOR
void CSE_ALifeItem::FillProp				(LPCSTR pref, PropItemVec& values)
{
	inherited1::FillProp		(pref,	 values);
	inherited2::FillProp		(pref,	 values);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemTorch
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemTorch::CSE_ALifeItemTorch		(LPCSTR caSection) : CSE_ALifeItem(caSection), CSE_Abstract(caSection)
{
	strcpy						(spot_texture,"");
	strcpy						(animator,"");
	spot_range					= 10.f;
	spot_cone_angle				= PI_DIV_3;
	color						= 0xffffffff;
    spot_brightness				= 1.f;
	set_visual					("lights\\lights_torch");
}

CSE_ALifeItemTorch::~CSE_ALifeItemTorch		()
{
}

void CSE_ALifeItemTorch::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion > 20)
		inherited::STATE_Read	(tNetPacket,size);
	
	tNetPacket.r_u32			(color);
	tNetPacket.r_string			(animator);
	tNetPacket.r_string			(spot_texture);
	tNetPacket.r_float			(spot_range);
	tNetPacket.r_angle8			(spot_cone_angle);
	tNetPacket.r_float			(spot_brightness);
}

void CSE_ALifeItemTorch::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u32			(color);
	tNetPacket.w_string			(animator);
	tNetPacket.w_string			(spot_texture);
	tNetPacket.w_float			(spot_range);
	tNetPacket.w_angle8			(spot_cone_angle);
	tNetPacket.w_float			(spot_brightness);
}

void CSE_ALifeItemTorch::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemTorch::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeItemTorch::FillProp			(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,	 values);
	PHelper.CreateColor			(values, FHelper.PrepareKey(pref,s_name,"Color"),			&color);
	PHelper.CreateLightAnim		(values, FHelper.PrepareKey(pref,s_name,"Color animator"),	animator,			sizeof(animator));
	PHelper.CreateTexture		(values, FHelper.PrepareKey(pref,s_name,"Texture"),			spot_texture,		sizeof(spot_texture));
	PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Range"),			&spot_range,		0.1f, 1000.f);
	PHelper.CreateAngle			(values, FHelper.PrepareKey(pref,s_name,"Angle"),			&spot_cone_angle,	0, PI_DIV_2);
    PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Brightness"),		&spot_brightness,	0.1f, 5.f);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemWeapon
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemWeapon::CSE_ALifeItemWeapon	(LPCSTR caSection) : CSE_ALifeItem(caSection), CSE_Abstract(caSection)
{
	a_current					= 90;
	a_elapsed					= 0;
	state						= 0;
	m_fHitPower					= pSettings->r_float(caSection,"hit_power");
	m_tHitType					= g_tfString2HitType(pSettings->r_string(caSection,"hit_type"));
	m_caAmmoSections			= pSettings->r_string(caSection,"ammo_class");
	m_dwSlot					= pSettings->r_u32(caSection,"slot");
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
        set_visual				(pSettings->r_string(caSection,"visual"));
}

void CSE_ALifeItemWeapon::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	tNetPacket.r_u32			(timestamp);
	tNetPacket.r_u8				(flags);

	tNetPacket.r_u16			(a_current);
	tNetPacket.r_u16			(a_elapsed);

	tNetPacket.r_vec3			(o_Position	);
	tNetPacket.r_angle8			(o_Angle.x	);
	tNetPacket.r_angle8			(o_Angle.y	);
	tNetPacket.r_angle8			(o_Angle.z	);
}

void CSE_ALifeItemWeapon::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	tNetPacket.w_u32			(timestamp);
	tNetPacket.w_u8				(flags);

	tNetPacket.w_u16			(a_current);
	tNetPacket.w_u16			(a_elapsed);

	tNetPacket.w_vec3			(o_Position	);
	tNetPacket.w_angle8			(o_Angle.x	);
	tNetPacket.w_angle8			(o_Angle.y	);
	tNetPacket.w_angle8			(o_Angle.z	);
}

void CSE_ALifeItemWeapon::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
	tNetPacket.r_u16			(a_current);
	tNetPacket.r_u16			(a_elapsed);
	tNetPacket.r_u8				(state);
}

void CSE_ALifeItemWeapon::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u16			(a_current);
	tNetPacket.w_u16			(a_elapsed);
	tNetPacket.w_u8				(state);
}

void CSE_ALifeItemWeapon::OnEvent			(NET_Packet	&tNetPacket, u16 type, u32 time, u32 sender )
{
	inherited::OnEvent			(tNetPacket,type,time,sender);
	switch (type) {
		case GE_WPN_STATE_CHANGE:
			tNetPacket.r_u8	(state);
			break;
	}
}

u8	 CSE_ALifeItemWeapon::get_slot			()
{
	return						((u8)pSettings->r_u8(s_name,"slot"));
}

u16	 CSE_ALifeItemWeapon::get_ammo_limit	()
{
	return						(u16) pSettings->r_u16(s_name,"ammo_limit");
}

u16	 CSE_ALifeItemWeapon::get_ammo_total	()
{
	return						((u16)a_current);
}

u16	 CSE_ALifeItemWeapon::get_ammo_elapsed	()
{
	return						((u16)a_elapsed);
}

u16	 CSE_ALifeItemWeapon::get_ammo_magsize	()
{
	if (pSettings->line_exist(s_name,"ammo_mag_size"))
		return					(pSettings->r_u16(s_name,"ammo_mag_size"));
	else
		return					0;
}

#ifdef _EDITOR
void CSE_ALifeItemWeapon::FillProp			(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref, items);
	PHelper.CreateU16			(items,FHelper.PrepareKey(pref,s_name,"Ammo: total"),			&a_current,0,1000,1);
	PHelper.CreateU16			(items,FHelper.PrepareKey(pref,s_name,"Ammo: in magazine"),	&a_elapsed,0,30,1);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemAmmo
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemAmmo::CSE_ALifeItemAmmo		(LPCSTR caSection) : CSE_ALifeItem(caSection), CSE_Abstract(caSection)
{
	a_elapsed					= m_boxSize = (u16)pSettings->r_s32(caSection, "box_size");
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
        set_visual				(pSettings->r_string(caSection,"visual"));
}

void CSE_ALifeItemAmmo::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
	tNetPacket.r_u16			(a_elapsed);
}

void CSE_ALifeItemAmmo::STATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u16			(a_elapsed);
}

void CSE_ALifeItemAmmo::UPDATE_Read			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	tNetPacket.r_u16			(a_elapsed);
}

void CSE_ALifeItemAmmo::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	tNetPacket.w_u16			(a_elapsed);
}

#ifdef _EDITOR
void CSE_ALifeItemAmmo::FillProp			(LPCSTR pref, PropItemVec& values) {
  	inherited::FillProp			(pref,values);
	PHelper.CreateU16			(values, FHelper.PrepareKey(pref, s_name, "Ammo: left"), &a_elapsed, 0, m_boxSize, m_boxSize);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemCar
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemCar::CSE_ALifeItemCar(LPCSTR caSection) : CSE_ALifeItem(caSection), CSE_Abstract(caSection)
{
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
    	set_visual				(pSettings->r_string(caSection,"visual"));
};

void CSE_ALifeItemCar::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion > 20)
		inherited::STATE_Read	(tNetPacket,size);
}

void CSE_ALifeItemCar::STATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemCar::UPDATE_Read			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemCar::UPDATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeItemCar::FillProp				(LPCSTR pref, PropItemVec& values)
{
  	inherited::FillProp			(pref,values);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemDetector
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemDetector::CSE_ALifeItemDetector(LPCSTR caSection) : CSE_ALifeItem(caSection), CSE_Abstract(caSection)
{
}

void CSE_ALifeItemDetector::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion > 20)
		inherited::STATE_Read	(tNetPacket,size);
}

void CSE_ALifeItemDetector::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemDetector::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemDetector::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeItemDetector::FillProp		(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProp			(pref,items);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemDetector
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemArtefact::CSE_ALifeItemArtefact(LPCSTR caSection) : CSE_ALifeItem(caSection), CSE_Abstract(caSection)
{
	m_fAnomalyValue				= 100.f;
}

void CSE_ALifeItemArtefact::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CSE_ALifeItemArtefact::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemArtefact::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemArtefact::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeItemArtefact::FillProp		(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref,items);
	PHelper.CreateFloat			(items, FHelper.PrepareKey(pref, s_name, "Anomaly value:"), &m_fAnomalyValue, 0.f, 200.f);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemPDA
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemPDA::CSE_ALifeItemPDA(LPCSTR caSection) : CSE_ALifeItem(caSection), CSE_Abstract(caSection)
{
}

void CSE_ALifeItemPDA::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CSE_ALifeItemPDA::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemPDA::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemPDA::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeItemPDA::FillProp		(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref,items);
}
#endif

