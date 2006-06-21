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

#include "character_info_defs.h"
#include "infoportiondefs.h"

#pragma warning(push)
#pragma warning(disable:4005)

class CSE_ALifeItemAmmo;

SERVER_ENTITY_DECLARE_BEGIN0(CSE_ALifeInventoryItem)
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
	CSE_ALifeObject					*m_self;

									CSE_ALifeInventoryItem	(LPCSTR caSection);
	virtual							~CSE_ALifeInventoryItem	();
	// we need this to prevent virtual inheritance :-(
	virtual CSE_Abstract			*base					() = 0;
	virtual const CSE_Abstract		*base					() const = 0;
	virtual CSE_Abstract			*init					();
	virtual CSE_Abstract			*cast_abstract			() {return 0;};
	virtual CSE_ALifeInventoryItem	*cast_inventory_item	() {return this;};
	// end of the virtual inheritance dependant code

	IC		bool					attached	() const
	{
		return						(base()->ID_Parent < 0xffff);
	}
	virtual bool					bfUseful();

	/////////// network ///////////////
	u32								m_dwTimeStamp;
	u16								m_u16NumItems;
	SPHNetState						State;
	///////////////////////////////////
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeInventoryItem)
#define script_type_list save_type_list(CSE_ALifeInventoryItem)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeItem,CSE_ALifeDynamicObjectVisual,CSE_ALifeInventoryItem)
									CSE_ALifeItem	(LPCSTR caSection);
	virtual							~CSE_ALifeItem	();
	virtual CSE_Abstract			*base			();
	virtual const CSE_Abstract		*base			() const;
	virtual CSE_Abstract			*init					();
	virtual CSE_Abstract			*cast_abstract			() {return this;};
	virtual CSE_ALifeInventoryItem	*cast_inventory_item	() {return this;};
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItem)
#define script_type_list save_type_list(CSE_ALifeItem)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemTorch,CSE_ALifeItem)
	bool							m_active;
									CSE_ALifeItemTorch	(LPCSTR caSection);
    virtual							~CSE_ALifeItemTorch	();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemTorch)
#define script_type_list save_type_list(CSE_ALifeItemTorch)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemAmmo,CSE_ALifeItem)
	u16								a_elapsed;
	u16								m_boxSize;

									CSE_ALifeItemAmmo	(LPCSTR caSection);
	virtual							~CSE_ALifeItemAmmo	();
	virtual CSE_ALifeItemAmmo		*cast_item_ammo		()  {return this;};
	virtual bool					can_switch_online	() const;
	virtual bool					can_switch_offline	() const;

SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemAmmo)
#define script_type_list save_type_list(CSE_ALifeItemAmmo)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemWeapon,CSE_ALifeItem)

	//����������� ����������� �������
	enum EWeaponAddonStatus{
		eAddonDisabled				= 0,	//������ ������������
		eAddonPermanent				= 1,	//��������� ���������� �� ���������
		eAddonAttachable			= 2		//����� ������������
	};

	//������� ��������� �������
	enum EWeaponAddonState 
	{
		eWeaponAddonScope = 0x01,
		eWeaponAddonGrenadeLauncher = 0x02,
		eWeaponAddonSilencer = 0x04
	};

	EWeaponAddonStatus				m_scope_status;
	EWeaponAddonStatus				m_silencer_status;				
	EWeaponAddonStatus				m_grenade_launcher_status;

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
	u32								m_ef_main_weapon_type;
	u32								m_ef_weapon_type;

									CSE_ALifeItemWeapon(LPCSTR caSection);
	virtual							~CSE_ALifeItemWeapon();
	virtual void					OnEvent			(NET_Packet& P, u16 type, u32 time, ClientID sender );
	virtual u32						ef_main_weapon_type	() const;
	virtual u32						ef_weapon_type	() const;
	u8								get_slot		();
	u16								get_ammo_limit	();
	u16								get_ammo_total	();
	u16								get_ammo_elapsed();
	u16								get_ammo_magsize();
	virtual CSE_ALifeItemWeapon		*cast_item_weapon	() {return this;}
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemWeapon)
#define script_type_list save_type_list(CSE_ALifeItemWeapon)


SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemWeaponShotGun,CSE_ALifeItemWeapon)
	xr_vector<u8>				m_AmmoIDs;
								CSE_ALifeItemWeaponShotGun(LPCSTR caSection);
virtual							~CSE_ALifeItemWeaponShotGun();

virtual CSE_ALifeItemWeapon		*cast_item_weapon	() {return this;}
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemWeaponShotGun)
#define script_type_list save_type_list(CSE_ALifeItemWeaponShotGun)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemDetector,CSE_ALifeItem)
	u32								m_ef_detector_type;
									CSE_ALifeItemDetector(LPCSTR caSection);
	virtual							~CSE_ALifeItemDetector();
	virtual u32						ef_detector_type() const;
	virtual CSE_ALifeItemDetector	*cast_item_detector		() {return this;}
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemDetector)
#define script_type_list save_type_list(CSE_ALifeItemDetector)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemArtefact,CSE_ALifeItem)
	float							m_fAnomalyValue;
									CSE_ALifeItemArtefact(LPCSTR caSection);
	virtual							~CSE_ALifeItemArtefact();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemArtefact)
#define script_type_list save_type_list(CSE_ALifeItemArtefact)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemPDA,CSE_ALifeItem)
	u16								m_original_owner;
	SPECIFIC_CHARACTER_ID			m_specific_character;
//	INFO_INDEX						m_info_portion;
	INFO_ID							m_info_portion;

									CSE_ALifeItemPDA(LPCSTR caSection);
	virtual							~CSE_ALifeItemPDA();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemPDA)
#define script_type_list save_type_list(CSE_ALifeItemPDA)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemDocument,CSE_ALifeItem)
	INFO_ID							m_wDoc;
									CSE_ALifeItemDocument(LPCSTR caSection);
	virtual							~CSE_ALifeItemDocument();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemDocument)
#define script_type_list save_type_list(CSE_ALifeItemDocument)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemGrenade,CSE_ALifeItem)
	u32								m_ef_weapon_type;
									CSE_ALifeItemGrenade	(LPCSTR caSection);
	virtual							~CSE_ALifeItemGrenade	();
	virtual u32						ef_weapon_type			() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemGrenade)
#define script_type_list save_type_list(CSE_ALifeItemGrenade)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemExplosive,CSE_ALifeItem)
									CSE_ALifeItemExplosive(LPCSTR caSection);
	virtual							~CSE_ALifeItemExplosive();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemExplosive)
#define script_type_list save_type_list(CSE_ALifeItemExplosive)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemBolt,CSE_ALifeItem)
	u32								m_ef_weapon_type;
									CSE_ALifeItemBolt	(LPCSTR caSection);
	virtual							~CSE_ALifeItemBolt	();
	virtual bool					can_save			() const;
	virtual bool					used_ai_locations	() const;
	virtual u32						ef_weapon_type		() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemBolt)
#define script_type_list save_type_list(CSE_ALifeItemBolt)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeItemCustomOutfit,CSE_ALifeItem)
	u32								m_ef_equipment_type;
									CSE_ALifeItemCustomOutfit(LPCSTR caSection);
	virtual							~CSE_ALifeItemCustomOutfit();
	virtual u32						ef_equipment_type	() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeItemCustomOutfit)
#define script_type_list save_type_list(CSE_ALifeItemCustomOutfit)

#pragma warning(pop)

#endif