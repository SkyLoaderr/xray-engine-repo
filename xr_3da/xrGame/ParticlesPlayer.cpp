///////////////////////////////////////////////////////////////
// ParticlesPlayer.cpp
// интерфейс дл€ проигрывани€ партиклов на объекте
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ParticlesPlayer.h"
//-------------------------------------------------------------------------------------

CParticlesPlayer::SParticlesInfo* CParticlesPlayer::SBoneInfo::FindParticles(ref_str ps_name)
{
	for (ParticlesInfoListIt it=particles.begin(); it!=particles.end(); it++)
		if (it->ps->Name()==ps_name) return &(*it);
	return 0;
}
CParticlesPlayer::SParticlesInfo*	CParticlesPlayer::SBoneInfo::AppendParticles(CObject* object, ref_str ps_name)
{
	SParticlesInfo* pi	= FindParticles(ps_name);
	if (pi)				return pi;
	particles.push_back	(SParticlesInfo());
	pi					= &particles.back();
	pi->ps				= xr_new<CParticlesObject>(*ps_name, object->Sector(), false);
	return pi;
}
void CParticlesPlayer::SBoneInfo::StopParticles(ref_str ps_name)
{
	SParticlesInfo* pi	= FindParticles(ps_name);
	if (pi)				pi->ps->Stop();
}
void CParticlesPlayer::SBoneInfo::StopParticles(u16 sender_id)
{
	for (ParticlesInfoListIt it=particles.begin(); it!=particles.end(); it++)
		if (it->sender_id==sender_id) it->ps->Stop();
}
//-------------------------------------------------------------------------------------

CParticlesPlayer::CParticlesPlayer ()
{
	m_Bones.push_back	(SBoneInfo(0,Fvector().set(0,0,0)));
	bone_mask			= u64(1)<<u64(0);
}

CParticlesPlayer::~CParticlesPlayer ()
{
}

void CParticlesPlayer::Load(CKinematics* K)
{
	VERIFY				(K);
	//считать список косточек и соответствующих
	//офсетов  куда можно вешать партиклы
	CInifile* ini		= K->LL_UserData();
	if(ini&&ini->section_exist("particle_bones")){
		bone_mask		= 0;
		m_Bones.clear	();
		CInifile::Sect& data		= ini->r_section("particle_bones");
		for (CInifile::SectIt I=data.begin(); I!=data.end(); I++){
			CInifile::Item& item	= *I;
			u16 index				= K->LL_BoneID(*item.first); 
			R_ASSERT3(index != BI_NONE, "Particles bone not found", *item.first);
			Fvector					offs;
			sscanf					(*item.second,"%f,%f,%f",&offs.x,&offs.y,&offs.z);
			m_Bones.push_back		(SBoneInfo(index,offs));
			bone_mask				|= u64(1)<<u64(index);
		}
	}
}


CParticlesPlayer::SBoneInfo* CParticlesPlayer::get_nearest_bone_info(CKinematics* K, u16 bone_index)
{
	u16 play_bone	= bone_index;
	while((BI_NONE!=play_bone)&&!(bone_mask&(u64(1)<<u64(play_bone))))
		play_bone	= K->LL_GetData(play_bone).ParentID;
	return get_bone_info(play_bone);
}



void CParticlesPlayer::StartParticles(ref_str particles_name, u16 bone_num, const Fvector& dir, u16 sender_id)
{
	R_ASSERT(*particles_name);
	
	CObject* object					= dynamic_cast<CObject*>(this);
	VERIFY(object);

	//найти ближайшую допустимую косточку, чтобы повесить партиклы
	SBoneInfo* pBoneInfo			=  get_nearest_bone_info(PKinematics(object->Visual()),bone_num);
	if(!pBoneInfo) return;

	SParticlesInfo* particles_info	= pBoneInfo->AppendParticles(object,particles_name);
	particles_info->dir				= dir;
	particles_info->sender_id		= sender_id;

	//начать играть партиклы
	Fmatrix xform;
	MakeXFORM						(object,pBoneInfo->index,particles_info->dir,pBoneInfo->offset,xform);
	particles_info->ps->UpdateParent(xform,zero_vel);
	particles_info->ps->Play		();
}

void CParticlesPlayer::StartParticles(ref_str ps_name, const Fvector& dir, u16 sender_id)
{
	CObject* object					= dynamic_cast<CObject*>(this);
	VERIFY(object);
	for(BoneInfoVecIt it = m_Bones.begin(); it!=m_Bones.end(); it++){
		SParticlesInfo* particles_info	= it->AppendParticles(object,ps_name);
		particles_info->dir			= dir;
		particles_info->sender_id	= sender_id;

		//начать играть партиклы
		Fmatrix xform;
		MakeXFORM					(object,it->index,particles_info->dir,it->offset,xform);
		particles_info->ps->UpdateParent(xform,zero_vel);
		particles_info->ps->Play	();
	}
}

void CParticlesPlayer::StopParticles(u16 sender_id, u16 bone_id)
{
	if (BI_NONE==bone_id){
		for(BoneInfoVecIt it=m_Bones.begin(); it!=m_Bones.end(); it++)
			it->StopParticles	(sender_id);
	}else{
		SBoneInfo* bi			= get_bone_info(bone_id); VERIFY(bi);
		bi->StopParticles		(sender_id);
	}
}

void CParticlesPlayer::StopParticles(ref_str ps_name, u16 bone_id)
{
	if (BI_NONE==bone_id){
		for(BoneInfoVecIt it=m_Bones.begin(); it!=m_Bones.end(); it++)
			it->StopParticles	(ps_name);
	}else{
		SBoneInfo* bi			= get_bone_info(bone_id); VERIFY(bi);
		bi->StopParticles		(ps_name);
	}
}

void CParticlesPlayer::UpdateParticles()
{
	CObject* object			= dynamic_cast<CObject*>(this);
	VERIFY(object);

	for(BoneInfoVecIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
		SBoneInfo& b_info	= *b_it;

		for (ParticlesInfoListIt p_it=b_info.particles.begin(); p_it!=b_info.particles.end(); p_it++){
			SParticlesInfo& p_info	= *p_it;
			//обновить позицию партиклов
			Fmatrix xform;
			MakeXFORM				(object,b_info.index,p_info.dir,b_info.offset,xform);
			p_info.ps->UpdateParent(xform,zero_vel);

			if(!p_info.ps->IsPlaying()){
				p_info.ps->PSI_destroy		();
				ParticlesInfoListIt cur_it	= p_it++;
				b_info.particles.erase		(cur_it);
			}
		}
	}
}

void CParticlesPlayer::MakeXFORM	(CObject* pObject, u16 bone_id, const Fvector& dir, const Fvector& offset, Fmatrix& result)
{
	CBoneInstance&		l_tBoneInstance = PKinematics(pObject->Visual())->LL_GetBoneInstance((u16)bone_id);

	result.identity		();
	result.k.normalize	(dir);
	Fvector::generate_orthonormal_basis(result.k, result.i, result.j);
	result.translate_over(offset);
	result.mulA			(l_tBoneInstance.mTransform);
	result.mulA			(pObject->XFORM());
}

u16 CParticlesPlayer::GetNearestBone	(CKinematics* K, u16 bone_id)
{
	u16 play_bone	= bone_id;
	while((BI_NONE!=play_bone)||!(bone_mask&(u64(1)<<u64(play_bone))))
		play_bone	= K->LL_GetData(play_bone).ParentID;
	return play_bone;
}
