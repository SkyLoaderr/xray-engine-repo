#include "stdafx.h"
#include "xrServer.h"
#include "entity.h"

class xrSE_Actor : public xrServerEntity
{
public:
	u32						timestamp;
	u8						flags;
	Fvector					pos;
	u8						mstate;
	float					model_yaw;
	SRotation				torso;
	Fvector					accel;
	Fvector					velocity;
	Fvector					f_pos;
	Fvector					f_dir;
	u8						weapon;

	xrSE_Actor() {};

	virtual void			Read			(NET_Packet& P)
	{
		P.r_u32				(timestamp	);
		P.r_u8				(flags		);
		P.r_vec3			(pos		);
		P.r_u8				(mstate		);
		P.r_angle8			(model_yaw	);
		P.r_angle8			(torso.yaw	);
		P.r_angle8			(torso.pitch);
		P.r_sdir			(accel		);
		P.r_sdir			(velocity	);
		P.r_u8				(weapon		);
		if (flags&MF_FIREPARAMS)
		{
			P.r_vec3		(f_pos);
			P.r_dir			(f_dir);
		};
	};
	virtual void			Write			(NET_Packet& P)
	{
		P.w_u32				(timestamp);
		P.w_u8				(flags);
		P.w_vec3			(pos);
		P.w_u8				(mstate);
		P.w_angle8			(model_yaw	);
		P.w_angle8			(torso.yaw	);
		P.w_angle8			(torso.pitch);
		P.w_sdir			(accel);
		P.w_sdir			(velocity);
		P.w_u8				(weapon);
		if (flags&MF_FIREPARAMS)
		{
			P.w_vec3		(f_pos);
			P.w_dir			(f_dir);
		}
	};
	virtual BOOL			RelevantTo		(xrServerEntity* E)
	{
		return TRUE;
	};
	virtual void			GetPlacement	(Fvector4& dest)
	{
		dest.set			(pos.x,pos.y,pos.z,model_yaw);
	};
};

//---------------------------------------------------------------------------------------------
class xrSE_Enemy : public xrServerEntity
{
public:
	u32				dwTimeStamp;			// server(game) timestamp
	u8				flags;
	float			o_model;				// model yaw
	SRotation		o_torso;				// torso in world coords
	Fvector			p_pos;					// in world coords

	xrSE_Enemy()	{};

	virtual void			Read			(NET_Packet& P)
	{
		P.r_u32				(dwTimeStamp	);
		P.r_u8				(flags			);
		P.r_vec3			(p_pos			);
		P.r_angle8			(o_model		);
		P.r_angle8			(o_torso.yaw	);
		P.r_angle8			(o_torso.pitch	);
	};
	virtual void			Write			(NET_Packet& P)
	{
		P.w_u32				(dwTimeStamp);
		P.w_u8				(flags);
		P.w_vec3			(p_pos);
		P.w_angle8			(o_model);
		P.w_angle8			(o_torso.yaw);
		P.w_angle8			(o_torso.pitch);
	}
	virtual BOOL			RelevantTo		(xrServerEntity* E)
	{
		return TRUE;
	};
	virtual void			GetPlacement	(Fvector4& dest)
	{
		dest.set			(p_pos.x,p_pos.y,p_pos.z,o_model);
	};
};


//--------------------------------------------------------------------
xrServerEntity*	xrServer::entity_Create		(LPCSTR name)
{
	CLASS_ID cls = pSettings->ReadCLSID(name,"class");

	switch (cls)
	{
	case CLSID_OBJECT_ACTOR:	return new	xrSE_Actor;
	//case CLSID_AI_HUMAN:		return new	xrSE_Enemy;
	//case CLSID_AI_RAT:			return new	xrSE_Enemy;
	case CLSID_AI_HEN:			return new	xrSE_Enemy;
	}
	return 0;
}

void			xrServer::entity_Destroy	(xrServerEntity* P)
{
	R_ASSERT	(P);
	ids_used	[P->ID]	= false;
	_DELETE		(P);
}

