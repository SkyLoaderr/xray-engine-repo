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
#include "xrMessages.h"

#ifndef XRSE_FACTORY_EXPORTS
#	include "xrEProps.h"
	
	IPropHelper &PHelper()
	{
		NODEFAULT;
#	ifdef DEBUG
		return(*(IPropHelper*)0);
#	endif
	}
#endif

////////////////////////////////////////////////////////////////////////////
// CPureServerObject
////////////////////////////////////////////////////////////////////////////
void CPureServerObject::save				(IWriter	&tMemoryStream)
{
}

void CPureServerObject::load				(IReader	&tFileStream)
{
}

void CPureServerObject::load				(NET_Packet	&tNetPacket)
{
}

void CPureServerObject::save				(NET_Packet	&tNetPacket)
{
}

////////////////////////////////////////////////////////////////////////////
// CSE_Abstract
////////////////////////////////////////////////////////////////////////////
CSE_Abstract::CSE_Abstract					(LPCSTR caSection)
{
	RespawnTime					= 0;
	net_Ready					= FALSE;
	ID							= 0xffff;
	ID_Parent					= 0xffff;
	ID_Phantom					= 0xffff;
	owner						= 0;
	s_gameid					= 0;
	s_RP						= 0xFE;			// Use supplied coords
	s_flags.assign				(0);
	Memory.mem_copy				(s_name,caSection,((u32)xr_strlen(caSection) + 1)*sizeof(char));
	ZeroMemory					(s_name_replace,sizeof(string64));
	o_Angle.set					(0.f,0.f,0.f);
	o_Position.set				(0.f,0.f,0.f);
	m_bALifeControl				= false;
	m_wVersion					= 0;
}

CSE_Abstract::~CSE_Abstract					()
{
}
	
void CSE_Abstract::Spawn_Write				(NET_Packet	&tNetPacket, BOOL bLocal)
{
	// generic
	tNetPacket.w_begin			(M_SPAWN);
	tNetPacket.w_stringZ		(s_name			);
	tNetPacket.w_stringZ		(s_name_replace	);
	tNetPacket.w_u8				(s_gameid		);
	tNetPacket.w_u8				(s_RP			);
	tNetPacket.w_vec3			(o_Position		);
	tNetPacket.w_vec3			(o_Angle		);
	tNetPacket.w_u16			(RespawnTime	);
	tNetPacket.w_u16			(ID				);
	tNetPacket.w_u16			(ID_Parent		);
	tNetPacket.w_u16			(ID_Phantom		);
	
	s_flags.set					(M_SPAWN_VERSION,TRUE);
	if (bLocal)
		tNetPacket.w_u16		(u16(s_flags.flags|M_SPAWN_OBJECT_LOCAL) );
	else
		tNetPacket.w_u16		(u16(s_flags.flags&~(M_SPAWN_OBJECT_LOCAL|M_SPAWN_OBJECT_ASPLAYER)));
	
	tNetPacket.w_u16			(SPAWN_VERSION);

	// write specific data
	u32	position				= tNetPacket.w_tell();
	tNetPacket.w_u16			(0);
	STATE_Write					(tNetPacket);
	u16 size					= u16(tNetPacket.w_tell()-position);
	tNetPacket.w_seek			(position,&size,sizeof(u16));
}

BOOL CSE_Abstract::Spawn_Read				(NET_Packet	&tNetPacket)
{
	u16							dummy16;
	// generic
	tNetPacket.r_begin			(dummy16);	
	R_ASSERT					(M_SPAWN==dummy16);
	tNetPacket.r_stringZ		(s_name			);
	tNetPacket.r_stringZ		(s_name_replace	);
	tNetPacket.r_u8				(s_gameid		);
	tNetPacket.r_u8				(s_RP			);
	tNetPacket.r_vec3			(o_Position		);
	tNetPacket.r_vec3			(o_Angle		);
	tNetPacket.r_u16			(RespawnTime	);
	tNetPacket.r_u16			(ID				);
	tNetPacket.r_u16			(ID_Parent		);
	tNetPacket.r_u16			(ID_Phantom		);
	tNetPacket.r_u16			(s_flags.flags	); 
	
	// dangerous!!!!!!!!!
	if (s_flags.is(M_SPAWN_VERSION))
		tNetPacket.r_u16		(m_wVersion);
	
	if (0==m_wVersion) {
		tNetPacket.r_pos		-= sizeof(u16);
		m_wVersion				= 0;
        return					FALSE;
	}
	// this is "zaplatka"

	// read specific data
	u16							size;
	tNetPacket.r_u16			(size			);	// size
	STATE_Read					(tNetPacket,size);
    
	return						TRUE;
}

CSE_Abstract *CSE_Abstract::base	()
{
	return						(this);
}

const CSE_Abstract *CSE_Abstract::base	() const
{
	return						(this);
}

CSE_Abstract *CSE_Abstract::init	()
{
	return						(this);
}

LPSTR		CSE_Abstract::name			()
{
	return	(s_name);
}

LPSTR		CSE_Abstract::name_replace	()
{
	return	(s_name_replace);
}

Fvector&	CSE_Abstract::position		()
{
	return	(o_Position);
}

Fvector&	CSE_Abstract::angle			()
{
	return	(o_Angle);
}

Flags16&	CSE_Abstract::flags			()
{
	return	(s_flags);
}

xr_token game_types[]={
	{ "Any game",		GAME_ANY		},
	{ "Single",			GAME_SINGLE		},
	{ "Deathmatch",		GAME_DEATHMATCH },
//	{ "CTF",			GAME_CTF		},
//	{ "Assault",		GAME_ASSAULT	},
	{ "Counter Strike",	GAME_CS			},
	{ "TeamDeathmatch",	GAME_TEAMDEATHMATCH },
	{ "ArtefactHunt",	GAME_ARTEFACTHUNT },
	{ 0,				0				}
};

void CSE_Abstract::FillProp					(LPCSTR pref, PropItemVec& items)
{
	PHelper().CreateToken8		(items,	PrepareKey(pref,"Game Type"),			&s_gameid,		game_types);
    PHelper().CreateU16			(items,	PrepareKey(pref, "Respawn Time (s)"),	&RespawnTime,	0,43200);
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

void CSE_Event::FillProp	(LPCSTR pref, PropItemVec& values)
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

void CSE_Spectator::FillProp				(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProp			(pref,items);
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

void CSE_Target::FillProp					(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
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

void CSE_TargetAssault::FillProp			(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
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

void CSE_Target_CS_Base::FillProp			(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref,items);
    PHelper().CreateFloat		(items,PrepareKey(pref,s_name,"Radius"),	&radius,1.f,100.f);
    PHelper().CreateU8			(items,PrepareKey(pref,s_name,"Team"),		&s_team,0,1);
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

void CSE_Target_CS_Cask::FillProp			(LPCSTR pref, PropItemVec& items)
{
	PHelper().CreateChoose		(items, PrepareKey(pref,s_name,"Model"),	&s_Model,	smGameObject);
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

void CSE_Target_CS::FillProp				(LPCSTR pref, PropItemVec& items)
{
	PHelper().CreateChoose		(items, PrepareKey(pref,s_name,"Model"),	&s_Model,	smGameObject);
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

void CSE_Temporary::FillProp				(LPCSTR pref, PropItemVec& values)
{
};

////////////////////////////////////////////////////////////////////////////
// CSE_SpawnGroup
////////////////////////////////////////////////////////////////////////////

CSE_SpawnGroup::CSE_SpawnGroup				(LPCSTR caSection) : CSE_Abstract(caSection)
{
	m_fGroupProbability						= 1.f;
}

CSE_SpawnGroup::~CSE_SpawnGroup				()
{
}

void CSE_SpawnGroup::STATE_Read				(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_float						(m_fGroupProbability);
};

void CSE_SpawnGroup::STATE_Write			(NET_Packet	&tNetPacket)
{
	tNetPacket.w_float						(m_fGroupProbability);
};

void CSE_SpawnGroup::UPDATE_Read			(NET_Packet	&tNetPacket)
{
};

void CSE_SpawnGroup::UPDATE_Write			(NET_Packet	&tNetPacket)
{
};

void CSE_SpawnGroup::FillProp				(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp				(pref,values);
	PHelper().CreateFloat			(values,PrepareKey(pref,s_name,"ALife\\Group probability"),	&m_fGroupProbability,0.f,1.f);
};

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

	tNetPacket.r_stringZ	(startup_animation);
	tNetPacket.r_u8			(_flags.flags);
	tNetPacket.r_u16		(source_id);
	if (_flags.test(flSavedData)) {
		data_load(tNetPacket);
	}
}

void CSE_PHSkeleton::STATE_Write		(NET_Packet	&tNetPacket)
{
	tNetPacket.w_stringZ		(startup_animation);
	tNetPacket.w_u8				(_flags.flags);
	tNetPacket.w_u16			(source_id);
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

void CSE_PHSkeleton::FillProp				(LPCSTR pref, PropItemVec& values)
{

};
