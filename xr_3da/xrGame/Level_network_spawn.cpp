#include "stdafx.h"

void CLevel::g_cl_Spawn		(LPCSTR name, int rp, int team, int squad, int group)
{
	Fvector		dummyPos,dummyAngle; dummyPos.set(0,0,0); dummyAngle.set(0,0,0);
	NET_Packet	P;
	P.w_begin	(M_SPAWN);
	P.w_string	(name);
	P.w_string	("");
	P.w_u8		((rp>=0)?u8(rp):u8(0xff));
	P.w_vec3	(dummyPos);
	P.w_vec3	(dummyAngle);
	P.w_u16		(0xffff);	// srv-id	| by server
	P.w_u16		(0xffff);
	P.w_u16		(M_SPAWN_OBJECT_ACTIVE  | M_SPAWN_OBJECT_LOCAL);
	P.w_u16		(3);		// data size
	P.w_u8		(u8(team));
	P.w_u8		(u8(squad));
	P.w_u8		(u8(group));
	Send		(P,net_flags(TRUE));
}

void CLevel::g_sv_Spawn		(NET_Packet* Packet)
{
	// Begin analysis
	NET_Packet&	P = *Packet;
	u16			type;
	P.r_begin	(type);
	R_ASSERT	(type==M_SPAWN);

	// Read definition
	char		s_name[128],s_replace[128];
	u8			s_rp;
	u16			s_server_id,s_server_parent_id,s_data_size,s_flags;
	Fvector		o_pos,o_angle;
	P.r_string	(s_name);
	P.r_string	(s_replace);
	P.r_u8		(s_rp);
	P.r_vec3	(o_pos);
	P.r_vec3	(o_angle);
	P.r_u16		(s_server_id);
	P.r_u16		(s_server_parent_id);
	P.r_u16		(s_flags);
	P.r_u16		(s_data_size);

	// Real spawn
	CObject*	O = Objects.LoadOne	(s_name);
	if (O)	
	{
		O->cName_set		(s_name);
		O->cNameSect_set	(s_name);
		if (s_replace[0])	O->cName_set		(s_replace);
	}
	if (0==O || (!O->net_Spawn(s_flags&M_SPAWN_OBJECT_LOCAL, s_server_id, o_pos, o_angle, P, s_flags))) 
	{
		Objects.DestroyObject(O);
		Msg("! Failed to spawn entity '%s'",s_name);
	} else {
		if ((s_flags&M_SPAWN_OBJECT_LOCAL) && (0==CurrentEntity()))	SetEntity		(O);
		if (s_flags&M_SPAWN_OBJECT_ACTIVE)							O->OnActivate	( );
		if (0xffff != s_server_parent_id)	
		{
			NET_Packet	GEN;
			GEN.w_begin	(M_EVENT);
			GEN.w_u32	(Level().timeServer());
			GEN.w_u16	(GE_OWNERSHIP_TAKE);
			GEN.w_u16	(s_server_parent_id);
			GEN.w_u16	(O->ID());
			Send		(GEN,net_flags(TRUE,TRUE));
		}
	}
}
