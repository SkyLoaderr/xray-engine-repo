////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Retsky and Dmitriy Iassenev
//	Description : Server objects for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALifeH
#define xrServer_Objects_ALifeH

#include "xrServer_Objects.h"
#include "ai_alife_templates.h"

SERVER_OBJECT_DECLARE_BEGIN(CALifeEvent,IPureServerObject)
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_GRAPH_ID						m_tGraphID;
	EBattleResult					m_tBattleResult;
	CALifeEventGroup				*m_tpMonsterGroup1;
	CALifeEventGroup				*m_tpMonsterGroup2;

									CALifeEvent(LPCSTR caSection);
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifePersonalEvent,IPureServerObject)
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_TASK_ID						m_tTaskID;
	int								m_iHealth;
	ERelation						m_tRelation;
	OBJECT_VECTOR					m_tpItemIDs;

									CALifePersonalEvent()
	{
	};
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifeTask,IPureServerObject)
	_TASK_ID						m_tTaskID;
	_TIME_ID						m_tTimeID;
	_OBJECT_ID						m_tCustomerID;
	float							m_fCost;
	ETaskType						m_tTaskType;
	union {
		_CLASS_ID					m_tClassID;
		_OBJECT_ID					m_tObjectID;
	};
	union {
		_LOCATION_ID				m_tLocationID[LOCATION_TYPE_COUNT];
		_GRAPH_ID					m_tGraphID;
	};

									CALifeTask()
	{
	};
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifePersonalTask,CALifeTask)
	u32								m_dwTryCount;

									CALifePersonalTask()
	{
		m_dwTryCount				= 0;
	};
SERVER_OBJECT_DECLARE_END

class CALifeObject : virtual public CAbstractServerObject {
public:
	typedef CAbstractServerObject inherited;
	_CLASS_ID						m_tClassID;
	_OBJECT_ID						m_tObjectID;
	_GRAPH_ID						m_tGraphID;
	float							m_fDistance;
	bool							m_bOnline;
	u8								m_ucProbability;
	u32								m_dwSpawnGroup;
	bool							m_bDirectControl;
	u32								m_tNodeID;

									CALifeObject(LPCSTR caSection) : CAbstractServerObject(caSection)
	{
		m_bOnline					= false;
		m_fDistance					= 0.0f;
		m_tClassID					= _CLASS_ID(-1);
		m_tObjectID					= _OBJECT_ID(-1);
		m_tGraphID					= _GRAPH_ID(-1);
		m_ucProbability				= 1;
		m_dwSpawnGroup				= 0;
		m_bDirectControl			= true;
		m_tNodeID					= u32(-1);
	};

	virtual void					STATE_Write	(NET_Packet &tNetPacket);
	virtual void					STATE_Read	(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read	(NET_Packet &tNetPacket);
#ifdef _EDITOR
    virtual void					FillProp	(LPCSTR pref, PropItemVec &items);
#endif
};

class CALifeAbstractGroup : virtual public CAbstractServerObject {
public:
	OBJECT_VECTOR					m_tpMembers;
	bool							m_bCreateSpawnPositions;
	u16								m_wCount;

									CALifeAbstractGroup(LPCSTR caSection) : CAbstractServerObject(caSection)
	{
		m_tpMembers.clear			();
		m_bCreateSpawnPositions		= true;
		m_wCount					= 1;
	};

	virtual							~CALifeAbstractGroup()
	{
	};
	
	virtual void STATE_Read			(NET_Packet	&tNetPacket, u16 size)
	{
		u32							dwDummy;
		tNetPacket.r_u32			(dwDummy);
		m_bCreateSpawnPositions		= !!dwDummy;
		tNetPacket.r_u16			(m_wCount);
		if (m_wVersion > 19)
			load_base_vector		(m_tpMembers,tNetPacket);
	};

	virtual void STATE_Write		(NET_Packet	&tNetPacket)
	{
		tNetPacket.w_u32			(m_bCreateSpawnPositions);
		tNetPacket.w_u16			(m_wCount);
		save_base_vector			(m_tpMembers,tNetPacket);
	};

	virtual void UPDATE_Read		(NET_Packet	&tNetPacket)
	{
		u32							dwDummy;
		tNetPacket.r_u32			(dwDummy);
		m_bCreateSpawnPositions		= !!dwDummy;
	};

	virtual void UPDATE_Write		(NET_Packet	&tNetPacket)
	{
		tNetPacket.w_u32			(m_bCreateSpawnPositions);
	};

	#ifdef _EDITOR
	virtual void FillProp			(LPCSTR pref, PropItemVec& items)
	{
		PHelper.CreateU16			(items,	PHelper.PrepareKey(pref, "ALife\\Count"),			&m_wCount,			0,0xff);
	};	
	#endif
};

template<class __A> class CALifeGroupTemplate : public __A, public CALifeAbstractGroup {
	typedef __A					inherited1;
	typedef CALifeAbstractGroup inherited2;
public:
									CALifeGroupTemplate(LPCSTR caSection) : __A(caSection), CALifeAbstractGroup(caSection), CAbstractServerObject(caSection)
	{
	};
	
	virtual							~CALifeGroupTemplate()
	{
	};
	
	virtual void STATE_Read			(NET_Packet	&tNetPacket, u16 size)
	{
		inherited1::STATE_Read		(tNetPacket,size);
		inherited2::STATE_Read		(tNetPacket,size);
	};

	virtual void STATE_Write		(NET_Packet	&tNetPacket)
	{
		inherited1::STATE_Write		(tNetPacket);
		inherited2::STATE_Write		(tNetPacket);
	};

	virtual void UPDATE_Read		(NET_Packet	&tNetPacket)
	{
		inherited1::UPDATE_Read		(tNetPacket);
		inherited2::UPDATE_Read		(tNetPacket);
	};

	virtual void UPDATE_Write		(NET_Packet	&tNetPacket)
	{
		inherited1::UPDATE_Write	(tNetPacket);
		inherited2::UPDATE_Write	(tNetPacket);
	};

	#ifdef _EDITOR
	virtual void FillProp			(LPCSTR pref, PropItemVec& items)
	{
   		inherited1::FillProp		(pref, items);
   		inherited2::FillProp		(pref, items);
	};	
	#endif
};

SERVER_OBJECT_DECLARE_BEGIN(CALifeEventGroup,CALifeObject)
	u16								m_wCountBefore;
	u16								m_wCountAfter;
	
									CALifeEventGroup(LPCSTR caSection) : CALifeObject(caSection), CAbstractServerObject(caSection)
	{
		m_wCountAfter				= m_wCountBefore;
	};
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifeDynamicObject,CALifeObject)
	_TIME_ID						m_tTimeID;
	u32								m_dwLastSwitchTime;
	
									CALifeDynamicObject(LPCSTR caSection) : CALifeObject(caSection), CAbstractServerObject(caSection)
	{
		m_tTimeID					= 0;
		m_dwLastSwitchTime			= 1;
	}
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN2(CALifeDynamicObjectVisual,CALifeDynamicObject,CServerObjectVisual)
									CALifeDynamicObjectVisual(LPCSTR caSection) : CALifeDynamicObject(caSection), CServerObjectVisual(), CAbstractServerObject(caSection)
	{
	}
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN2(CALifeAnomalousZone,CALifeDynamicObject,CServerObjectShape)
	f32								m_maxPower;
	f32								m_attn;
	u32								m_period;
									CALifeAnomalousZone	(LPCSTR caSection);
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifeObjectPhysic,CALifeDynamicObjectVisual)
	u32 							type;
	f32 							mass;
    string32 						fixed_bone;
									CALifeObjectPhysic	(LPCSTR caSection);
    virtual 						~CALifeObjectPhysic	();
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifeObjectHangingLamp,CALifeDynamicObjectVisual)
#ifdef _EDITOR
	void __fastcall					OnChangeAnim	(PropValue* sender);
#endif
	enum{
    	flPhysic					= (1<<0)
    };
	Flags16							flags;
    float							mass;
	u32								color;
	string32						startup_animation;
	string64						color_animator;
	string64						spot_texture;
	string32						spot_bone;
	float							spot_range;
	float							spot_cone_angle;
    float							spot_brightness;
									CALifeObjectHangingLamp	(LPCSTR caSection);
    virtual							~CALifeObjectHangingLamp	();
SERVER_OBJECT_DECLARE_END

#endif