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
	tNetPacket.w_stringZ			(s_name			);
	tNetPacket.w_stringZ			(s_name_replace	);
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
	PHelper().CreateToken8		(items,	PHelper().PrepareKey(pref,"Game Type"),			&s_gameid,		game_types);
    PHelper().CreateU16			(items,	PHelper().PrepareKey(pref, "Respawn Time (s)"),	&RespawnTime,	0,43200);
}

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

////////////////////////////////////////////////////////////////////////////
// CSE_Visual
////////////////////////////////////////////////////////////////////////////
CSE_Visual::CSE_Visual		   	(LPCSTR name)
{
	visual_name					= name;
#ifdef _EDITOR
	play_animation				= "$editor";
	visual						= 0;
	OnChangeVisual				(0);
#endif
}

CSE_Visual::~CSE_Visual			()
{
}

void CSE_Visual::set_visual	   	(LPCSTR name, bool load)
{
//.
	string_path tmp;
    strcpy						(tmp,name);
    if (strext(tmp))		 	*strext(tmp) = 0;
	visual_name					= tmp; 
#ifdef _EDITOR
	if (load)					OnChangeVisual(0);
#endif
}

void CSE_Visual::visual_read   	(NET_Packet	&tNetPacket)
{
	tNetPacket.r_stringZ		(visual_name);
#ifdef _EDITOR
	OnChangeVisual				(0);
#endif
}

void CSE_Visual::visual_write  	(NET_Packet	&tNetPacket)
{
	tNetPacket.w_stringZ			(visual_name);
}

#ifdef _EDITOR
void CSE_Visual::PlayAnimation 	(LPCSTR name)
{
    // play motion if skeleton
    if (PSkeletonAnimated(visual)){ 
	    play_animation			= name;
        CMotionDef				*M = PSkeletonAnimated(visual)->ID_Cycle_Safe(play_animation.c_str());
        if (M)					PSkeletonAnimated(visual)->PlayCycle(M); 
    }
    if (PKinematics(visual))	PKinematics(visual)->CalculateBones();
/*
    PSkeletonAnimated(visual)->LL_SetBonesVisible(0);
    PSkeletonAnimated(visual)->LL_SetBoneVisible(1,TRUE,TRUE);
    PSkeletonAnimated(visual)->LL_SetBoneRoot(1);
*/
//        PSkeletonAnimated(visual)->LL_SetBonesVisible((1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6)|(1<<7)|(1<<8)|(1<<9));
//        PSkeletonAnimated(visual)->LL_SetBoneRoot(1);
}

void CSE_Visual::OnChangeVisual	(PropValue* sender)
{
	::Render->model_Delete		(visual,TRUE);
    if (visual_name.size()) {
        visual					= ::Render->model_Create(visual_name.c_str());
        PlayAnimation			(play_animation.c_str());
    }
//.	UI->Command					(COMMAND_UPDATE_PROPERTIES);
}
#endif

void CSE_Visual::FillProp		(LPCSTR pref, PropItemVec& values)
{
#ifdef _EDITOR
	ChooseValue *V 				= PHelper().CreateChoose(values, PHelper().PrepareKey(pref,"Model"),&visual_name, smGameObject);
	V->OnChangeEvent.bind		(this,&CSE_Visual::OnChangeVisual);
#endif
}

////////////////////////////////////////////////////////////////////////////
// CSE_Animated
////////////////////////////////////////////////////////////////////////////
CSE_Motion::CSE_Motion			(LPCSTR name)
{
	motion_name					= name;
#ifdef _EDITOR
	animator					= 0;
	OnChangeMotion				(0);
#endif
}

CSE_Motion::~CSE_Motion			()
{
}

void CSE_Motion::set_motion		(LPCSTR name)
{
	motion_name					= name;
#ifdef _EDITOR
	OnChangeMotion				(0);
#endif
}

void CSE_Motion::motion_read	(NET_Packet	&tNetPacket)
{
	tNetPacket.r_stringZ		(motion_name);
#ifdef _EDITOR
	OnChangeMotion				(0);
#endif
}

void CSE_Motion::motion_write	(NET_Packet	&tNetPacket)
{
	tNetPacket.w_stringZ			(motion_name);
}

#ifdef _EDITOR
#include "ObjectAnimator.h"
void CSE_Motion::PlayMotion(LPCSTR name)
{
    // play motion if skeleton
    if (animator) animator->Play(name);
}

void CSE_Motion::OnChangeMotion	(PropValue* sender)
{
	xr_delete					(animator);
    if (*motion_name) {
        animator				= xr_new<CObjectAnimator>();
        animator->Load			(*motion_name);
        PlayMotion				();
    }
	UI->Command					(COMMAND_UPDATE_PROPERTIES);
}
#endif

void CSE_Motion::FillProp		(LPCSTR pref, PropItemVec& values)
{
#ifdef _EDITOR
    PropValue					*V = PHelper().CreateChoose(values, PHelper().PrepareKey(pref,"Motion"),&motion_name, smGameAnim);
    V->OnChangeEvent.bind		(this,&CSE_Motion::OnChangeMotion);
#endif
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
    PHelper().CreateFloat			(items,PHelper().PrepareKey(pref,s_name,"Radius"),	&radius,1.f,100.f);
    PHelper().CreateU8			(items,PHelper().PrepareKey(pref,s_name,"Team"),		&s_team,0,1);
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
	PHelper().CreateChoose		(items, PHelper().PrepareKey(pref,s_name,"Model"),	&s_Model,	smGameObject);
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
	PHelper().CreateChoose		(items, PHelper().PrepareKey(pref,s_name,"Model"),	&s_Model,	smGameObject);
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
	PHelper().CreateFloat			(values,PHelper().PrepareKey(pref,s_name,"ALife\\Group probability"),	&m_fGroupProbability,0.f,1.f);
};

////////////////////////////////////////////////////////////////////////////
// CSE_PHSkeleton
////////////////////////////////////////////////////////////////////////////
CSE_PHSkeleton::CSE_PHSkeleton(LPCSTR caSection)
{
	source_id					= u16(-1);
	flags.zero					();
}

CSE_PHSkeleton::~CSE_PHSkeleton()
{

}

void CSE_PHSkeleton::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{

	tNetPacket.r_stringZ	(startup_animation);
	tNetPacket.r_u8			(flags.flags);
	tNetPacket.r_u16		(source_id);
	if (flags.test(flSavedData)) {
		data_load(tNetPacket);
	}
}

void CSE_PHSkeleton::STATE_Write		(NET_Packet	&tNetPacket)
{
	tNetPacket.w_stringZ			(startup_animation);
	tNetPacket.w_u8				(flags.flags);
	tNetPacket.w_u16			(source_id);
	////////////////////////saving///////////////////////////////////////
	if(flags.test(flSavedData))
	{
		data_save(tNetPacket);
	}
}

void CSE_PHSkeleton::data_load(NET_Packet &tNetPacket)
{
	saved_bones.net_Load(tNetPacket);
	flags.set(flSavedData,TRUE);
}

void CSE_PHSkeleton::data_save(NET_Packet &tNetPacket)
{
	saved_bones.net_Save(tNetPacket);
	flags.set(flSavedData,FALSE);
}

void CSE_PHSkeleton::load(NET_Packet &tNetPacket)
{
	flags.assign				(tNetPacket.r_u8());
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
