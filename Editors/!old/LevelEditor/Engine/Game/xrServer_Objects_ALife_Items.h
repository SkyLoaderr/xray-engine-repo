////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects items for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALife_ItemsH
#define xrServer_Objects_ALife_ItemsH

#include "xrServer_Objects_ALife.h"
#include "PHSynchronize.h"
#include "inventory_space.h"

class CSE_ALifeInventoryItem : virtual public CSE_Abstract {
public:
	typedef CSE_Abstract inherited;
	enum EFlags {
		FLAG_NO_POSITION = u16(0x8000),
	};

public:
	float							m_fCondition;
	float							m_fMass;
	int								m_iVolume;
	u32								m_dwCost;
	s32								m_iHealthValue;
	s32								m_iFoodValue;
	float							m_fDeteriorationValue;
	int								m_iGridWidth;
	int								m_iGridHeight;
	u64								m_qwGridBitMask;
	ALife::_OBJECT_ID				m_tPreviousParentID;
	bool							m_can_switch_offline;

	//положение вещи в инвенторе (используется актером 
	//при переходе в offline и обратно)
	EItemPlace						m_eItemPlace;

									CSE_ALifeInventoryItem	(LPCSTR caSection);
	virtual							~CSE_ALifeInventoryItem	();
	IC		bool					attached	() const
	{
		return						(ID_Parent < 0xffff);
	}
	virtual bool					bfUseful();
	virtual bool					can_switch_offline	() const;

	/////////// network ///////////////
	u32								m_dwTimeStamp;
	u16								m_u16NumItems;
	SPHNetState						State;
	///////////////////////////////////
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeItem,CSE_ALifeDynamicObjectVisual,CSE_ALifeInventoryItem)
									CSE_ALifeItem	(LPCSTR caSection);
	virtual							~CSE_ALifeItem	();
	virtual bool					can_switch_offline	() const;
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemTorch,CSE_ALifeItem)
/*
	u32								color;
	string64						animator;
	string64						spot_texture;
	float							spot_range;
	float							spot_cone_angle;
    float							spot_brightness;
	string64						glow_texture;
	float							glow_radius;
    u16								guid_bone;
*/
	bool							m_active;

									CSE_ALifeItemTorch	(LPCSTR caSection);
    virtual							~CSE_ALifeItemTorch	();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemAmmo,CSE_ALifeItem)
	u16								a_elapsed;
	u16								m_boxSize;

									CSE_ALifeItemAmmo	(LPCSTR caSection);
	virtual							~CSE_ALifeItemAmmo	();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemWeapon,CSE_ALifeItem)

	//возможность подключения аддонов
	enum EWeaponAddonStatus{
		eAddonDisabled				= 0,	//нельзя присоеденить
		eAddonPermanent				= 1,	//постоянно подключено по умолчанию
		eAddonAttachable			= 2		//можно присоединять
	};

	//текущее состояние аддонов
	enum EWeaponAddonState 
	{
		eWeaponAddonScope = 0x01,
		eWeaponAddonGrenadeLauncher = 0x02,
		eWeaponAddonSilencer = 0x04
	};



	u32								timestamp;
	u8								flags;
	u8								state;
	u8								ammo_type;
	u16								a_current;
	u16								a_elapsed;
	float							m_fHitPower;
	ALife::EHitType					m_tHitType;
	LPCSTR							m_caAmmoSections;
	u32								m_dwAmmoAvailable;
	u32								m_dwSlot;
	Flags8							m_addon_flags;
	u8								m_bZoom;

									CSE_ALifeItemWeapon(LPCSTR caSection);
	virtual							~CSE_ALifeItemWeapon();
	virtual void					OnEvent			(NET_Packet& P, u16 type, u32 time, u32 sender );
	u8								get_slot		();
	u16								get_ammo_limit	();
	u16								get_ammo_total	();
	u16								get_ammo_elapsed();
	u16								get_ammo_magsize();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemDetector,CSE_ALifeItem)
									CSE_ALifeItemDetector(LPCSTR caSection);
	virtual							~CSE_ALifeItemDetector();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemArtefact,CSE_ALifeItem)
	float							m_fAnomalyValue;
									CSE_ALifeItemArtefact(LPCSTR caSection);
	virtual							~CSE_ALifeItemArtefact();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemPDA,CSE_ALifeItem)
	u16								m_original_owner;

									CSE_ALifeItemPDA(LPCSTR caSection);
	virtual							~CSE_ALifeItemPDA();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemDocument,CSE_ALifeItem)
	u16								m_wDocIndex;
									CSE_ALifeItemDocument(LPCSTR caSection);
	virtual							~CSE_ALifeItemDocument();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemGrenade,CSE_ALifeItem)
									CSE_ALifeItemGrenade(LPCSTR caSection);
	virtual							~CSE_ALifeItemGrenade();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemExplosive,CSE_ALifeItem)
									CSE_ALifeItemExplosive(LPCSTR caSection);
	virtual							~CSE_ALifeItemExplosive();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeItemBolt,CSE_ALifeDynamicObject,CSE_ALifeInventoryItem)
									CSE_ALifeItemBolt(LPCSTR caSection);
	virtual							~CSE_ALifeItemBolt();
	virtual bool					can_switch_offline	() const;
	virtual bool					can_save			() const;
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemCustomOutfit,CSE_ALifeItem)
									CSE_ALifeItemCustomOutfit(LPCSTR caSection);
	virtual							~CSE_ALifeItemCustomOutfit();
SERVER_ENTITY_DECLARE_END

#endif