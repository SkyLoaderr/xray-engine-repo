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
	char	Name[];
	BYTE	team;
	BYTE	squad;
	BYTE	group;
	BYTE	respawn_point; // 0xff = AutoSelect
};

SV_SPAWN
{
	char	Name[];
	char	NameReplace[];
	BYTE	team;
	BYTE	squad;
	BYTE	group;
	
	BYTE	server_id;

	BYTE	local;

	// position & orientation
	Fvector	o_Position;
	u8		o_angle;
}

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
