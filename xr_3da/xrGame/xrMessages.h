#pragma once

// CL	== client 2 server message
// SV	== server 2 client message

enum {
	M_UPDATE				=0,	// DUAL: Update state
	M_SPAWN,					// DUAL: Spawning, full state
	M_DESTROY,					// TO:   Destroying
	M_OWNERSHIP_TAKE,			// DUAL: Client request for ownership of an item
	M_OWNERSHIP_REJECT,			// DUAL: Client request ownership rejection
	M_MIGRATE_DEACTIVATE,		// TO:   Changing server, just deactivate
	M_MIGRATE_ACTIVATE,			// TO:   Changing server, full state
	M_HIT,						// 
	M_DIE,						// 

	M_CHAT,						// DUAL:

	M_SV_CONFIG,
	M_SV_CONFIG_FINISHED,

	MSG_FORCEDWORD				= DWORD(-1)
};

enum
{
	M_SPAWN_OBJECT_LOCAL		= (1<<0),
	M_SPAWN_OBJECT_ACTIVE		= (1<<1),

	M_SPAWN_OBJECT_FORCEDWORD	= DWORD(-1)
};
enum
{
	M_UPDATE_WEAPON_wfWorking	= (1<<0),
	M_UPDATE_WEAPON_wfVisible	= (1<<1),

	M_UPDATE_WEAPON_FORCEDWORD	= DWORD(-1)
};

/*
M_REQUEST_OWNERSHIP
{
	u16			id_parent;
	u16			id_entity;
}
M_REQUEST_REJECTION
{
	u16			id_parent;
	u16			id_entity;
}
M_SPAWN
{	
	stringZ		Name_section;		// section in SYSTEM.LTX
	stringZ		Name_replace;		// Name of EDITOR's object, user can change this

	u8			o_Point;			// [0..0xFD] = NumberOfRespawnPoint 0xFF = AutoSelect, 0xFE = UseSupplied
	vec3		o_Position;
	vec3		o_Angle;

	u16			server_id;			// 0xffff = Unknown/None/Invalid
	u16			server_id_parent;	// 0xffff = Unknown/None/Invalid
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
	DWORD	server_time;	// only for server2client update
	actor 
	{
		DWORD		timestamp;
		u8			flags;
		vec3		pos;
		u8			mstate;		// checked and verified keyboard/motion state
		angle8		m_yaw;		// model's yaw
		angle8		t_yaw;		// torso's yaw
		angle8		t_pitch;	// torso's pitch
		direction	accel;
		float		accel_magnitude;
	}

	enemy
	{
		DWORD		timestamp;
		u8			flags;
		vec3		pos;
		u8			state_data_size;
		
		?...?		state_data

	}

	weapon
	{
		DWORD		timestamp;
		u8			flags;				// wf_working, wf_visible=selected or independent,  

		u16			a_current;
		u16			a_elapsed;

		if (0xffff==parent && wf_visible) 
		{
			vec3		pos;			// position
			angle8		aX;				// X: orientation
			angle8		aY;				// Y: 
			angle8		aZ;				// Z: 
		}

		if (flags&wf_working || wf_visible) 
		{
			vec3		f_pos;
			direction	f_dir;
		}
	}
}

M_HIT
{  
		P.w_begin	(M_HIT		);
		P.w_u16		(net_ID		);
		P.w_u16		(who->net_ID);
		P.w_u8		(perc		);
		P.w_dir		(dir		);
}

M_DIE
{
}
*/
