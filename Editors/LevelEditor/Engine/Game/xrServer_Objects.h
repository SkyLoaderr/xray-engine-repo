////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_ObjectsH
#define xrServer_ObjectsH

#include "xrMessages.h"
#include "ai_alife_interfaces.h"
#include "xrServer_Space.h"

#ifdef _EDITOR
	#include "PropertiesListHelper.h"
#endif

#define SPAWN_VERSION	u16(59)
//------------------------------------------------------------------------------
// Version history
//------------------------------------------------------------------------------
// 10 - xrSE_ALifeObjectPhysic			appended with property 'fixed_bone'
// 11 - xrSE_ALifeObjectHangingLamp		appended with property 'spot_brightness'
// 12 - xrSE_ALifeObjectHangingLamp		appended with property 'flags'
// 13 - xrSE_ALifeObjectHangingLamp		appended with property 'mass'
// 14 - xrSE_ALifeObjectPhysic			inherited from CSE_ALifeObject
// 15 - xrSE_ALifeAnomalousZone			inherited calls from CSE_ALifeDynamicObject
// 16 - xrSE_ALifeObjectPhysic			inherited from CSE_ALifeDynamicObject
// 17 - xrSE_...						inherited from CSE_Visual for smart Level Editor
// 18 - xrSE_ALifeObjectHangingLamp		appended with 'startup_animation'
// 19 - xrSE_Teamed						didn't save health parameter
// 20 - xrSE_ALife...					saving vectors in UPDATE_Read/UPDATE_Write changed to STATE_Read/STATE_Write
// 21 -	GLOBAL CLASS HIERARCHY UPDATE
// 22 - CSE_AnomalousZone				appended with a artefact spawns
// 23 - CSE_ALifeObject					appended with a spawn ID
// 24 - CSE_ALifeObject					appended with a group control
// 25 - CSE_ALifeObject					changed type of the property probability from u8 to float
// 26 - CSE_AnomalousZone				appended with artefact spawn information
// 27 - CSE_AnomalousZone				weights changed type from u32 to float
// 28 - CSE_AnomalousZone				appended with an anomalous zone type
// 29 - CSE_ALifeObjectPhysic			appended with an animation property
// 30 - CSE_ALifeTrader					appended with an ordered artefacts property
// 31 - CSE_ALifeTrader					appended with a supplies property
// 32 - CSE_ALifeDynamicObjectVisual	the only this class saves and loads visual object
// 33 - CSE_ALifeGraphPoint and CSE_ALifeLevelChanger	level id changed to level name
// 34 - CSE_ALifeLevelPoint and CSE_ALifeLevelChanger	appended several _new properties
// 35 - CSE_ALifeTrader					artefact order structures changed
// 36 - CSE_ALifeTrader					appended with the organization identifier property
//		and CSE_ALifeHumanAbstract		appended with known_traders property
//		and CSE_ALifeTask				appended with try count property
//		and CSE_ALifePersonalTask		no more exists
// 37 - ������� ����� ServerEntity �� CSE_ALifeItemWeapon, � CSE_ALifeItem
// 38 - CSE_ALifeHumanAbstract			appended with the equipment and weapon preferences
// 39 - CSE_ALifeAnomalousZone			appended with the start power property
// 40 - CSE_ALifeObjectPhysic			appended with activate flag
//		and CSE_ALifeItemWeapon			appended with weapon addons state flag
// 41 - CSE_ALifeItemTorch				appended glow
// 42 - CSE_ALifeItemTorch				appended guid_bone
// 43 - CSE_ALifeObjectHangingLamp		appended glow_texture and glow_radius
// 44 - xrSE_ALifeObjectHangingLamp		appended with property 'fixed bones'
// 45 - xrSE_ALifeObjectHangingLamp		appended with property 'health'
// 46 - xrSE_ALifeObjectSearchLight		appended with property 'guid_bone', 
//										appended with property 'rotation_bone'
// 47 - CSE_ALifeItemWeapon				appended with ammo type index
// 48 - CSE_ALifeObjectSearchlight		appended with property 'cone_bone'
// 49 - CSE_ALifeObjectHangingLamp		remove some property
// 50 - CSE_ALifeObject					appended with ALife flags property
// 51 - CSE_ALifeItemBolt				new class based on CSE_ALifeDynamicObject and CSE_AlifeInventoryItem
// 52 - CSE_ALifeItemExplosive			new class based on CSE_ALifeItem
// 53 - CSE_AlifeInventoryItem			appended with property 'condition'
// 54 - CSE_AlifeLevelChanger			property 'angles' changed from float to fvector
// 55 - CSE_AlifeItemCar->CSE_ALifeCar	heritage changed
// 56 - CSE_ALifeObjectHangingLamp		restore removed props + appended some new
// 57 - CSE_ALifeObjectPhysic			appended with source_id
// 58 - CSE_ALifeObject					appended with ini_string
// 59 - CSE_ALifeItemPDA				appended with m_original_owner
//------------------------------------------------------------------------------

class CPureServerObject : public IPureServerObject {
public:
	virtual							~CPureServerObject(){}
	virtual void					Load(IReader	&tFileStream);
	virtual void					Save(IWriter	&tMemoryStream);
	virtual void					Load(NET_Packet	&tNetPacket);
	virtual void					Save(NET_Packet	&tNetPacket);
};

class xrClientData;

class CSE_Abstract : public CPureServerObject {
public:
	BOOL							net_Ready;
	BOOL							net_Processed;	// Internal flag for connectivity-graph
	
	u16								m_wVersion;
	u16								RespawnTime;

	u16								ID;				// internal ID
	u16								ID_Parent;		// internal ParentID, 0xffff means no parent
	u16								ID_Phantom;		// internal PhantomID, 0xffff means no phantom
	xrClientData*					owner;
	xr_vector<u16>					children;

	// spawn data
	string64						s_name;
	string64						s_name_replace;
	u8								s_gameid;
	u8								s_RP;
	Flags16							s_flags;		// state flags

	// update data
	Fvector							o_Position;
	Fvector							o_Angle;

	// for ALife control
	bool							m_bALifeControl;
	ref_str							m_ini_string;

									CSE_Abstract	(LPCSTR caSection);
	virtual							~CSE_Abstract	();
	virtual void					OnEvent			(NET_Packet &tNetPacket, u16 type, u32 time, u32 sender ){};
	void							Spawn_Write		(NET_Packet &tNetPacket, BOOL bLocal);
	BOOL							Spawn_Read		(NET_Packet &tNetPacket);
	IC		const Fvector			&Position		() const					{return o_Position;};
	// editor integration
#ifdef _EDITOR
    virtual void					FillProp		(LPCSTR pref, PropItemVec &items);
#endif
};

class CSE_Shape
{
public:
	enum{
    	cfSphere=0,
        cfBox
    };
	union shape_data
	{
		Fsphere		sphere;
		Fmatrix		box;
	};
	struct shape_def
	{
		u8			type;
		shape_data	data;
	};
    DEFINE_VECTOR					(shape_def,ShapeVec,ShapeIt);
	ShapeVec						shapes;
public:
	void							cform_read		(NET_Packet& P);
	void							cform_write		(NET_Packet& P);
									CSE_Shape		();
	virtual							~CSE_Shape		();
};

class CSE_Visual
{
	string64						visual_name;
public:
#ifdef _EDITOR
	AnsiString						play_animation;
	IRender_Visual*		   			visual;
    void __fastcall					OnChangeVisual	(PropValue* sender);
    void 							PlayAnimation	(LPCSTR name);
#endif
public:
									CSE_Visual		(LPCSTR name=0);
	virtual							~CSE_Visual		();

	void							visual_read		(NET_Packet& P);
	void							visual_write	(NET_Packet& P);

    void							set_visual		(LPCSTR name, bool load=true);
	LPCSTR							get_visual		() const {return visual_name;};
    
#ifdef _EDITOR
    void 							FillProp		(LPCSTR pref, PropItemVec& values);
#endif
};

#ifdef _EDITOR
	class CObjectAnimator;
#endif

class CSE_Motion
{
	ref_str							motion_name;
public:
#ifdef _EDITOR
	CObjectAnimator*	   			animator;
    void __fastcall					OnChangeMotion	(PropValue* sender);
    void 							PlayMotion		(LPCSTR name=0);
#endif
public:
									CSE_Motion 		(LPCSTR name=0);
	virtual							~CSE_Motion		();

	void							motion_read		(NET_Packet& P);
	void							motion_write	(NET_Packet& P);

    void							set_motion		(LPCSTR name);
	LPCSTR							get_motion		() const {return *motion_name;};
    
#ifdef _EDITOR
    void 							FillProp		(LPCSTR pref, PropItemVec& values);
#endif
};

SERVER_ENTITY_DECLARE_BEGIN(CSE_Spectator,CSE_Abstract)
									CSE_Spectator	(LPCSTR caSection);
	virtual							~CSE_Spectator	();
	virtual u8						g_team			();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_Target,CSE_Abstract)
									CSE_Target		(LPCSTR caSection);
	virtual							~CSE_Target		();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_TargetAssault,CSE_Target)
									CSE_TargetAssault(LPCSTR caSection);
	virtual							~CSE_TargetAssault();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_Target_CS_Base,CSE_Target)
	float							radius;
	u8								s_team;
									CSE_Target_CS_Base(LPCSTR caSection);
	virtual							~CSE_Target_CS_Base();
	virtual u8						g_team			();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_Target_CS_Cask,CSE_Target)
	string64						s_Model;
									CSE_Target_CS_Cask(LPCSTR caSection);
	virtual							~CSE_Target_CS_Cask();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_Target_CS,CSE_Target)
	string64						s_Model;
									CSE_Target_CS	(LPCSTR caSection);
	virtual							~CSE_Target_CS	();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_Temporary,CSE_Abstract)
	u32								m_tNodeID;
									CSE_Temporary	(LPCSTR caSection);
	virtual							~CSE_Temporary	();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_Event,CSE_Shape,CSE_Abstract)
	struct tAction
	{
		u8		type;
		u16		count;
		u64		cls;
		LPSTR	event;
	};
	xr_vector<tAction>				Actions;

									CSE_Event		(LPCSTR caSection);
	virtual							~CSE_Event		();
			void					Actions_clear	();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_SpawnGroup,CSE_Abstract)
	float							m_fGroupProbability;
	u32								m_dwSpawnGroup;
	
									CSE_SpawnGroup	(LPCSTR caSection);
	virtual							~CSE_SpawnGroup	();
SERVER_ENTITY_DECLARE_END

extern CSE_Abstract		*F_entity_Create	(LPCSTR caSection);

#endif