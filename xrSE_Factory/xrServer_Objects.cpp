////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "net_utils.h"
#include "xrServer_Objects.h"
#include "game_base_space.h"

////////////////////////////////////////////////////////////////////////////
// CSE_Shape
////////////////////////////////////////////////////////////////////////////
CSE_Shape::CSE_Shape						()
{
}

CSE_Shape::~CSE_Shape						()
{
}

void CSE_Shape::cform_read					(NET_Packet	&tNetPacket)
{
	shapes.clear				();
	u8							count;
	tNetPacket.r_u8				(count);
	
	while (count) {
		shape_def				S;
		tNetPacket.r_u8			(S.type);
		switch (S.type) {
			case 0 :	
				tNetPacket.r	(&S.data.sphere,sizeof(S.data.sphere));	
				break;
			case 1 :	
				tNetPacket.r_matrix(S.data.box);
				break;
		}
		shapes.push_back		(S);
		--count;
	}
}

void CSE_Shape::cform_write					(NET_Packet	&tNetPacket)
{
	tNetPacket.w_u8				(u8(shapes.size()));
	for (u32 i=0; i<shapes.size(); ++i) {
		shape_def				&S = shapes[i];
		tNetPacket.w_u8			(S.type);
		switch (S.type) {
			case 0:	
				tNetPacket.w	(&S.data.sphere,sizeof(S.data.sphere));
				break;
			case 1:	
				tNetPacket.w_matrix	(S.data.box);
				break;
		}
	}
}

void CSE_Shape::assign_shapes	(CShapeData::shape_def* _shapes, u32 _cnt)
{
	shapes.resize	(_cnt);
	for (u32 k=0; k<_cnt; k++)
		shapes[k]	= _shapes[k];
}

////////////////////////////////////////////////////////////////////////////
// CSE_Event
////////////////////////////////////////////////////////////////////////////
CSE_Event::CSE_Event						(LPCSTR caSection) : CSE_Shape(), CSE_Abstract(caSection)
{
}

CSE_Event::~CSE_Event						()
{
}

ISE_Shape* CSE_Event::shape					()
{
	return						(this);
}

void CSE_Event::Actions_clear				()
{
	for (u32 a=0; a<Actions.size(); a++)
		xr_free					(Actions[a].event);
	Actions.clear				();
}

void CSE_Event::UPDATE_Read					(NET_Packet	&tNetPacket)
{
}

void CSE_Event::UPDATE_Write				(NET_Packet	&tNetPacket)
{
}

void CSE_Event::STATE_Read					(NET_Packet	&tNetPacket, u16 size)
{
	// CForm
	cform_read					(tNetPacket);

	// Actions
	Actions_clear				();
	u8							count;
	tNetPacket.r_u8				(count);
	while (count) {
		tAction					A;
		string512				str;
		tNetPacket.r_u8			(A.type);
		tNetPacket.r_u16		(A.count);
		tNetPacket.r_u64		(A.cls);
		tNetPacket.r_stringZ	(str);
		A.event					= xr_strdup(str);
		Actions.push_back		(A);
		--count;
	}
}

void CSE_Event::STATE_Write					(NET_Packet	&tNetPacket)
{
	// CForm
	cform_write					(tNetPacket);

	// Actions
	tNetPacket.w_u8				(u8(Actions.size()));
	for (u32 i=0; i<Actions.size(); ++i) {
		tAction&				A = Actions[i];
		tNetPacket.w_u8			(A.type	);
		tNetPacket.w_u16		(A.count);
		tNetPacket.w_u64		(A.cls	);
		tNetPacket.w_stringZ		(A.event);
	}
}

void CSE_Event::FillProps	(LPCSTR pref, PropItemVec& values)
{
}

////////////////////////////////////////////////////////////////////////////
// CSE_Spectator
////////////////////////////////////////////////////////////////////////////
CSE_Spectator::CSE_Spectator				(LPCSTR caSection) : CSE_Abstract(caSection)
{
}

CSE_Spectator::~CSE_Spectator				()
{
}

u8	 CSE_Spectator::g_team					()
{
	return 0;
}

void CSE_Spectator::STATE_Read				(NET_Packet	&tNetPacket, u16 size)
{
}

void CSE_Spectator::STATE_Write				(NET_Packet	&tNetPacket)
{
}

void CSE_Spectator::UPDATE_Read				(NET_Packet	&tNetPacket)
{
}

void CSE_Spectator::UPDATE_Write			(NET_Packet	&tNetPacket)
{
}

void CSE_Spectator::FillProps				(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProps			(pref,items);
}

////////////////////////////////////////////////////////////////////////////
// CSE_Target
////////////////////////////////////////////////////////////////////////////
CSE_Target::CSE_Target						(LPCSTR caSection) : CSE_Abstract(caSection)
{
}

CSE_Target::~CSE_Target						()
{
}

void CSE_Target::STATE_Read					(NET_Packet	&tNetPacket, u16 size)
{
}

void CSE_Target::STATE_Write				(NET_Packet	&tNetPacket)
{
}

void CSE_Target::UPDATE_Read				(NET_Packet	&tNetPacket)
{
}

void CSE_Target::UPDATE_Write				(NET_Packet	&tNetPacket)
{
}

void CSE_Target::FillProps					(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProps			(pref,values);
}

////////////////////////////////////////////////////////////////////////////
// CSE_TargetAssault
////////////////////////////////////////////////////////////////////////////
CSE_TargetAssault::CSE_TargetAssault		(LPCSTR caSection) : CSE_Target(caSection)
{
}

CSE_TargetAssault::~CSE_TargetAssault		()
{
}

void CSE_TargetAssault::STATE_Read			(NET_Packet	&tNetPacket, u16 size)	
{
}

void CSE_TargetAssault::STATE_Write			(NET_Packet	&tNetPacket)
{
}

void CSE_TargetAssault::UPDATE_Read			(NET_Packet	&tNetPacket)
{
}

void CSE_TargetAssault::UPDATE_Write		(NET_Packet &tNetPacket)
{
}

void CSE_TargetAssault::FillProps			(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProps			(pref,values);
}

////////////////////////////////////////////////////////////////////////////
// CSE_Target_CS_Base
////////////////////////////////////////////////////////////////////////////
CSE_Target_CS_Base::CSE_Target_CS_Base		(LPCSTR caSection) : CSE_Target(caSection)
{
	s_team						= 0;
	radius						= 10.f;
    s_gameid					= GAME_CS;    
}

CSE_Target_CS_Base::~CSE_Target_CS_Base		()
{
}

u8	 CSE_Target_CS_Base::g_team()
{
	return s_team;
}

void CSE_Target_CS_Base::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_float			(radius);
	tNetPacket.r_u8				(s_team);
}

void CSE_Target_CS_Base::STATE_Write		(NET_Packet	&tNetPacket)
{
	tNetPacket.w_float			(radius);
	tNetPacket.w_u8				(s_team);
}

void CSE_Target_CS_Base::UPDATE_Read		(NET_Packet	&tNetPacket)
{
}

void CSE_Target_CS_Base::UPDATE_Write		(NET_Packet	&tNetPacket)
{
}

void CSE_Target_CS_Base::FillProps			(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProps		(pref,items);
    PHelper().CreateFloat		(items,PrepareKey(pref,*s_name,"Radius"),	&radius,1.f,100.f);
    PHelper().CreateU8			(items,PrepareKey(pref,*s_name,"Team"),		&s_team,0,1);
}

////////////////////////////////////////////////////////////////////////////
// CSE_Target_CS_Cask
////////////////////////////////////////////////////////////////////////////
CSE_Target_CS_Cask::CSE_Target_CS_Cask		(LPCSTR caSection) : CSE_Target(caSection)
{
}

CSE_Target_CS_Cask::~CSE_Target_CS_Cask		()
{
}

void CSE_Target_CS_Cask::UPDATE_Read		(NET_Packet	&tNetPacket)
{
}

void CSE_Target_CS_Cask::UPDATE_Write		(NET_Packet	&tNetPacket)
{
}

void CSE_Target_CS_Cask::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_stringZ		(s_Model);
}

void CSE_Target_CS_Cask::STATE_Write		(NET_Packet	&tNetPacket)
{
	tNetPacket.w_stringZ			(s_Model);
}

void CSE_Target_CS_Cask::FillProps			(LPCSTR pref, PropItemVec& items)
{
	PHelper().CreateChoose		(items, PrepareKey(pref,*s_name,"Model"),	&s_Model,	smVisual);
}

////////////////////////////////////////////////////////////////////////////
// CSE_Target_CS
////////////////////////////////////////////////////////////////////////////
CSE_Target_CS::CSE_Target_CS				(LPCSTR caSection) : CSE_Target(caSection)
{
}

CSE_Target_CS::~CSE_Target_CS				()
{
}

void CSE_Target_CS::UPDATE_Read				(NET_Packet	&tNetPacket)
{
}

void CSE_Target_CS::UPDATE_Write			(NET_Packet	&tNetPacket)
{
}

void CSE_Target_CS::STATE_Read				(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_stringZ		(s_Model);
}

void CSE_Target_CS::STATE_Write				(NET_Packet	&tNetPacket)
{
	tNetPacket.w_stringZ			(s_Model);
}

void CSE_Target_CS::FillProps				(LPCSTR pref, PropItemVec& items)
{
	PHelper().CreateChoose		(items, PrepareKey(pref,*s_name,"Model"),	&s_Model,	smVisual);
}

////////////////////////////////////////////////////////////////////////////
// CSE_Temporary
////////////////////////////////////////////////////////////////////////////
CSE_Temporary::CSE_Temporary				(LPCSTR caSection) : CSE_Abstract(caSection)
{
	m_tNodeID					= u32(-1);
}

CSE_Temporary::~CSE_Temporary				()
{
}

void CSE_Temporary::STATE_Read				(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_u32			(m_tNodeID);
};

void CSE_Temporary::STATE_Write				(NET_Packet	&tNetPacket)
{
	tNetPacket.w_u32			(m_tNodeID);
};

void CSE_Temporary::UPDATE_Read				(NET_Packet	&tNetPacket)
{
};

void CSE_Temporary::UPDATE_Write			(NET_Packet	&tNetPacket)
{
};

void CSE_Temporary::FillProps				(LPCSTR pref, PropItemVec& values)
{
};

////////////////////////////////////////////////////////////////////////////
// CSE_SpawnGroup
////////////////////////////////////////////////////////////////////////////

CSE_SpawnGroup::CSE_SpawnGroup				(LPCSTR caSection) : CSE_Abstract(caSection)
{
	m_spawn_probability						= 1.f;
}

CSE_SpawnGroup::~CSE_SpawnGroup				()
{
}

void CSE_SpawnGroup::STATE_Read				(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_float						(m_spawn_probability);
	if (m_wVersion > 80) {
		tNetPacket.r_float					(m_min_spawn_interval);
		tNetPacket.r_float					(m_max_spawn_interval);
		m_flags.assign						(tNetPacket.r_u32());
	}
}

void CSE_SpawnGroup::STATE_Write			(NET_Packet	&tNetPacket)
{
	tNetPacket.w_float						(m_spawn_probability);
	tNetPacket.w_float						(m_min_spawn_interval);
	tNetPacket.w_float						(m_max_spawn_interval);
	tNetPacket.w_u32						(m_flags.get());
}

void CSE_SpawnGroup::UPDATE_Read			(NET_Packet	&tNetPacket)
{
	tNetPacket.r_float						(m_spawn_probability);
	tNetPacket.r_float						(m_min_spawn_interval);
	tNetPacket.r_float						(m_max_spawn_interval);
	m_flags.assign							(tNetPacket.r_u32());
}

void CSE_SpawnGroup::UPDATE_Write			(NET_Packet	&tNetPacket)
{
	tNetPacket.w_float						(m_spawn_probability);
	tNetPacket.w_float						(m_min_spawn_interval);
	tNetPacket.w_float						(m_max_spawn_interval);
	tNetPacket.w_u32						(m_flags.get());
}

void CSE_SpawnGroup::FillProps				(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProps					(pref,values);
	PHelper().CreateFloat					(values,PrepareKey(pref,*s_name,"ALife\\Spawn group probability"),	&m_spawn_probability,	0.f,	1.f);
	PHelper().CreateTime					(values,PrepareKey(pref,*s_name,"ALife\\Min time spawn interval"),	&m_min_spawn_interval);
	PHelper().CreateTime					(values,PrepareKey(pref,*s_name,"ALife\\Max time spawn interval"),	&m_max_spawn_interval);
	PHelper().CreateFlag32					(values,PrepareKey(pref,*s_name,"ALife\\Active"),					&m_flags,	flSpawnGroupActive);
	PHelper().CreateFlag32					(values,PrepareKey(pref,*s_name,"ALife\\Spawn on surge only"),		&m_flags,	flSpawnGroupOnSurgeOnly);
	PHelper().CreateFlag32					(values,PrepareKey(pref,*s_name,"ALife\\Spawn single item only"),	&m_flags,	flSpawnGroupSingleItemOnly);
	PHelper().CreateFlag32					(values,PrepareKey(pref,*s_name,"ALife\\Spawn if destroyed only"),	&m_flags,	flSpawnGroupIfDestroyedOnly);
	PHelper().CreateFlag32					(values,PrepareKey(pref,*s_name,"ALife\\Spawn single time only"),	&m_flags,	flSpawnGroupSingleTimeOnly);
}

////////////////////////////////////////////////////////////////////////////
// CSE_PHSkeleton
////////////////////////////////////////////////////////////////////////////
CSE_PHSkeleton::CSE_PHSkeleton(LPCSTR caSection)
{
	source_id					= u16(-1);
	_flags.zero					();
}

CSE_PHSkeleton::~CSE_PHSkeleton()
{

}

void CSE_PHSkeleton::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	CSE_Visual				*visual = smart_cast<CSE_Visual*>(this);
	R_ASSERT				(visual);
	tNetPacket.r_stringZ	(visual->startup_animation);
	tNetPacket.r_u8			(_flags.flags);
	tNetPacket.r_u16		(source_id);
	if (_flags.test(flSavedData)) {
		data_load(tNetPacket);
	}
}

void CSE_PHSkeleton::STATE_Write		(NET_Packet	&tNetPacket)
{
	CSE_Visual				*visual = smart_cast<CSE_Visual*>(this);
	R_ASSERT				(visual);
	tNetPacket.w_stringZ	(visual->startup_animation);
	tNetPacket.w_u8			(_flags.flags);
	tNetPacket.w_u16		(source_id);
	////////////////////////saving///////////////////////////////////////
	if(_flags.test(flSavedData))
	{
		data_save(tNetPacket);
	}
}

void CSE_PHSkeleton::data_load(NET_Packet &tNetPacket)
{
	saved_bones.net_Load(tNetPacket);
	_flags.set(flSavedData,TRUE);
}

void CSE_PHSkeleton::data_save(NET_Packet &tNetPacket)
{
	saved_bones.net_Save(tNetPacket);
	_flags.set(flSavedData,FALSE);
}

void CSE_PHSkeleton::load(NET_Packet &tNetPacket)
{
	_flags.assign				(tNetPacket.r_u8());
	data_load					(tNetPacket);
}
void CSE_PHSkeleton::UPDATE_Write(NET_Packet &tNetPacket)
{

};

void CSE_PHSkeleton::UPDATE_Read(NET_Packet &tNetPacket)
{

};

void CSE_PHSkeleton::FillProps				(LPCSTR pref, PropItemVec& values)
{

};
