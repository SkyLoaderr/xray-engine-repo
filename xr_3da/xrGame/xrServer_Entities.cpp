#include "stdafx.h"
#include "xrServer.h"
#include "entity.h"

class xrSE_Actor : public xrServerEntity
{
public:
	DWORD					timestamp;
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
		timestamp			= P.r_u32		();
		flags				= P.r_u8		();
		pos					= P.r_vec3		();
		mstate				= P.r_u8		();
		model_yaw			= P.r_angle8	();
		torso.yaw			= P.r_angle8	();
		torso.pitch			= P.r_angle8	();
		accel				= P.r_sdir		();
		velocity			= P.r_sdir		();
		weapon				= P.r_u8		();
		if (flags&MF_FIREPARAMS)
		{
			f_pos			= P.r_vec3		();
			f_dir			= P.r_dir		();
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
	virtual BOOL			Spawn			(BYTE rp, xrS_entities& ent)
	{
		// We need to select respawn point
		// Get list of respawn points
		if (s_team>=int(Level().Teams.size()))	return FALSE;
		svector<Fvector4,maxRP>&	RP = Level().Teams[s_team].RespawnPoints;
		if (RP.empty())							return FALSE;

		DWORD	selected	= 0;
		if (rp==0xff) 
		{
			// Select respawn point
			float	best		= -1;
			for (DWORD id=0; id<RP.size(); id++)
			{
				Fvector4&	P = RP[id]; 
				Fvector		POS;
				POS.set		(P.x,P.y,P.z);
				float		cost=0;
				DWORD		count=0;
				
				for (DWORD o=0; o<ent.size(); o++) 
				{
					// Get entity & it's position
					xrServerEntity*	E	= ent[o];
					Fvector4	e_ori;	E->GetPlacement(e_ori);
					Fvector		e_pos;	e_pos.set(e_ori.x,e_ori.y,e_ori.z);
					
					float	dist		= POS.distance_to(e_pos);
					float	e_cost		= 0;
					
					if (s_team == E->s_team)	{
						// same teams, try to spawn near them, but not so near
						if (dist<5)		e_cost += 3*(5-dist);
					} else {
						// different teams, try to avoid them
						if (dist<30)	e_cost += 3*(30-dist);
					}
					
					cost	+= dist;
					count	+= 1;
				}
				
				if (0==count)	{ selected = id; break; }
				cost /= float(count);
				if (cost>best)	{ selected = id; best = cost; }
			}
		} else {
			if (rp>=RP.size())	Msg("! ERROR: Can't spawn entity at RespawnPoint #%d.",DWORD(rp));
			selected = DWORD(rp);
		}
		
		// Perform spawn
		Fvector4&			P = Level().Teams[s_team].RespawnPoints[selected];
		pos.set				(P.x,P.y,P.z);
		model_yaw			= P.w;
		flags				= 0;
		return TRUE;
	};
};

//---------------------------------------------------------------------------------------------
class xrSE_Enemy : public xrServerEntity
{
public:
	DWORD			dwTimeStamp;			// server(game) timestamp
	u8				flags;
	float			o_model;				// model yaw
	SRotation		o_torso;				// torso in world coords
	Fvector			p_pos;					// in world coords

	xrSE_Enemy()	{};

	virtual void			Read			(NET_Packet& P)
	{
		dwTimeStamp			= P.r_u32		();
		flags				= P.r_u8		();
		p_pos				= P.r_vec3		();
		o_model				= P.r_angle8	();
		o_torso.yaw			= P.r_angle8	();
		o_torso.pitch		= P.r_angle8	();
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
	virtual BOOL			Spawn			(BYTE rp, xrS_entities& ent)
	{
		// We need to select respawn point
		// Get list of respawn points
		if (s_team>=int(Level().Teams.size()))	return FALSE;
		svector<Fvector4,maxRP>&	RP = Level().Teams[s_team].RespawnPoints;
		if (RP.empty())							return FALSE;
		
		// Select respawn point
		DWORD	selected	= 0;
		if (rp==0xff) 
		{
			float	best		= -1;
			for (DWORD id=0; id<RP.size(); id++)
			{
				Fvector4&	P = RP[id]; 
				Fvector		POS;
				POS.set		(P.x,P.y,P.z);
				float		cost=0;
				DWORD		count=0;
				
				for (DWORD o=0; o<ent.size(); o++) 
				{
					// Get entity & it's position
					xrServerEntity*	E	= ent[o];
					Fvector4	e_ori;	E->GetPlacement(e_ori);
					Fvector		e_pos;	e_pos.set(e_ori.x,e_ori.y,e_ori.z);
					
					float	dist		= POS.distance_to(e_pos);
					float	e_cost		= 0;
					
					if (s_team == E->s_team)	{
						// same teams, try to spawn near them, but not so near
						if (dist<5)		e_cost += 3*(5-dist);
					} else {
						// different teams, try to avoid them
						if (dist<30)	e_cost += 3*(30-dist);
					}
					
					cost	+= dist;
					count	+= 1;
				}
				
				if (0==count)	{ selected = id; break; }
				cost /= float(count);
				if (cost>best)	{ selected = id; best = cost; }
			}
		} else {
			if (rp>=RP.size())	Msg("! ERROR: Can't spawn entity at RespawnPoint #%d.",DWORD(rp));
			selected = DWORD(rp);
		}
		
		// Perform spawn
		Fvector4&			P = Level().Teams[s_team].RespawnPoints[selected];
		p_pos.set			(P.x,P.y,P.z);
		o_model				= P.w;
		flags				= 0;
		return TRUE;
	};
};


//--------------------------------------------------------------------
xrServerEntity*	xrServer::entity_Create		(LPCSTR name)
{
	CLASS_ID cls = pSettings->ReadCLSID(name,"class");

	switch (cls)
	{
	case CLSID_OBJECT_ACTOR:	return new	xrSE_Actor;
	case CLSID_AI_HUMAN:		return new	xrSE_Enemy;
	}
	return 0;
}

void			xrServer::entity_Destroy	(xrServerEntity* P)
{
	_DELETE		(P);
}

