////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Retsky and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma hdrstop

#include "xrServer_Objects.h"
#include "game_base.h"

#ifdef _EDITOR
	#include "ui_main.h"
	#include "BodyInstance.h"
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_Abstract
////////////////////////////////////////////////////////////////////////////
void CSE_Abstract::Spawn_Write		(NET_Packet	&tNetPacket, BOOL bLocal)
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

BOOL CSE_Abstract::Spawn_Read		(NET_Packet	&tNetPacket)
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
	{ 0,				0				}
};

void CSE_Abstract::FillProp		(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateToken			(items,	PHelper.PrepareKey(pref,"Game Type"),			&s_gameid,		game_types, 1);
    PHelper.CreateFlag16		(items,	PHelper.PrepareKey(pref, "Active"),				&s_flags, 		M_SPAWN_OBJECT_ACTIVE);
    PHelper.CreateU16			(items,	PHelper.PrepareKey(pref, "Respawn Time (s)"),	&RespawnTime,	0,43200);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_Shape
////////////////////////////////////////////////////////////////////////////
void CSE_Shape::cform_read			(NET_Packet	&tNetPacket)
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
		count--;
	}
}

void CSE_Shape::cform_write		(NET_Packet	&tNetPacket)
{
	tNetPacket.w_u8				(u8(shapes.size()));
	for (u32 i=0; i<shapes.size(); i++) {
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
void CSE_Visual::set_visual		(LPCSTR name)
{
	strcpy						(visual_name,name);
#ifdef _EDITOR
	OnChangeVisual				(0);
#endif
}

void CSE_Visual::visual_read		(NET_Packet	&tNetPacket)
{
	tNetPacket.r_string			(visual_name);
#ifdef _EDITOR
	OnChangeVisual				(0);
#endif
}

void CSE_Visual::visual_write		(NET_Packet	&tNetPacket)
{
	tNetPacket.w_string			(visual_name);
}

#ifdef _EDITOR
void CSE_Visual::PlayAnimation		(LPCSTR name)
{
    // play motion if skeleton
    if (PKinematics(visual)){ 
	    play_animation			= name;
        CMotionDef				*M = PKinematics(visual)->ID_Cycle_Safe(play_animation.c_str());
        if (M)
			PKinematics(visual)->PlayCycle(M); 
        PKinematics(visual)->Calculate();
    }
}

void __fastcall	CSE_Visual::OnChangeVisual(PropValue* sender)
{
	Device.Models.Delete		(visual);
    if (visual_name[0]) {
        visual					= Device.Models.Create(visual_name);
        PlayAnimation			(play_animation.c_str());
    }
	UI.Command					(COMMAND_UPDATE_PROPERTIES);
}

void CSE_Visual::FillProp			(LPCSTR pref, PropItemVec& values)
{
    PropValue					*V = PHelper.CreateGameObject(values, PHelper.PrepareKey(pref,"Model"),visual_name,sizeof(visual_name));
    V->SetEvents				(0,0,OnChangeVisual);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_Event
////////////////////////////////////////////////////////////////////////////
void CSE_Event::UPDATE_Read		(NET_Packet	&tNetPacket)
{
}

void CSE_Event::UPDATE_Write		(NET_Packet	&tNetPacket)
{
}

void CSE_Event::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
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
		count--;
	}
}

void CSE_Event::STATE_Write		(NET_Packet	&tNetPacket)
{
	// CForm
	cform_write					(tNetPacket);

	// Actions
	tNetPacket.w_u8				(u8(Actions.size()));
	for (u32 i=0; i<Actions.size(); i++) {
		tAction&				A = Actions[i];
		tNetPacket.w_u8			(A.type	);
		tNetPacket.w_u16		(A.count);
		tNetPacket.w_u64		(A.cls	);
		tNetPacket.w_string		(A.event);
	}
}

////////////////////////////////////////////////////////////////////////////
// CSE_Dummy
////////////////////////////////////////////////////////////////////////////
CSE_Dummy::CSE_Dummy(LPCSTR caSection) : CSE_Abstract(caSection)
{
	s_Animation					= 0;
	s_Model						= 0;
	s_Particles					= 0;
	s_Sound						= 0;
}
CSE_Dummy::~CSE_Dummy()
{
	xr_free						(s_Animation	);
	xr_free						(s_Model		);
	xr_free						(s_Particles	);
	xr_free						(s_Sound		);
}
void CSE_Dummy::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_u8				(s_style);

	if (s_style&esAnimated)		{
		// Load animator
		string256				fn;
		tNetPacket.r_string		(fn);
		s_Animation				= xr_strdup(fn);
	}
	if (s_style & esModel) {
		// Load model
		string256				fn;
		tNetPacket.r_string		(fn);
		s_Model					= xr_strdup(fn);
	}
	if (s_style & esParticles) {
		// Load model
		string256				fn;
		tNetPacket.r_string		(fn);
		s_Particles				= xr_strdup(fn);
	}
	if (s_style & esSound) {
		// Load model
		string256				fn;
		tNetPacket.r_string		(fn);
		s_Sound					= xr_strdup(fn);
	}
}
void CSE_Dummy::STATE_Write		(NET_Packet	&tNetPacket)
{
	tNetPacket.w_u8				(s_style		);
	if (s_style&esAnimated)		tNetPacket.w_string			(s_Animation	);
	if (s_style&esModel)		tNetPacket.w_string			(s_Model		);
	if (s_style&esParticles)	tNetPacket.w_string			(s_Particles	);
	if (s_style&esSound)		tNetPacket.w_string			(s_Sound		);
}
void CSE_Dummy::UPDATE_Read		(NET_Packet	&tNetPacket)
{
}

void CSE_Dummy::UPDATE_Write		(NET_Packet	&tNetPacket)
{
}

#ifdef _EDITOR
void CSE_Dummy::FillProp			(LPCSTR pref, PropItemVec& values)
{
  	inherited::FillProp			(pref,values);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_Spectator
////////////////////////////////////////////////////////////////////////////
void CSE_Spectator::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
}

void CSE_Spectator::STATE_Write	(NET_Packet	&tNetPacket)
{
}

void CSE_Spectator::UPDATE_Read	(NET_Packet	&tNetPacket)
{
}

void CSE_Spectator::UPDATE_Write	(NET_Packet	&tNetPacket)
{
}

#ifdef _EDITOR
void CSE_Spectator::FillProp		(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProp			(pref,items);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_Target
////////////////////////////////////////////////////////////////////////////
void CSE_Target::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
};

void CSE_Target::STATE_Write		(NET_Packet	&tNetPacket)
{
};

void CSE_Target::UPDATE_Read		(NET_Packet	&tNetPacket)
{
};

void CSE_Target::UPDATE_Write		(NET_Packet	&tNetPacket)
{
};

#ifdef _EDITOR
void CSE_Target::FillProp			(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_TargetAssault
////////////////////////////////////////////////////////////////////////////
void CSE_TargetAssault::STATE_Read	(NET_Packet	&tNetPacket, u16 size)	
{
};

void CSE_TargetAssault::STATE_Write(NET_Packet	&tNetPacket)
{
};

void CSE_TargetAssault::UPDATE_Read(NET_Packet	&tNetPacket)
{
};

void CSE_TargetAssault::UPDATE_Write(NET_Packet &tNetPacket)
{
};

#ifdef _EDITOR
void CSE_TargetAssault::FillProp	(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_Target_CS_Base
////////////////////////////////////////////////////////////////////////////
CSE_Target_CS_Base::CSE_Target_CS_Base(LPCSTR caSection) : CSE_Target(caSection)
{
	s_team						= 0;
	radius						= 10.f;
    s_gameid					= GAME_CS;    
};

void CSE_Target_CS_Base::STATE_Read(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_float			(radius);
	tNetPacket.r_u8				(s_team);
}

void CSE_Target_CS_Base::STATE_Write(NET_Packet	&tNetPacket)
{
	tNetPacket.w_float			(radius);
	tNetPacket.w_u8				(s_team);
}

void CSE_Target_CS_Base::UPDATE_Read(NET_Packet	&tNetPacket)
{
}

void CSE_Target_CS_Base::UPDATE_Write(NET_Packet	&tNetPacket)
{
}

#ifdef _EDITOR
void CSE_Target_CS_Base::FillProp	(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref,items);
    PHelper.CreateFloat			(items,PHelper.PrepareKey(pref,s_name,"Radius"),	&radius,1.f,100.f);
    PHelper.CreateU8			(items,PHelper.PrepareKey(pref,s_name,"Team"),		&s_team,0,1);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_Target_CS_Cask
////////////////////////////////////////////////////////////////////////////
CSE_Target_CS_Cask::CSE_Target_CS_Cask(LPCSTR caSection) : CSE_Target(caSection)
{
	s_Model[0]					= 0;
}
void CSE_Target_CS_Cask::UPDATE_Read(NET_Packet	&tNetPacket)
{
}

void CSE_Target_CS_Cask::UPDATE_Write(NET_Packet	&tNetPacket)
{
}

void CSE_Target_CS_Cask::STATE_Read(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_string			(s_Model);
}

void CSE_Target_CS_Cask::STATE_Write(NET_Packet	&tNetPacket)
{
	tNetPacket.w_string			(s_Model);
}

#ifdef _EDITOR
void CSE_Target_CS_Cask::FillProp	(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateGameObject	(items, PHelper.PrepareKey(pref,s_name,"Model"),	s_Model,	sizeof(s_Model));
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_Target_CS
////////////////////////////////////////////////////////////////////////////
CSE_Target_CS::CSE_Target_CS(LPCSTR caSection) : CSE_Target(caSection)
{
	s_Model[0]					= 0;
}

void CSE_Target_CS::UPDATE_Read	(NET_Packet	&tNetPacket)
{
}

void CSE_Target_CS::UPDATE_Write	(NET_Packet	&tNetPacket)
{
}

void CSE_Target_CS::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_string			(s_Model);
}

void CSE_Target_CS::STATE_Write	(NET_Packet	&tNetPacket)
{
	tNetPacket.w_string			(s_Model);
}

#ifdef _EDITOR
void CSE_Target_CS::FillProp		(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateGameObject	(items, PHelper.PrepareKey(pref,s_name,"Model"),	s_Model,	sizeof(s_Model));
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_Temporary
////////////////////////////////////////////////////////////////////////////
CSE_Temporary::CSE_Temporary(LPCSTR caSection) : CSE_Abstract(caSection)
{
};

void CSE_Temporary::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
};

void CSE_Temporary::STATE_Write	(NET_Packet	&tNetPacket)
{
};

void CSE_Temporary::UPDATE_Read	(NET_Packet	&tNetPacket)
{
};

void CSE_Temporary::UPDATE_Write	(NET_Packet	&tNetPacket)
{
};

#ifdef _EDITOR
void CSE_Temporary::FillProp		(LPCSTR pref, PropItemVec& values)
{
};

#endif
