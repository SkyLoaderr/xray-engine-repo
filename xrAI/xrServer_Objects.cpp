////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#ifdef AI_COMPILER
	#include "net_utils.h"
#endif

#include "xrServer_Objects.h"
#include "game_base.h"

#ifdef _EDITOR
	#include "ui_main.h"
	#include "SkeletonAnimated.h"
#endif

////////////////////////////////////////////////////////////////////////////
// CPureServerObject
////////////////////////////////////////////////////////////////////////////
void CPureServerObject::save				(IWriter	&tMemoryStream)
{
	NET_Packet					l_tNetPacket;
	l_tNetPacket.w_begin		(M_UPDATE);
	UPDATE_Write				(l_tNetPacket);
	tMemoryStream.w_u32			(l_tNetPacket.B.count);
	tMemoryStream.w				(l_tNetPacket.B.data,l_tNetPacket.B.count);
}

void CPureServerObject::load				(IReader	&tFileStream)
{
	NET_Packet					l_tNetPacket;
	u16							l_wType;
	l_tNetPacket.B.count		= tFileStream.r_u32();
	tFileStream.r				(l_tNetPacket.B.data,l_tNetPacket.B.count);
	l_tNetPacket.r_begin		(l_wType);
	UPDATE_Read					(l_tNetPacket);
}

void CPureServerObject::load				(NET_Packet	&tNetPacket)
{
	UPDATE_Read					(tNetPacket);
}

void CPureServerObject::save				(NET_Packet	&tNetPacket)
{
	UPDATE_Write				(tNetPacket);
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
	s_flags.set					(0);
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
	tNetPacket.w_string			(s_name			);
	tNetPacket.w_string			(s_name_replace	);
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
	tNetPacket.r_string			(s_name			);
	tNetPacket.r_string			(s_name_replace	);
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

#ifdef _EDITOR
xr_token game_types[]={
	{ "Any game",		GAME_ANY		},
	{ "Single",			GAME_SINGLE		},
	{ "Deathmatch",		GAME_DEATHMATCH },
	{ "CTF",			GAME_CTF		},
	{ "Assault",		GAME_ASSAULT	},
	{ "Counter Strike",	GAME_CS			},
	{ "TeamDeathmatch",	GAME_TEAMDEATHMATCH },
	{ "ArtefactHunt",	GAME_ARTEFACTHUNT },
	{ 0,				0				}
};

void CSE_Abstract::FillProp					(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateToken<u8>		(items,	FHelper.PrepareKey(pref,"Game Type"),			&s_gameid,		game_types);
    PHelper.CreateU16			(items,	FHelper.PrepareKey(pref, "Respawn Time (s)"),	&RespawnTime,	0,43200);
}
#endif

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
CSE_Visual::CSE_Visual					(LPCSTR name)
{
	strcpy						(visual_name,name?name:"");
#ifdef _EDITOR
	play_animation				= "$editor";
	visual						= 0;
	OnChangeVisual				(0);
#endif
}

CSE_Visual::~CSE_Visual						()
{
}

void CSE_Visual::set_visual					(LPCSTR name, bool load)
{
	strcpy						(visual_name,name); if (strext(visual_name)) *strext(visual_name) = 0;
#ifdef _EDITOR
	if (load)					OnChangeVisual(0);
#endif
}

void CSE_Visual::visual_read				(NET_Packet	&tNetPacket)
{
	tNetPacket.r_string			(visual_name);
#ifdef _EDITOR
	OnChangeVisual				(0);
#endif
}

void CSE_Visual::visual_write				(NET_Packet	&tNetPacket)
{
	tNetPacket.w_string			(visual_name);
}

#ifdef _EDITOR
void CSE_Visual::PlayAnimation		(LPCSTR name)
{
    // play motion if skeleton
    if (PSkeletonAnimated(visual)){ 
	    play_animation			= name;
        CMotionDef				*M = PSkeletonAnimated(visual)->ID_Cycle_Safe(play_animation.c_str());
        if (M)					PSkeletonAnimated(visual)->PlayCycle(M); 
    }
    if (PKinematics(visual))	PKinematics(visual)->Calculate();
/*
    PSkeletonAnimated(visual)->LL_SetBonesVisible(0);
    PSkeletonAnimated(visual)->LL_SetBoneVisible(1,TRUE,TRUE);
    PSkeletonAnimated(visual)->LL_SetBoneRoot(1);
*/
//        PSkeletonAnimated(visual)->LL_SetBonesVisible((1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6)|(1<<7)|(1<<8)|(1<<9));
//        PSkeletonAnimated(visual)->LL_SetBoneRoot(1);
}

void __fastcall	CSE_Visual::OnChangeVisual	(PropValue* sender)
{
	::Render->model_Delete		(visual,TRUE);
    if (visual_name[0]) {
        visual					= ::Render->model_Create(visual_name);
        PlayAnimation			(play_animation.c_str());
    }
	UI->Command					(COMMAND_UPDATE_PROPERTIES);
}

void CSE_Visual::FillProp		(LPCSTR pref, PropItemVec& values)
{
    PropValue					*V = PHelper.CreateChoose(values, FHelper.PrepareKey(pref,"Model"),visual_name,sizeof(visual_name), smGameObject);
    V->OnChangeEvent			= OnChangeVisual;
}
#endif

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
	tNetPacket.r_string			(motion_name);
#ifdef _EDITOR
	OnChangeMotion				(0);
#endif
}

void CSE_Motion::motion_write	(NET_Packet	&tNetPacket)
{
	tNetPacket.w_string			(motion_name);
}

#ifdef _EDITOR
#include "ObjectAnimator.h"
void CSE_Motion::PlayMotion(LPCSTR name)
{
    // play motion if skeleton
    if (animator) animator->Play(name);
}

void __fastcall	CSE_Motion::OnChangeMotion	(PropValue* sender)
{
	xr_delete					(animator);
    if (*motion_name) {
        animator				= xr_new<CObjectAnimator>();
        animator->Load			(*motion_name);
        PlayMotion				();
    }
	UI->Command					(COMMAND_UPDATE_PROPERTIES);
}

void CSE_Motion::FillProp		(LPCSTR pref, PropItemVec& values)
{
    PropValue					*V = PHelper.CreateChoose(values, FHelper.PrepareKey(pref,"Motion"),&motion_name, smGameAnim);
    V->OnChangeEvent			= OnChangeMotion;
}
#endif

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
		tNetPacket.r_string		(str);
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
		tNetPacket.w_string		(A.event);
	}
}

#ifdef _EDITOR
void CSE_Event::FillProp	(LPCSTR pref, PropItemVec& values)
{
}
#endif

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

#ifdef _EDITOR
void CSE_Spectator::FillProp				(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProp			(pref,items);
}
#endif

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

#ifdef _EDITOR
void CSE_Target::FillProp					(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
}
#endif

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

#ifdef _EDITOR
void CSE_TargetAssault::FillProp			(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
}
#endif

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

#ifdef _EDITOR
void CSE_Target_CS_Base::FillProp			(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref,items);
    PHelper.CreateFloat			(items,FHelper.PrepareKey(pref,s_name,"Radius"),	&radius,1.f,100.f);
    PHelper.CreateU8			(items,FHelper.PrepareKey(pref,s_name,"Team"),		&s_team,0,1);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_Target_CS_Cask
////////////////////////////////////////////////////////////////////////////
CSE_Target_CS_Cask::CSE_Target_CS_Cask		(LPCSTR caSection) : CSE_Target(caSection)
{
	s_Model[0]					= 0;
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
	tNetPacket.r_string			(s_Model);
}

void CSE_Target_CS_Cask::STATE_Write		(NET_Packet	&tNetPacket)
{
	tNetPacket.w_string			(s_Model);
}

#ifdef _EDITOR
void CSE_Target_CS_Cask::FillProp			(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateChoose		(items, FHelper.PrepareKey(pref,s_name,"Model"),	s_Model,	sizeof(s_Model), smGameObject);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_Target_CS
////////////////////////////////////////////////////////////////////////////
CSE_Target_CS::CSE_Target_CS				(LPCSTR caSection) : CSE_Target(caSection)
{
	s_Model[0]					= 0;
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
	tNetPacket.r_string			(s_Model);
}

void CSE_Target_CS::STATE_Write				(NET_Packet	&tNetPacket)
{
	tNetPacket.w_string			(s_Model);
}

#ifdef _EDITOR
void CSE_Target_CS::FillProp				(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateChoose		(items, FHelper.PrepareKey(pref,s_name,"Model"),	s_Model,	sizeof(s_Model), smGameObject);
}
#endif

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

#ifdef _EDITOR
void CSE_Temporary::FillProp				(LPCSTR pref, PropItemVec& values)
{
};

#endif

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

#ifdef _EDITOR
void CSE_SpawnGroup::FillProp				(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
	PHelper.CreateFloat			(values,FHelper.PrepareKey(pref,s_name,"ALife\\Group probability"),	&m_fGroupProbability,0.f,1.f);
};

#endif
