#ifndef _INCDEF_XRMESSAGES_H_
#define _INCDEF_XRMESSAGES_H_

#pragma once

// CL	== client 2 server message
// SV	== server 2 client message

enum {
	M_UPDATE			= 0,	// DUAL: Update state
	M_SPAWN,					// DUAL: Spawning, full state

	M_SV_CONFIG_GAME,
	M_SV_CONFIG_FINISHED,

	M_MIGRATE_DEACTIVATE,		// TO:   Changing server, just deactivate
	M_MIGRATE_ACTIVATE,			// TO:   Changing server, full state

	M_CHAT,						// DUAL:

	M_EVENT,					// Game Event
	M_CL_INPUT,					// Client Input Data
	M_CLIENTREADY,				// Client has finished to load level and are ready to play
	M_CHANGE_LEVEL,				// changing level
	MSG_FORCEDWORD				= u32(-1)
};

enum {
	GE_RESPAWN,
	GE_OWNERSHIP_TAKE,			// DUAL: Client request for ownership of an item
	GE_OWNERSHIP_REJECT,		// DUAL: Client request ownership rejection
	GE_TRANSFER_AMMO,			// DUAL: Take ammo out of weapon for our weapon
	GE_HIT,						//
	GE_DIE,						//
	GE_DESTROY,					// authorative client request for entity-destroy

	GE_BUY,

	GE_PDA,						//a PDA message sent from one PDA to another

	GE_INFO_TRANSFER,			//transfer _new_ info on PDA
	
	GE_TRADE_SELL,
	GE_TRADE_BUY,

	GE_WPN_AMMO_ADD,
	GE_WPN_STATE_CHANGE,

	GE_ADDON_ATTACH,
	GE_ADDON_DETACH,
	
	GE_GRENADE_EXPLODE,
	GE_INV_ACTION,				//a action beign taken on inventory

	GEG_SIGNAL,
	GEG_PLAYER_READY,
	GEG_PLAYER_CHANGE_TEAM,
	GEG_PLAYER_KILL,			//player wants to die
	GEG_PLAYER_BUY_FINISHED,	//player end to buy items

	GE_FORCEDWORD				= u32(-1)
};

enum
{
	M_SPAWN_OBJECT_LOCAL		= (1<<0),	// after spawn it becomes local (authorative)
	M_SPAWN_OBJECT_HASUPDATE	= (1<<2),	// after spawn info it has update inside message
	M_SPAWN_OBJECT_ASPLAYER		= (1<<3),	// after spawn it must become viewable
	M_SPAWN_OBJECT_PHANTOM		= (1<<4),	// after spawn it must become viewable
	M_SPAWN_VERSION				= (1<<5),	// control version
	M_SPAWN_UPDATE				= (1<<6),	// + update packet

	M_SPAWN_OBJECT_FORCEDWORD	= u32(-1)
};

enum
{
	M_UPDATE_WEAPON_wfWorking	= (1<<0),
	M_UPDATE_WEAPON_wfVisible	= (1<<1),

	M_UPDATE_WEAPON_FORCEDWORD	= u32(-1)
};

/*
M_SV_CONFIG_GAME
{
	game-specific
}

M_EVENT
{
	u32			dwTimestamp;
	u16			type;
	u16			destination;
}

M_SPAWN
{
	stringZ		Name_section;		// section in SYSTEM.LTX
	stringZ		Name_replace;		// Name of EDITOR's object, user can change this

	u8			gameid;
	u8			o_Point;			// [0..0xFC] = NumberOfRespawnPoint, or following...
	vec3		o_Position;			// ... 0xFF = AutoSelectAny, 0xFD = AutoSelectTeamed, 0xFE = UseSuppliedCoords
	vec3		o_Angle;
	u16			RespawnTime;		// time in sec, when respawn must be performed, 0=no respawn

	u16			server_id;			// 0xffff = Unknown/None/Invalid
	u16			server_id_parent;	// 0xffff = Unknown/None/Invalid
	u16			server_id_phantom;	// for respawn, for example :)
	u16			flags;

	u16			data_size;
	...

	event
	{
		cform
		{
			u8				count;

			element {
				u8			type;			// 0=Sphere, 1=Box

				sphere {
					Fsphere		def;
				}
				box {
					Fmatrix		def;
				}
			}
		}
		actions
		{
			u8				count;

			action {
				u8			type;			// 0=Enter, 1=Leave
				u16			count;			// number of times to play (0xffff(-1) - infinite play)
				u64			clsid;
				stringZ		event;
			}
			...
		}
	}
	object
	{
		u8			g_team;			// user defined
		u8			g_squad;		// user defined
		u8			g_group;		// user defined
	}
	weapon
	{
		u16			a_current;
		u16			a_elapsed;
	}
	dummy
	{
		u8				style;				// esAnimated=1<<0,	esModel=1<<1, esParticles=1<<2, esSound=1<<3, esRelativePosition=1<<4

		esAnimated
		{
			stringZ		file;
		}
		esModel
		{
			stringZ		file;
		}
		esParticles
		{
			stringZ		file;
		}
		esSound
		{
			stringZ		file;
		}
	}
	zone
	{
		cform {
			u8				count;

			element {
				u8			type;	// 0=Sphere, 1=Box

				sphere {
					Fsphere		def;
				}
				box {
					Fmatrix		def;
				}
			}
		}
	}
};

M_UPDATE
{
	u32	server_time;	// only for server2client update

	header
	{
		u16		id;
		u8		size
	}
	actor
	{
		u32		timestamp;
		u8			flags;
		vec3		pos;
		u8			mstate;		// checked and verified keyboard/motion state
		angle8		m_yaw;		// model's yaw
		angle8		t_yaw;		// torso's yaw
		angle8		t_pitch;	// torso's pitch
		direction	accel;
		float		accel_magnitude;
		f_q16[-1k,1k] health;
		f_q16[-1k,1k] armor;
	}

	enemy
	{
		u32			timestamp;
		u8			flags;
		vec3		pos;
		u8			state_data_size;

		?...?		state_data

	}

	weapon
	{
		u32			timestamp;
		u8			flags;				// wf_working, wf_visible=selected or independent,

		u16			a_current;
		u16			a_elapsed;

		vec3		pos;				// position
		angle8		aX;					// X: orientation
		angle8		aY;					// Y:
		angle8		aZ;					// Z:

		vec3		f_pos;
		direction	f_dir;
	}
}

GE_RESPAWN
{
}

GE_OWNERSHIP_TAKE
{
	u16			id_entity;
}

GE_OWNERSHIP_REJECT
{
	u16			id_entity;
}
GE_TRANSFER_AMMO
{
	u16			id_entity;				// �� ����
}
GE_HIT									// ��������� (�����)
{
	u16			id_entity;				// dest=��� ����������, entity=��� �����
	direction	dir;
	float		power;
	s16			element;
	vector3		position_in_bone_space;
}
GE_DIE
{
	u16			id_entity;				// dest=��� ����, entity=���� ������� ���� ��������
	u32			client;					// determined by server
}
GE_DESTROY
{
}
GE_WPN_STATE_CHANGE
{
	u8			state;
}
*/

#endif /*_INCDEF_XRMESSAGES_H_*/
