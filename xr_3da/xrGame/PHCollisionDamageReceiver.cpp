#include "stdafx.h"
#include "phcollisiondamagereceiver.h"
#include "PhysicsShellHolder.h"
#include "xr_ini.h"
#include "../skeletoncustom.h"
#include "geometry.h"
#include "PhysicsShell.h"
#include "GameMtlLib.h"
#include "Physics.h"
#include "net_utils.h"
#include "xrMessages.h"
void CPHCollisionDamageReceiver::Init()
{
	CPhysicsShellHolder *sh	=PPhysicsShellHolder	();
	CKinematics			*K	=smart_cast<CKinematics*>(sh->Visual());
	CInifile			*ini=K->LL_UserData();
	if(ini->section_exist("collision_damage"))
	{
		
		CInifile::Sect& data		= ini->r_section("collision_damage");
		for (CInifile::SectIt I=data.begin(); I!=data.end(); I++){
			CInifile::Item& item	= *I;
			u16 index				= K->LL_BoneID(*item.first); 
			R_ASSERT3(index != BI_NONE, "Wrong bone name", *item.first);
			m_controled_bones.insert(std::pair<u16,float>(index,float(atof(*item.second))));
			CODEGeom* og= sh->PPhysicsShell()->get_GeomByID(index);
			//R_ASSERT3(og, "collision damage bone has no physics collision", *item.first);
			if(og)og->set_obj_contact_cb(CollisionCallback);
		}
		
	}
}



void _stdcall CPHCollisionDamageReceiver::CollisionCallback(bool& do_colide,dContact& c,SGameMtl* material_1,SGameMtl* material_2)
{
	if(material_1->Flags.test(SGameMtl::flPassable)||material_2->Flags.test(SGameMtl::flPassable))return;
	dBodyID						b1		=	dGeomGetBody(c.geom.g1);
	dBodyID						b2		=	dGeomGetBody(c.geom.g2);
	dxGeomUserData				*ud1	=	retrieveGeomUserData(c.geom.g1);
	dxGeomUserData				*ud2	=	retrieveGeomUserData(c.geom.g2);
	CPhysicsShellHolder			*o1		=	NULL;if(ud1)o1=ud1->ph_ref_object;
	CPhysicsShellHolder			*o2		=	NULL;if(ud2)o2=ud2->ph_ref_object;
	u16							id1		=	u16(-1);
	u16							id2		=	u16(-1);
	CPHCollisionDamageReceiver	*dr1	=	NULL;
	CPHCollisionDamageReceiver	*dr2	=	NULL;
	if(o1){dr1=o1->PHCollisionDamageReceiver();id1=o1->ID();}
	if(o2){dr2=o2->PHCollisionDamageReceiver();id2=o2->ID();}
	
	VERIFY2(dr1||dr2,"wrong callback");
	float dfs=(material_1->fBounceDamageFactor+material_2->fBounceDamageFactor);
	if(fis_zero(dfs)) return;
	if(b1)
	{
		if(b2)
		{
			float E=E_NLD(b1,b2,c.geom.normal);
			Fvector dir;dir.set(*(Fvector*)c.geom.normal);
			Fvector pos;
			pos.sub(*(Fvector*)c.geom.pos,*(Fvector*)dBodyGetPosition(b1));//it is not true pos in bone space
			if(dr1) dr1->Hit(id2,ud1->bone_id,E*material_2->fBounceDamageFactor/dfs,dir,pos);
			pos.sub(*(Fvector*)c.geom.pos,*(Fvector*)dBodyGetPosition(b2));
			dir.invert();
			if(dr2) dr2->Hit(id1,ud2->bone_id,E*material_1->fBounceDamageFactor/dfs,dir,pos);
		}
		else
		{
			Fvector pos;
			pos.sub(*(Fvector*)c.geom.pos,*(Fvector*)dBodyGetPosition(b1));//it is not true pos in bone space
			Fvector dir;dir.set(*(Fvector*)c.geom.normal);
			if(dr1) dr1->Hit(id2,ud1->bone_id,E_NlS(b1,c.geom.normal,1.f)*material_2->fBounceDamageFactor/dfs,dir,pos);
		}
	}
	else
	{
		Fvector pos;
		pos.sub(*(Fvector*)c.geom.pos,*(Fvector*)dBodyGetPosition(b2));//it is not true pos in bone space
		Fvector dir;dir.set(*(Fvector*)c.geom.normal);
		dir.invert();
		if(dr2)dr2->Hit(id1,ud2->bone_id,E_NlS(b2,c.geom.normal,-1.f)*material_1->fBounceDamageFactor/dfs,dir,pos);
	}
	
}

const static float hit_threthhold=5.f;
void CPHCollisionDamageReceiver::Hit(u16 source_id,u16 bone_id,float power,const Fvector& dir,Fvector &pos )
{

	xr_map<u16,float>::iterator i=m_controled_bones.find(bone_id);
	if(i==m_controled_bones.end())return;
	power*=i->second;
	if(power<hit_threthhold)return;
	
	NET_Packet		P;
	CPhysicsShellHolder *ph=PPhysicsShellHolder();
	ph->u_EventGen(P,GE_HIT,ph->ID());
	P.w_u16		(ph->ID());
	P.w_u16		(source_id);
	P.w_dir		(dir);
	P.w_float	(power);
	P.w_s16		(s16(bone_id));
	P.w_vec3	(pos);
	P.w_float	(0.f);
	P.w_u16		(ALife::eHitTypeStrike);
	ph->u_EventSend(P);
}

void CPHCollisionDamageReceiver::Clear()
{
	//CPhysicsShellHolder *sh	=PPhysicsShellHolder	();
	//xr_map<u16,float>::iterator i=m_controled_bones.begin(),e=m_controled_bones.end();
	//for(;e!=i;++i)
	//{
	//	CODEGeom* og= sh->PPhysicsShell()->get_GeomByID(i->first);
	//	if(og)og->set_obj_contact_cb(NULL);
	//}
		m_controled_bones.clear();
}