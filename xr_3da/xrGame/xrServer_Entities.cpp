#include "stdafx.h"
#include "xrServer.h"
//#include "entity.h"

// EDITOR, NETWORK, SAVE, LOAD, DEMO

void	xrServerEntity::Spawn_Write		(NET_Packet& P, BOOL bLocal)
{
	// generic
	P.w_begin			(M_SPAWN		);
	P.w_string			(s_name			);
	P.w_string			(s_name_replace	);
	P.w_u8				(0xFE			);	// No need for RP, use supplied (POS,ANGLEs)
	P.w_vec3			(o_Position		);
	P.w_vec3			(o_Angle		);
	P.w_u16				(ID				);
	P.w_u16				(ID_Parent		);
	if (bLocal)			P.w_u16(s_flags	| M_SPAWN_OBJECT_LOCAL );
	else				P.w_u16(s_flags );

	// write specific data
	u32	position		= P.w_tell		();
	P.w_u16				(0				);
	STATE_Write			(P				);
	u16 size			= u16			(P.w_tell()-position);
	P.w_seek			(position,&size,sizeof(u16));
}
void	xrServerEntity::Spawn_Read		(NET_Packet& P)
{
	u16					dummy16;
	// generic
	P.r_begin			(dummy16		);
	P.r_string			(s_name			);
	P.r_string			(s_name_replace	);
	P.r_u8				(s_RP			);
	P.r_vec3			(o_Position		);
	P.r_vec3			(o_Angle		);
	P.r_u16				(ID				);
	P.r_u16				(ID_Parent		);
	P.r_u16				(s_flags		); s_flags&=~M_SPAWN_OBJECT_LOCAL;

	// read specific data
	u16					size;
	P.r_u16				(size			);	// size
	STATE_Read			(P,size			);
}
void	xrServerEntity::P_Read			(CStream& FS)
{
	FS.Read				(&desc,sizeof(desc));
}
void	xrServerEntity::P_Write			(CFS_Base& FS)
{
	FS.write			(&desc,sizeof(desc));
}

//
class xrSE_Weapon : public xrServerEntity
{
	typedef	xrServerEntity	inherited;
public:
	u32						timestamp;
	u8						flags;

	u16						a_current;
	u16						a_elapsed;

	Fvector					f_pos;
	Fvector					f_dir;
public:

	virtual void			UPDATE_Read		(NET_Packet& P)
	{
		P.r_u32				(timestamp);
		P.r_u8				(flags);

		P.r_u16				(a_current);
		P.r_u16				(a_elapsed);

		if ((0xffff==ID_Parent) && (flags&M_UPDATE_WEAPON_wfVisible))
		{
			P.r_vec3		(o_Position	);
			P.r_angle8		(o_Angle.x	);
			P.r_angle8		(o_Angle.y	);
			P.r_angle8		(o_Angle.z	);
		}
		if (flags & (M_UPDATE_WEAPON_wfVisible + M_UPDATE_WEAPON_wfWorking))
		{
			P.r_vec3		(f_pos);
			P.r_dir			(f_dir);
		}
	}

	virtual void			UPDATE_Write	(NET_Packet& P)
	{
		P.w_u32				(timestamp);
		P.w_u8				(flags);

		P.w_u16				(a_current);
		P.w_u16				(a_elapsed);

		if ((0xffff==ID_Parent) && (flags&M_UPDATE_WEAPON_wfVisible))
		{
			P.w_vec3		(o_Position	);
			P.w_angle8		(o_Angle.x	);
			P.w_angle8		(o_Angle.y	);
			P.w_angle8		(o_Angle.z	);
		}
		if (flags & (M_UPDATE_WEAPON_wfVisible + M_UPDATE_WEAPON_wfWorking))
		{
			P.w_vec3		(f_pos);
			P.w_dir			(f_dir);
		}
	}

	virtual void			STATE_Read		(NET_Packet& P, u16 size)
	{
		P.r_u16				(a_current);
		P.r_u16				(a_elapsed);
	}

	virtual void			STATE_Write		(NET_Packet& P)
	{
		P.w_u16				(a_current);
		P.w_u16				(a_elapsed);
	}

	virtual void			P_Write				(CFS_Base& FS)
	{
		inherited::P_Write	(FS);

		xrP_Integer			dI;
		dI.min				= 0;
		
		dI.max = 1000;		dI.value=a_current;	xrPWRITE_PROP(FS,"Ammo: total",			xrPID_INTEGER,dI);
		dI.max = 30;		dI.value=a_elapsed;	xrPWRITE_PROP(FS,"Ammo: in magazine",	xrPID_INTEGER,dI);
	}
	virtual void			P_Read				(CStream& FS)
	{
		inherited::P_Read	(FS);

		xrP_Integer			dI;
		xrPREAD_PROP		(FS,xrPID_INTEGER,dI);	a_current	=	u8(dI.value);
		xrPREAD_PROP		(FS,xrPID_INTEGER,dI);  a_elapsed	=	u8(dI.value);
	}
};

//
class xrSE_Teamed : public xrServerEntity
{
	typedef	xrServerEntity	inherited;
public:
	u8						s_team;
	u8						s_squad;
	u8						s_group;
public:
	xrSE_Teamed()
	{
		s_team = s_squad = s_group = 0;
	}

	virtual u8				g_team()			{ return s_team;	}
	virtual u8				g_squad()			{ return s_squad;	}
	virtual u8				g_group()			{ return s_group;	}
	
	virtual void			STATE_Read			(NET_Packet& P, u16 size)
	{
		P.r_u8				(s_team	);
		P.r_u8				(s_squad);
		P.r_u8				(s_group);
	}
	virtual void			STATE_Write			(NET_Packet& P)
	{
		P.w_u8				(s_team	);
		P.w_u8				(s_squad);
		P.w_u8				(s_group);
	}
	virtual void			P_Write				(CFS_Base& FS)
	{
		inherited::P_Write	(FS);

		xrP_Integer			dI;
		dI.min				= 0;
		dI.max				= 64;

		dI.value=s_team;	xrPWRITE_PROP		(FS,"Team",	xrPID_INTEGER,dI);
		dI.value=s_squad;	xrPWRITE_PROP		(FS,"Squad",xrPID_INTEGER,dI);
		dI.value=s_group;	xrPWRITE_PROP		(FS,"Group",xrPID_INTEGER,dI);
	}
	virtual void			P_Read				(CStream& FS)
	{
		inherited::P_Read	(FS);

		xrP_Integer			dI;

		xrPREAD_PROP		(FS,xrPID_INTEGER,dI);	s_team	=	u8(dI.value);
		xrPREAD_PROP		(FS,xrPID_INTEGER,dI);  s_squad	=	u8(dI.value);
		xrPREAD_PROP		(FS,xrPID_INTEGER,dI);  s_group	=	u8(dI.value);
	}
};

//
class xrSE_Dummy : public xrServerEntity
{
	typedef	xrServerEntity	inherited;
protected:
	enum SStyle{
		esAnimated			=1<<0,	
		esModel				=1<<1, 
		esParticles			=1<<2, 
		esSound				=1<<3,
		esRelativePosition	=1<<4
	};
protected:
	u8						s_style;
	char*					s_Animation;
	char*					s_Model;
	char*					s_Particles;
	char*					s_Sound;
public:
	~xrSE_Dummy()
	{
		_FREE				(s_Animation	);
		_FREE				(s_Model		);
		_FREE				(s_Particles	);
		_FREE				(s_Sound		);
	}

	virtual void			STATE_Read			(NET_Packet& P, u16 size)
	{
		P.r_u8				(s_style);

		if (s_style&esAnimated)		{
			// Load animator
			string256				fn;
			P.r_string				(fn);
			s_Animation				= xr_strdup(fn);
		}
		if (s_style&esModel)			{
			// Load model
			string256				fn;
			P.r_string				(fn);
			s_Model					= xr_strdup(fn);
		}
		if (s_style&esParticles)		{
			// Load model
			string256				fn;
			P.r_string				(fn);
			s_Particles				= xr_strdup(fn);
		}
		if (s_style&esSound)			{
			// Load model
			string256				fn;
			P.r_string				(fn);
			s_Sound					= xr_strdup(fn);
		}
	}
	virtual void			STATE_Write			(NET_Packet& P)
	{
		P.w_u8				(s_style		);
		if (s_style&esAnimated)		P.w_string			(s_Animation	);
		if (s_style&esModel)		P.w_string			(s_Model		);
		if (s_style&esParticles)	P.w_string			(s_Particles	);
		if (s_style&esSound)		P.w_string			(s_Sound		);
	}
	virtual void			UPDATE_Read			(NET_Packet& P)	{};
	virtual void			UPDATE_Write		(NET_Packet& P)	{};
};

//
class xrSE_Car : public xrSE_Teamed
{
	typedef	xrSE_Teamed		inherited;
public:
	virtual void			UPDATE_Read			(NET_Packet& P)	{};
	virtual void			UPDATE_Write		(NET_Packet& P)	{};
};

//
class xrSE_Crow : public xrSE_Teamed
{
	typedef	xrSE_Teamed		inherited;
public:
	virtual void			UPDATE_Read			(NET_Packet& P)	{};
	virtual void			UPDATE_Write		(NET_Packet& P)	{};
};

//
class xrSE_Actor : public xrSE_Teamed
{
	typedef	xrSE_Teamed		inherited;
public:	
	u32						timestamp;
	u8						flags;
	u8						mstate;
	float					model_yaw;
	SRotation				torso;
	Fvector					accel;
	Fvector					velocity;

	xrSE_Actor() {};

	virtual void			UPDATE_Read			(NET_Packet& P)
	{
		P.r_u32				(timestamp	);
		P.r_u8				(flags		);
		P.r_vec3			(o_Position	);
		P.r_u8				(mstate		);
		P.r_angle8			(model_yaw	);
		P.r_angle8			(torso.yaw	);
		P.r_angle8			(torso.pitch);
		P.r_sdir			(accel		);
		P.r_sdir			(velocity	);
	};
	virtual void			UPDATE_Write		(NET_Packet& P)
	{
		P.w_u32				(timestamp	);
		P.w_u8				(flags		);
		P.w_vec3			(o_Position	);
		P.w_u8				(mstate		);
		P.w_angle8			(model_yaw	);
		P.w_angle8			(torso.yaw	);
		P.w_angle8			(torso.pitch);
		P.w_sdir			(accel		);
		P.w_sdir			(velocity	);
	};
};

//---------------------------------------------------------------------------------------------
class xrSE_Enemy : public xrSE_Teamed
{
	typedef	xrSE_Teamed		inherited;
public:
	u32						dwTimeStamp;			// server(game) timestamp
	u8						flags;
	float					o_model;				// model yaw
	SRotation				o_torso;				// torso in world coords
	
	xrSE_Enemy()			{};
	
	virtual void			UPDATE_Read			(NET_Packet& P)
	{
		P.r_u32				(dwTimeStamp	);
		P.r_u8				(flags			);
		P.r_vec3			(o_Position		);
		P.r_angle8			(o_model		);
		P.r_angle8			(o_torso.yaw	);
		P.r_angle8			(o_torso.pitch	);
	};
	virtual void			UPDATE_Write		(NET_Packet& P)
	{
		P.w_u32				(dwTimeStamp	);
		P.w_u8				(flags			);
		P.w_vec3			(o_Position		);
		P.w_angle8			(o_model		);
		P.w_angle8			(o_torso.yaw	);
		P.w_angle8			(o_torso.pitch	);
	}
};
//---------------------------------------------------------------------------------------------
class xrSE_CFormed
{
public:
	union shape_data
	{
		Fsphere		sphere;
		Fmatrix		box;
	};
	struct shape_def
	{
		u8			type;
		shape_data	data;
	};
	vector<shape_def>	shapes;
public:
	void					cform_read			(NET_Packet& P)
	{
		shapes.clear();
		u8	count;	P.r_u8(count);
		while (count) {
			shape_def	S;
			P.r_u8	(S.type);
			switch	(S.type)
			{
			case 0:	P.r			(&S.data.sphere,sizeof(S.data.sphere));	break;
			case 1:	P.r_matrix	(S.data.box);							break;
			}
			shapes.push_back	(S);
			count--;
		}
	}
	void					cform_write			(NET_Packet& P)
	{
		P.w_u8	(u8(shapes.size()));
		for (u32 i=0; i<shapes.size(); i++)
		{
			shape_def&	S	= shapes[i];
			P.w_u8	(S.type);
			switch (S.type)
			{
			case 0:	P.w			(&S.data.sphere,sizeof(S.data.sphere));	break;
			case 1:	P.w_matrix	(S.data.box);							break;
			}
		}
	}
};

class xrSE_Event : public xrSE_CFormed, public xrServerEntity
{
	typedef	xrServerEntity		inherited;
public:	// actions
	struct tAction
	{
		u8		type;
		u16		count;
		u64		cls;
		LPSTR	event;
	};
	vector<tAction>			Actions;

	void					Actions_clear()
	{
		for (u32 a=0; a<Actions.size(); a++)
			_FREE(Actions[a].event);
		Actions.clear	();
	}
public:	
	virtual void			UPDATE_Read			(NET_Packet& P)
	{
	}
	virtual void			UPDATE_Write		(NET_Packet& P)
	{
	}
	virtual void			STATE_Read			(NET_Packet& P, u16 size)
	{
		// CForm
		cform_read			(P);

		// Actions
		Actions_clear		();
		u8 count;	P.r_u8	(count);
		while (count)	{
			tAction		A;
			string512	str;
			P.r_u8		(A.type);
			P.r_u16		(A.count);
			P.r_u64		(A.cls);
			P.r_string	(str);
			A.event		= xr_strdup(str);
			Actions.push_back(A);
			count--;
		}
	}
	virtual void			STATE_Write			(NET_Packet& P)
	{
		// CForm
		cform_write			(P);

		// Actions
		P.w_u8				(u8(Actions.size()));
		for (u32 i=0; i<Actions.size(); i++)
		{
			tAction&	A = Actions[i];
			P.w_u8		(A.type	);
			P.w_u16		(A.count);
			P.w_u64		(A.cls	);
			P.w_string	(A.event);
		}
	}
};

//--------------------------------------------------------------------
xrServerEntity*	xrServer::entity_Create		(LPCSTR name)
{
	CLASS_ID cls = pSettings->ReadCLSID(name,"class");

	switch (cls)
	{
	case CLSID_OBJECT_ACTOR:	return new	xrSE_Actor;
	case CLSID_OBJECT_DUMMY:	return new  xrSE_Dummy;
	case CLSID_AI_HEN:			return new	xrSE_Enemy;
	case CLSID_AI_RAT:			return new	xrSE_Enemy;
	case CLSID_AI_SOLDIER:		return new	xrSE_Enemy;
	case CLSID_AI_ZOMBIE:		return new	xrSE_Enemy;
	case CLSID_AI_CROW:			return new	xrSE_Crow;
	case CLSID_EVENT:			return new  xrSE_Event;
	case CLSID_CAR_NIVA:		return new  xrSE_Car;
	}
	return 0;
}

void			xrServer::entity_Destroy	(xrServerEntity* P)
{
	R_ASSERT	(P);
	ids_used	[P->ID]	= false;
	_DELETE		(P);
}

