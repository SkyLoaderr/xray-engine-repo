////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Items.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Retsky and Dmitriy Iassenev
//	Description : Server objects items for ALife simulator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Items.h"

////////////////////////////////////////////////////////////////////////////
// CALifeItem
////////////////////////////////////////////////////////////////////////////
void CALifeItem::STATE_Write				(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifeItem::STATE_Read					(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
}

void CALifeItem::UPDATE_Write				(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
};

void CALifeItem::UPDATE_Read				(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
};

////////////////////////////////////////////////////////////////////////////
// CALifeItemTorch
////////////////////////////////////////////////////////////////////////////
CALifeItemTorch::CALifeItemTorch			(LPCSTR caSection) : CALifeItem(caSection), CAbstractServerObject(caSection)
{
	strcpy						(spot_texture,"");
	strcpy						(animator,"");
	spot_range					= 10.f;
	spot_cone_angle				= PI_DIV_3;
	color						= 0xffffffff;
    spot_brightness				= 1.f;
	set_visual					("lights\\lights_torch");
}

CALifeItemTorch::~CALifeItemTorch			()
{
}

void CALifeItemTorch::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
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

void CALifeItemTorch::STATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u32			(color);
	tNetPacket.w_string			(animator);
	tNetPacket.w_string			(spot_texture);
	tNetPacket.w_float			(spot_range);
	tNetPacket.w_angle8			(spot_cone_angle);
	tNetPacket.w_float			(spot_brightness);
}

void CALifeItemTorch::UPDATE_Read			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CALifeItemTorch::UPDATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CALifeItemTorch::FillProp				(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,	 values);
	PHelper.CreateColor			(values, PHelper.PrepareKey(pref,s_name,"Color"),			&color);
	PHelper.CreateLightAnim		(values, PHelper.PrepareKey(pref,s_name,"Color animator"),	animator,			sizeof(animator));
	PHelper.CreateTexture		(values, PHelper.PrepareKey(pref,s_name,"Texture"),			spot_texture,		sizeof(spot_texture));
	PHelper.CreateFloat			(values, PHelper.PrepareKey(pref,s_name,"Range"),			&spot_range,		0.1f, 1000.f);
	PHelper.CreateAngle			(values, PHelper.PrepareKey(pref,s_name,"Angle"),			&spot_cone_angle,	0, PI_DIV_2);
    PHelper.CreateFloat			(values, PHelper.PrepareKey(pref,s_name,"Brightness"),		&spot_brightness,	0.1f, 5.f);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CALifeItemWeapon
////////////////////////////////////////////////////////////////////////////
CALifeItemWeapon::CALifeItemWeapon			(LPCSTR caSection) : CALifeItem(caSection), CAbstractServerObject(caSection)
{
	a_current					= 90;
	a_elapsed					= 0;
	state						= 0;
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
        set_visual				(pSettings->r_string(caSection,"visual"));
}

void CALifeItemWeapon::UPDATE_Read			(NET_Packet	&tNetPacket)
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

void CALifeItemWeapon::UPDATE_Write			(NET_Packet	&tNetPacket)
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

void CALifeItemWeapon::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
	tNetPacket.r_u16			(a_current);
	tNetPacket.r_u16			(a_elapsed);
	tNetPacket.r_u8				(state);
}

void CALifeItemWeapon::STATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u16			(a_current);
	tNetPacket.w_u16			(a_elapsed);
	tNetPacket.w_u8				(state);
}

void CALifeItemWeapon::OnEvent				(NET_Packet	&tNetPacket, u16 type, u32 time, u32 sender )
{
	inherited::OnEvent			(tNetPacket,type,time,sender);
	switch (type) {
		case GE_WPN_STATE_CHANGE:
			tNetPacket.r_u8	(state);
			break;
	}
}

u8	 CALifeItemWeapon::get_slot				()
{
	return						((u8)pSettings->r_u8(s_name,"slot"));
}

u16	 CALifeItemWeapon::get_ammo_limit		()
{
	return						(u16) pSettings->r_u16(s_name,"ammo_limit");
}

u16	 CALifeItemWeapon::get_ammo_total		()
{
	return						((u16)a_current);
}

u16	 CALifeItemWeapon::get_ammo_elapsed		()
{
	return						((u16)a_elapsed);
}

u16	 CALifeItemWeapon::get_ammo_magsize		()
{
	if (pSettings->line_exist(s_name,"ammo_mag_size"))
		return					(pSettings->r_u16(s_name,"ammo_mag_size"));
	else
		return					0;
}

#ifdef _EDITOR
void CALifeItemWeapon::FillProp				(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref, items);
	PHelper.CreateU16			(items,PHelper.PrepareKey(pref,s_name,"Ammo: total"),			&a_current,0,1000,1);
	PHelper.CreateU16			(items,PHelper.PrepareKey(pref,s_name,"Ammo: in magazine"),	&a_elapsed,0,30,1);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CALifeItemAmmo
////////////////////////////////////////////////////////////////////////////
CALifeItemAmmo::CALifeItemAmmo				(LPCSTR caSection) : CALifeItem(caSection), CAbstractServerObject(caSection)
{
	a_elapsed					= m_boxSize = (u16)pSettings->r_s32(caSection, "box_size");
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
        set_visual				(pSettings->r_string(caSection,"visual"));
}

void CALifeItemAmmo::STATE_Read				(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
	tNetPacket.r_u16			(a_elapsed);
}

void CALifeItemAmmo::STATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u16			(a_elapsed);
}

void CALifeItemAmmo::UPDATE_Read			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	tNetPacket.r_u16			(a_elapsed);
}

void CALifeItemAmmo::UPDATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	tNetPacket.w_u16			(a_elapsed);
}

#ifdef _EDITOR
void CALifeItemAmmo::FillProp				(LPCSTR pref, PropItemVec& values) {
  	inherited::FillProp			(pref,values);
	PHelper.CreateU16			(values, PHelper.PrepareKey(pref, s_name, "Ammo: left"), &a_elapsed, 0, m_boxSize, m_boxSize);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CALifeItemCar
////////////////////////////////////////////////////////////////////////////
void CALifeItemCar::STATE_Read				(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion > 20)
		inherited::STATE_Read	(tNetPacket,size);
}

void CALifeItemCar::STATE_Write				(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifeItemCar::UPDATE_Read				(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CALifeItemCar::UPDATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CALifeItemCar::FillProp				(LPCSTR pref, PropItemVec& values)
{
  	inherited::FillProp			(pref,values);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CALifeItemDetector
////////////////////////////////////////////////////////////////////////////
void CALifeItemDetector::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion > 20)
		inherited::STATE_Read	(tNetPacket,size);
}

void CALifeItemDetector::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CALifeItemDetector::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CALifeItemDetector::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CALifeItemDetector::FillProp			(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProp			(pref,items);
}
#endif
