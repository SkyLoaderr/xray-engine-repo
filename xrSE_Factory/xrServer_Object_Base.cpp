////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Object_Base.cpp
//	Created 	: 19.09.2002
//  Modified 	: 16.07.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server base object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects.h"
#include "xrMessages.h"
#include "game_base_space.h"
#include "script_value_container_impl.h"

#ifndef AI_COMPILER
#	include "object_factory.h"
#endif

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

LPCSTR script_section = "script";
LPCSTR current_version = "current_server_entity_version";

IC	u16	script_server_object_version	()
{
	static bool initialized		= false;
	static u16  script_version	= 0;
	if (!initialized) {
		initialized				= true;
		if (!pSettings->section_exist(script_section) || !pSettings->line_exist(script_section,current_version))
			script_version		= 0;
		script_version			= pSettings->r_u16(script_section,current_version);
	}
	return						(script_version);
}

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
	m_editor_flags.zero			();
	RespawnTime					= 0;
	net_Ready					= FALSE;
	ID							= 0xffff;
	ID_Parent					= 0xffff;
	ID_Phantom					= 0xffff;
	owner						= 0;
	s_gameid					= 0;
	s_RP						= 0xFE;			// Use supplied coords
	s_flags.assign				(0);
	s_name						= caSection;
	s_name_replace				= 0;
	o_Angle.set					(0.f,0.f,0.f);
	o_Position.set				(0.f,0.f,0.f);
	m_bALifeControl				= false;
	m_wVersion					= 0;
	m_script_version			= 0;
	m_tClassID					= TEXT2CLSID(pSettings->r_string(caSection,"class"));

	m_spawn_probability			= 1.f;
	m_spawn_flags.zero			();
	m_spawn_flags.set			(flSpawnActive			,TRUE);
	m_spawn_flags.set			(flSpawnOnSurgeOnly		,TRUE);
	m_spawn_flags.set			(flSpawnSingleItemOnly	,TRUE);
	m_spawn_flags.set			(flSpawnIfDestroyedOnly	,TRUE);
	m_spawn_flags.set			(flSpawnInfiniteCount	,TRUE);
	m_max_spawn_count			= 1;
	m_spawn_control				= "";
	m_spawn_count				= 0;
	m_last_spawn_time			= 0;
	m_min_spawn_interval		= 0;
	m_max_spawn_interval		= 0;

#ifndef AI_COMPILER
	m_script_clsid				= object_factory().script_clsid(m_tClassID);
#endif

	client_data.clear			();
}

CSE_Abstract::~CSE_Abstract					()
{
	xr_free						(s_name_replace);
}

CSE_Visual* CSE_Abstract::visual			()
{
	return						(0);
}

ISE_Shape*  CSE_Abstract::shape				()
{
	return						(0);
}

CSE_Motion* CSE_Abstract::motion			()
{
	return						(0);
}
	
void CSE_Abstract::Spawn_Write				(NET_Packet	&tNetPacket, BOOL bLocal)
{
	LPCSTR						temp;
	
	temp						= pSettings->r_string("rat_group","monster_section");
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
	
	tNetPacket.w_u16			(script_server_object_version());


	//client object custom data serialization SAVE
	u8 client_data_size			= (u8)client_data.size(); //не может быть больше 256 байт
	tNetPacket.w_u8				(client_data_size);
	if(client_data_size>0)
		tNetPacket.w			(&*client_data.begin(),client_data_size);

	tNetPacket.w				(&m_tSpawnID,	sizeof(m_tSpawnID));
	tNetPacket.w_float			(m_spawn_probability);
	tNetPacket.w_u32			(m_spawn_flags.get());
	tNetPacket.w_stringZ		(m_spawn_control);
	tNetPacket.w_u32			(m_max_spawn_count);
	tNetPacket.w_u32			(m_spawn_count);
	tNetPacket.w_u64			(m_last_spawn_time);
	tNetPacket.w_u64			(m_min_spawn_interval);
	tNetPacket.w_u64			(m_max_spawn_interval);

#ifdef XRSE_FACTORY_EXPORTS
	CScriptValueContainer::assign();
#endif

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

	if (m_wVersion > 69)
		m_script_version		= tNetPacket.r_u16();

	// read specific data

	//client object custom data serialization LOAD
	if (m_wVersion > 70)
	{
		u8 client_data_size		= tNetPacket.r_u8(); //не может быть больше 256 байт
		if(client_data_size>0)
		{
			client_data.resize	(client_data_size);
			tNetPacket.r		(&*client_data.begin(),client_data_size);
		}
		else
			client_data.clear();
	}else
		client_data.clear		();

	if (m_wVersion > 79)
		tNetPacket.r			(&m_tSpawnID,	sizeof(m_tSpawnID));

	if (m_wVersion > 82)
		tNetPacket.r_float		(m_spawn_probability);

	if (m_wVersion > 83) {
		m_spawn_flags.assign	(tNetPacket.r_u32());
		tNetPacket.r_stringZ	(m_spawn_control);
		tNetPacket.r_u32		(m_max_spawn_count);
		tNetPacket.r_u32		(m_spawn_count);
		tNetPacket.r_u64		(m_last_spawn_time);
	}

	if (m_wVersion > 84) {
		tNetPacket.r_u64		(m_min_spawn_interval);
		tNetPacket.r_u64		(m_max_spawn_interval);
	}

	u16							size;
	tNetPacket.r_u16			(size			);	// size
	STATE_Read					(tNetPacket,size);
    
	return						TRUE;
}

void	CSE_Abstract::load			(NET_Packet	&tNetPacket)
{
	CPureServerObject::load	(tNetPacket);
	u8 client_data_size		= tNetPacket.r_u8(); //не может быть больше 256 байт
	if(client_data_size>0)
	{
		client_data.resize	(client_data_size);
		tNetPacket.r		(&*client_data.begin(),client_data_size);
	}
	else
        client_data.clear();
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

LPCSTR		CSE_Abstract::name			()
{
	return	(*s_name);
}

void		CSE_Abstract::set_name		(LPCSTR new_name)
{
	s_name	;
}

LPCSTR		CSE_Abstract::name_replace	()
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

void CSE_Abstract::FillProps				(LPCSTR pref, PropItemVec& items)
{
	PHelper().CreateToken8		(items,	PrepareKey(pref,"Game Type"),			&s_gameid,		game_types);
    PHelper().CreateU16			(items,	PrepareKey(pref, "Respawn Time (s)"),	&RespawnTime,	0,43200);

	PHelper().CreateFloat		(items,PrepareKey(pref,*s_name,"Spawn parameters\\Spawn probability"),			&m_spawn_probability,	0.f,			1.f);
	PHelper().CreateFlag32		(items,PrepareKey(pref,*s_name,"Spawn parameters\\Spawn is active"),			&m_spawn_flags,			flSpawnActive);
	PHelper().CreateFlag32		(items,PrepareKey(pref,*s_name,"Spawn parameters\\Spawn on surge only"),		&m_spawn_flags,			flSpawnOnSurgeOnly);
	PHelper().CreateFlag32		(items,PrepareKey(pref,*s_name,"Spawn parameters\\Spawn if destroyed only"),	&m_spawn_flags,			flSpawnIfDestroyedOnly);
	PHelper().CreateFlag32		(items,PrepareKey(pref,*s_name,"Spawn parameters\\Spawn infinite count"),		&m_spawn_flags,			flSpawnInfiniteCount);
	PHelper().CreateFlag32		(items,PrepareKey(pref,*s_name,"Spawn parameters\\Auto destroy on spawn"),		&m_spawn_flags,			flSpawnDestroyOnSpawn);

	LPCSTR						gcs = pSettings->r_string(s_name,"GroupControlSection");
	PHelper().CreateChoose		(items,PrepareKey(pref,*s_name,"Spawn parameters\\Group control"),				&m_spawn_control,		smSpawnItem,	0,	(void*)gcs,	16);

//	PHelper().CreateTime		(values,PrepareKey(pref,*s_name,"Spawn parameters\\Min spawn interval"),	&m_min_spawn_interval);
//	PHelper().CreateTime		(values,PrepareKey(pref,*s_name,"Spawn parameters\\Max spawn interval"),	&m_max_spawn_interval);
}

void CSE_Abstract::FillProp					(LPCSTR pref, PropItemVec &items)
{
#ifdef XRSE_FACTORY_EXPORTS
	CScriptValueContainer::assign();
	CScriptValueContainer::clear();
#endif
	FillProps					(pref,items);
}
