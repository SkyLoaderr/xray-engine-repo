#pragma once

// CL	== client 2 server message
// SV	== server 2 client message

enum {
	M_UPDATE			=0,
	M_FIRE_BEGIN,
	M_FIRE_END,
	M_FIRE_HIT,
	
	M_DIE,
	M_CHAT,

	M_SV_CONFIG,
	M_CL_SPAWN,
	M_SV_SPAWN,
	M_SV_CONFIG_FINISHED,

	MSG_FORCEDWORD		= DWORD(-1)
};

enum 
{
	MF_FIREPARAMS		= (1<<0),

	MF_FORCEDWORD		= DWORD(-1)
};

/*
CL_SPAWN
{
	char		Name	[];
	BYTE		g_team;
	BYTE		g_squad;
	BYTE		g_group;
	BYTE		respawn_point; // 0xFF = AutoSelect, 0xFE = UsePosition
	Fvector		o_Pos;
	Fvector		o_Angle;

	WORD		data_size;
	...
};

SV_SPAWN
{
	char		Name		[];
	char		NameReplace	[];
	BYTE		team;
	BYTE		squad;
	BYTE		group;
	

	BYTE		local;

	// position & orientation
	Fvector		o_Position;
	u8			o_Angle;
};

M_SPAWN
{	
	stringZ		Name_section;	// section in SYSTEM.LTX
	stringZ		Name_replace;	// Name of EDITOR's object, user can change this

	u8			g_team;			// user defined
	u8			g_squad;		// user defined
	u8			g_group;		// user defined

	u8			o_Point;		// [0..0xFD] = NumberOfRespawnPoint 0xFF = AutoSelect, 0xFE = UseSupplied
	vec3		o_Position;
	vec3		o_Angle;

	u16			server_id;
	u8			local;

	u16			data_size;
	...

	event
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
		actions {
			u8				count;

			action {
				u8			bOnce;
				u64			clsid_target;
				stringZ		OnEnter;
				stringZ		OnLeave;
			}
			...
		}
	}
	zone
	{
		cform {
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
		u8			mstate;	// checked and verified keyboard/motion state
		angle8		m_yaw;	// model's yaw
		angle8		t_yaw;	// torso's yaw
		angle8		t_pitch;// torso's pitch
		direction	accel;
		float		accel_magnitude;

		u8			weapon;

		if (flags&MF_FIREPARAMS) {
			vec3		f_pos;
			direction	f_dir;
		}
	}
	enemy
	{
		DWORD		timestamp;
		u8			flags;
		vec3		pos;
		u8			state_data_size;
		
		?...?		state_data

		if (flags&MF_FIREPARAMS) {
			vec3		f_pos;
			direction	f_dir;
		}
	}
}

M_FIRE_HIT
{  
		P.w_begin	(M_FIRE_HIT);
		P.w_u8		(u8(net_ID));
		P.w_u8		(u8(who->net_ID));
		P.w_u8		(perc	);
		P.w_dir		(dir	);
}

M_FIRE_BEGIN
{
		P.w_u8		(u8(net_ID));
		pos
		direction
}

M_DIE
{
}

*/
