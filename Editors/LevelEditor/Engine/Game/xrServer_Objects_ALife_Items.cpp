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
	#include "bone.h"
#else
	#include "..\bone.h"
#endif

#ifdef _EDITOR
	#include "clsid_game.h"
    #include "skeletoncustom.h"
#endif

#include "xrServer_Objects_ALife_Items.h"

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeInventoryItem
////////////////////////////////////////////////////////////////////////////
CSE_ALifeInventoryItem::CSE_ALifeInventoryItem(LPCSTR caSection) : CSE_Abstract(caSection)
{
	//текущее состояние вещи
	m_fCondition				= 1.0f;

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
	for (int i=0; i<m_iGridHeight; ++i)
		m_qwGridBitMask			|= ((u64(1) << m_iGridWidth) - 1) << (i*RUCK_WIDTH);

	m_tPreviousParentID			= 0xffff;
	m_can_switch_offline		= true;
}

CSE_ALifeInventoryItem::~CSE_ALifeInventoryItem	()
{
}

void CSE_ALifeInventoryItem::STATE_Write	(NET_Packet &tNetPacket)
{
	tNetPacket.w_float			(m_fCondition);
}

void CSE_ALifeInventoryItem::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	if (m_wVersion > 52)
		tNetPacket.r_float		(m_fCondition);

	State.position				= o_Position;
}

void CSE_ALifeInventoryItem::UPDATE_Write	(NET_Packet &tNetPacket)
{
	tNetPacket.w_float				(m_fCondition);
	tNetPacket.w_u32				(m_dwTimeStamp);
	tNetPacket.w_u16				(m_u16NumItems);
	tNetPacket.w_vec3				( State.position );

	if (!m_u16NumItems)
		return;	

	tNetPacket.w_u8					( State.enabled );

	tNetPacket.w_vec3				( State.angular_vel );
	tNetPacket.w_vec3				( State.linear_vel );

	tNetPacket.w_vec3				( State.force );
	tNetPacket.w_vec3				( State.torque );


	tNetPacket.w_float				( State.quaternion.x );
	tNetPacket.w_float				( State.quaternion.y );
	tNetPacket.w_float				( State.quaternion.z );
	tNetPacket.w_float				( State.quaternion.w );	

	o_Position						= State.position;
};

void CSE_ALifeInventoryItem::UPDATE_Read	(NET_Packet &tNetPacket)
{
	tNetPacket.r_float				(m_fCondition);
	tNetPacket.r_u32				(m_dwTimeStamp);
	tNetPacket.r_u16				(m_u16NumItems);
	tNetPacket.r_vec3				( State.position );

	m_can_switch_offline			= true;
	o_Position						= State.position;

	if (!m_u16NumItems)
		return;

	tNetPacket.r_u8					( *((u8*)&(State.enabled)) );

	tNetPacket.r_vec3				( State.angular_vel );
	tNetPacket.r_vec3				( State.linear_vel );

	tNetPacket.r_vec3				( State.force );
	tNetPacket.r_vec3				( State.torque );

	tNetPacket.r_float				( State.quaternion.x );
	tNetPacket.r_float				( State.quaternion.y );
	tNetPacket.r_float				( State.quaternion.z );
	tNetPacket.r_float				( State.quaternion.w );	
};

bool CSE_ALifeInventoryItem::can_switch_offline	() const
{
	return							(m_can_switch_offline);
}

#ifdef _EDITOR
void CSE_ALifeInventoryItem::FillProp		(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,	 values);
	PHelper.CreateFloat			(values, FHelper.PrepareKey(pref, s_name, "Item condition"), &m_fCondition, 0.f, 1.f);
}
#endif

bool CSE_ALifeInventoryItem::bfUseful		()
{
	return						(true);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItem
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItem::CSE_ALifeItem				(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_ALifeInventoryItem(caSection), CSE_Abstract(caSection)
{
}

CSE_ALifeItem::~CSE_ALifeItem				()
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

bool CSE_ALifeItem::can_switch_offline	() const
{
	return						(inherited1::can_switch_offline() && inherited2::can_switch_offline());
}

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
/*	strcpy						(spot_texture,"");
	strcpy						(animator,"");
	spot_range					= 10.f;
	spot_cone_angle				= PI_DIV_3;
	color						= 0xffffffff;
    spot_brightness				= 1.f;
	glow_texture[0]				= 0;
	glow_radius					= 0.1f;
    guid_bone					= u32(BI_NONE);
*/
	m_active					= false;
}

CSE_ALifeItemTorch::~CSE_ALifeItemTorch		()
{
}

void CSE_ALifeItemTorch::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion > 20)
		inherited::STATE_Read	(tNetPacket,size);
/*	
	tNetPacket.r_u32			(color);
	tNetPacket.r_string			(animator);
	tNetPacket.r_string			(spot_texture);
	tNetPacket.r_float			(spot_range);
	tNetPacket.r_angle8			(spot_cone_angle);
	tNetPacket.r_float			(spot_brightness);

	if (m_wVersion > 40){
		tNetPacket.r_string		(glow_texture);
		tNetPacket.r_float		(glow_radius);
	}
	if (m_wVersion > 41){
		tNetPacket.r_u16		(guid_bone);
	}
*/
}

void CSE_ALifeItemTorch::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
/*
	tNetPacket.w_u32			(color);
	tNetPacket.w_string			(animator);
	tNetPacket.w_string			(spot_texture);
	tNetPacket.w_float			(spot_range);
	tNetPacket.w_angle8			(spot_cone_angle);
	tNetPacket.w_float			(spot_brightness);
	tNetPacket.w_string			(glow_texture);
	tNetPacket.w_float			(glow_radius);
    tNetPacket.w_u16			(guid_bone);
*/
}

void CSE_ALifeItemTorch::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	m_active					= !!tNetPacket.r_u8();
}

void CSE_ALifeItemTorch::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	tNetPacket.w_u8				(m_active ? 1 : 0);
}

#ifdef _EDITOR
void CSE_ALifeItemTorch::FillProp			(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,	 values);
/*
	PHelper.CreateColor			(values, FHelper.PrepareKey(pref,s_name,"Color"),			&color);
	PHelper.CreateChoose		(values, FHelper.PrepareKey(pref,s_name,"Color animator"),	animator,			sizeof(animator), 		smLAnim);
	PHelper.CreateChoose		(values, FHelper.PrepareKey(pref,s_name,"Spot texture"),	spot_texture,		sizeof(spot_texture),	smTexture);
	PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Range"),			&spot_range,		0.1f, 1000.f);
	PHelper.CreateAngle			(values, FHelper.PrepareKey(pref,s_name,"Angle"),			&spot_cone_angle,	0, PI_DIV_2);
    PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Brightness"),		&spot_brightness,	0.1f, 5.f);
	PHelper.CreateChoose		(values, FHelper.PrepareKey(pref,s_name,"Glow texture"),	glow_texture,		sizeof(glow_texture), 	smTexture);
	PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Glow radius"),		&glow_radius,		0.1f, 1000.f);
    // bones
    if (visual && PKinematics(visual))
    {
        AStringVec				vec;
        u16 cnt					= PKinematics(visual)->LL_Bones()->size();
        for (u16 k=0; k<cnt; k++) vec.push_back(PKinematics(visual)->LL_BoneName_dbg(k));
		PHelper.CreateToken2<u16>(values, FHelper.PrepareKey(pref,s_name,"Guide bone"),		&guid_bone,	&vec);
    }
*/
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
	ammo_type					= 0;

	m_fHitPower					= pSettings->r_float(caSection,"hit_power");
	m_tHitType					= ALife::g_tfString2HitType(pSettings->r_string(caSection,"hit_type"));
	m_caAmmoSections			= pSettings->r_string(caSection,"ammo_class");
	m_dwSlot					= pSettings->r_u32(caSection,"slot");
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
        set_visual				(pSettings->r_string(caSection,"visual"));

	m_addon_flags.zero			();
}

CSE_ALifeItemWeapon::~CSE_ALifeItemWeapon	()
{
}

void CSE_ALifeItemWeapon::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);

	tNetPacket.r_u8				(flags);

	tNetPacket.r_u16			(a_elapsed);

	tNetPacket.r_u8				(m_addon_flags.flags);
	tNetPacket.r_u8				(ammo_type);
	tNetPacket.r_u8				(state);
	tNetPacket.r_u8				(m_bZoom);
}

void CSE_ALifeItemWeapon::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);

	tNetPacket.w_u8				(flags);

	tNetPacket.w_u16			(a_elapsed);

	tNetPacket.w_u8				(m_addon_flags.get());
	tNetPacket.w_u8				(ammo_type);
	tNetPacket.w_u8				(state);
	tNetPacket.w_u8				(m_bZoom);
}

void CSE_ALifeItemWeapon::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
	tNetPacket.r_u16			(a_current);
	tNetPacket.r_u16			(a_elapsed);
	tNetPacket.r_u8				(state);
	
	if (m_wVersion > 40)
		tNetPacket.r_u8			(m_addon_flags.flags);

	if (m_wVersion > 46)
		tNetPacket.r_u8			(ammo_type);
}

void CSE_ALifeItemWeapon::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u16			(a_current);
	tNetPacket.w_u16			(a_elapsed);
	tNetPacket.w_u8				(state);
	tNetPacket.w_u8				(m_addon_flags.get());
	tNetPacket.w_u8				(ammo_type);
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
	PHelper.CreateU8			(items,FHelper.PrepareKey(pref,s_name,"Ammo type:"), &ammo_type,0,255,1);
	PHelper.CreateU16			(items,FHelper.PrepareKey(pref,s_name,"Ammo: in magazine"),	&a_elapsed,0,30,1);
	

	if ((EWeaponAddonStatus)pSettings->r_s32(s_name,"scope_status") == eAddonAttachable)
	       PHelper.CreateFlag<Flags8>(items,FHelper.PrepareKey(pref,s_name,"Addons\\Scope"), 	&m_addon_flags, eWeaponAddonScope);

	if ((EWeaponAddonStatus)pSettings->r_s32(s_name,"silencer_status") == eAddonAttachable)
        PHelper.CreateFlag<Flags8>(items,FHelper.PrepareKey(pref,s_name,"Addons\\Silencer"), 	&m_addon_flags, eWeaponAddonSilencer);

	if ((EWeaponAddonStatus)pSettings->r_s32(s_name,"grenade_launcher_status") == eAddonAttachable)
        PHelper.CreateFlag<Flags8>(items,FHelper.PrepareKey(pref,s_name,"Addons\\Podstvolnik"),&m_addon_flags,eWeaponAddonGrenadeLauncher);
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

CSE_ALifeItemAmmo::~CSE_ALifeItemAmmo		()
{
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
// CSE_ALifeItemDetector
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemDetector::CSE_ALifeItemDetector(LPCSTR caSection) : CSE_ALifeItem(caSection), CSE_Abstract(caSection)
{
}

CSE_ALifeItemDetector::~CSE_ALifeItemDetector()
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

CSE_ALifeItemArtefact::~CSE_ALifeItemArtefact()
{
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
CSE_ALifeItemPDA::CSE_ALifeItemPDA		(LPCSTR caSection) : CSE_ALifeItem(caSection), CSE_Abstract(caSection)
{
}

CSE_ALifeItemPDA::~CSE_ALifeItemPDA		()
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

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemDocument
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemDocument::CSE_ALifeItemDocument(LPCSTR caSection): CSE_ALifeItem(caSection), CSE_Abstract(caSection)
{
	m_wDocIndex					= 0;
}

CSE_ALifeItemDocument::~CSE_ALifeItemDocument()
{
}

void CSE_ALifeItemDocument::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
	tNetPacket.r_u16			(m_wDocIndex);
}

void CSE_ALifeItemDocument::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u16			(m_wDocIndex);
}

void CSE_ALifeItemDocument::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemDocument::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeItemDocument::FillProp		(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref,items);
	PHelper.CreateU16			(items, FHelper.PrepareKey(pref, s_name, "Document index :"), &m_wDocIndex, 0, 65535);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemGrenade
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemGrenade::CSE_ALifeItemGrenade	(LPCSTR caSection): CSE_ALifeItem(caSection), CSE_Abstract(caSection)
{
	m_dwTimeStamp = 0;
}

CSE_ALifeItemGrenade::~CSE_ALifeItemGrenade	()
{
}

void CSE_ALifeItemGrenade::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CSE_ALifeItemGrenade::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemGrenade::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemGrenade::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeItemGrenade::FillProp			(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref,items);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemExplosive
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemExplosive::CSE_ALifeItemExplosive	(LPCSTR caSection): CSE_ALifeItem(caSection), CSE_Abstract(caSection)
{
}

CSE_ALifeItemExplosive::~CSE_ALifeItemExplosive	()
{
}

void CSE_ALifeItemExplosive::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CSE_ALifeItemExplosive::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemExplosive::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeItemExplosive::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeItemExplosive::FillProp			(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref,items);
}
#endif


////////////////////////////////////////////////////////////////////////////
// CSE_ALifeItemBolt
////////////////////////////////////////////////////////////////////////////
CSE_ALifeItemBolt::CSE_ALifeItemBolt		(LPCSTR caSection) : CSE_ALifeDynamicObject(caSection), CSE_ALifeInventoryItem(caSection), CSE_Abstract(caSection)
{
}

CSE_ALifeItemBolt::~CSE_ALifeItemBolt		()
{
}

void CSE_ALifeItemBolt::STATE_Write			(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
}

void CSE_ALifeItemBolt::STATE_Read			(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	if (m_wVersion > 50)
		inherited2::STATE_Read	(tNetPacket, size);
}

void CSE_ALifeItemBolt::UPDATE_Write		(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
};

void CSE_ALifeItemBolt::UPDATE_Read			(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
};

bool CSE_ALifeItemBolt::can_switch_offline	() const
{
	return						(inherited1::can_switch_offline() && inherited2::can_switch_offline());
}

bool CSE_ALifeItemBolt::can_save			() const
{
	return						(false);//!bfAttached());
}

#ifdef _EDITOR
void CSE_ALifeItemBolt::FillProp			(LPCSTR pref, PropItemVec& values)
{
	inherited1::FillProp		(pref,	 values);
	inherited2::FillProp		(pref,	 values);
}
#endif