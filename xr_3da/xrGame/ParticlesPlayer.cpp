///////////////////////////////////////////////////////////////
// ParticlesPlayer.cpp
// ��������� ��� ������������ ��������� �� �������
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ParticlesPlayer.h"
#include "../xr_object.h"
#include "../skeletoncustom.h"
//-------------------------------------------------------------------------------------
static void generate_orthonormal_basis(const Fvector& dir,Fmatrix &result)
{
	result.identity		();
	result.k.normalize	(dir);
	Fvector::generate_orthonormal_basis(result.k, result.j, result.i);
}
CParticlesPlayer::SParticlesInfo* CParticlesPlayer::SBoneInfo::FindParticles(const shared_str& ps_name)
{
	for (ParticlesInfoListIt it=particles.begin(); it!=particles.end(); it++)
		if (it->ps->Name()==ps_name) return &(*it);
	return 0;
}
CParticlesPlayer::SParticlesInfo* CParticlesPlayer::SBoneInfo::AppendParticles(CObject* object, const shared_str& ps_name)
{
	SParticlesInfo* pi	= FindParticles(ps_name);
	if (pi)				return pi;
	particles.push_back	(SParticlesInfo());
	pi					= &particles.back();
	pi->ps				= CParticlesObject::Create(*ps_name,FALSE);
	return pi;
}
void CParticlesPlayer::SBoneInfo::StopParticles(const shared_str& ps_name)
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
	bone_mask			= u64(1)<<u64(0);
	
	m_bActiveBones		= false;

	m_Bones.push_back	(SBoneInfo(0,Fvector().set(0,0,0)));

	SetParentVel		(zero_vel);
	m_self_object		= 0;
}

CParticlesPlayer::~CParticlesPlayer ()
{
	VERIFY				(!m_self_object);
}

void CParticlesPlayer::LoadParticles(CKinematics* K)
{
	VERIFY				(K);

	m_Bones.clear();
	m_Bones.push_back	(SBoneInfo(K->LL_GetBoneRoot(),Fvector().set(0,0,0)));

	//������� ������ �������� � ���������������
	//�������  ���� ����� ������ ��������
	CInifile* ini		= K->LL_UserData();
	if(ini&&ini->section_exist("particle_bones")){
		bone_mask		= 0;
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
//����������� ��������� �� net_Destroy
void	CParticlesPlayer::net_DestroyParticles	()
{
	VERIFY(m_self_object);

	for(BoneInfoVecIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
	{
		SBoneInfo& b_info	= *b_it;

		for (ParticlesInfoListIt p_it=b_info.particles.begin(); p_it!=b_info.particles.end(); p_it++)
		{
			SParticlesInfo& p_info	= *p_it;
			CParticlesObject::Destroy(p_info.ps);
		}
		b_info.particles.clear();
	}

	m_self_object	= 0;
}

CParticlesPlayer::SBoneInfo* CParticlesPlayer::get_nearest_bone_info(CKinematics* K, u16 bone_index)
{
	u16 play_bone	= bone_index;
	//u16 cur_bone	= play_bone;
	while((BI_NONE!=play_bone)&&!(bone_mask&(u64(1)<<u64(play_bone))))
	{
		//cur_bone = play_bone;
		play_bone	= K->LL_GetData(play_bone).GetParentID();
	}
	//return get_bone_info(cur_bone);
	return get_bone_info(play_bone);
}


void CParticlesPlayer::StartParticles(const shared_str& particles_name, u16 bone_num, const Fvector& dir, u16 sender_id, int life_time, bool auto_stop)
{
	Fmatrix xform;
	generate_orthonormal_basis(dir,xform);
	StartParticles(particles_name,bone_num,xform,sender_id,life_time,auto_stop);
}
void CParticlesPlayer::StartParticles(const shared_str& particles_name, u16 bone_num, const Fmatrix& xform, u16 sender_id, int life_time, bool auto_stop)
{

	VERIFY(fis_zero(xform.c.magnitude()));
	R_ASSERT(*particles_name);
	
	CObject* object					= m_self_object;
	VERIFY(object);

	//����� ��������� ���������� ��������, ����� �������� ��������
	SBoneInfo* pBoneInfo			=  get_nearest_bone_info(smart_cast<CKinematics*>(object->Visual()),bone_num);
	if(!pBoneInfo) return;

	SParticlesInfo &particles_info	=*pBoneInfo->AppendParticles(object,particles_name);
	
	//particles_info->dir				= dir;

	particles_info.sender_id		= sender_id;

	//particles_info->life_time		= life_time;
	//particles_info->cur_time		= 0;
	//particles_info->auto_stop		= auto_stop;
	particles_info.life_time=auto_stop ? life_time : u32(-1);
	//particles_info->x_form			.set(xform);
	xform.getHPB(particles_info.angles);
	//������ ������ ��������

	//MakeXFORM						(object,pBoneInfo->index,dir,pBoneInfo->offset,particles_info->x_form);
	Fmatrix m;m.setHPB(particles_info.angles.x,particles_info.angles.y,particles_info.angles.z);
	GetBonePos(object,pBoneInfo->index,pBoneInfo->offset,m.c);
	particles_info.ps->UpdateParent(m,zero_vel);
	if(!particles_info.ps->IsPlaying())
		particles_info.ps->Play		();

	m_bActiveBones = true;
}

void CParticlesPlayer::StartParticles(const shared_str& ps_name, const Fvector& dir, u16 sender_id, int life_time, bool auto_stop)
{
	Fmatrix xform;
	generate_orthonormal_basis(dir,xform);
	StartParticles(ps_name,xform,sender_id,life_time,auto_stop);
}

void CParticlesPlayer::StartParticles(const shared_str& ps_name, const Fmatrix& xform, u16 sender_id, int life_time, bool auto_stop)
{
	
	CObject* object					= m_self_object;
	VERIFY(object);
	for(BoneInfoVecIt it = m_Bones.begin(); it!=m_Bones.end(); it++){
		
		SParticlesInfo &particles_info	=*it->AppendParticles(object,ps_name);
		//particles_info->dir			= dir;
		particles_info.sender_id	= sender_id;

		//particles_info->life_time		= life_time;
		//particles_info->cur_time		= 0;
		//particles_info->auto_stop		= auto_stop;
		particles_info.life_time=auto_stop ? life_time : u32(-1);
		//VERIFY(life_time==u32)
		//particles_info->x_form			.set(xform);
		xform.getHPB(particles_info.angles);
		//������ ������ ��������

		//MakeXFORM					(object,it->index,dir,it->offset,particles_info->x_form);
		Fmatrix m;m.set(xform);
		GetBonePos(object,it->index,it->offset,m.c);
		particles_info.ps->UpdateParent(m,zero_vel);
		if(!particles_info.ps->IsPlaying())
			particles_info.ps->Play	();
	}

	m_bActiveBones = true;
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

void CParticlesPlayer::StopParticles(const shared_str& ps_name, u16 bone_id)
{
	if (BI_NONE==bone_id){
		for(BoneInfoVecIt it=m_Bones.begin(); it!=m_Bones.end(); it++)
			it->StopParticles	(ps_name);
	}else{
		SBoneInfo* bi			= get_bone_info(bone_id); VERIFY(bi);
		bi->StopParticles		(ps_name);
	}
}

//��������� ���������, �� ��������� �� ������� �����
void CParticlesPlayer::AutoStopParticles(const shared_str& ps_name, u16 bone_id,u32 life_time)
{
	if (BI_NONE==bone_id){
		for(BoneInfoVecIt it=m_Bones.begin(); it!=m_Bones.end(); it++)
		{
			SParticlesInfo* pInfo = it->FindParticles	(ps_name);
			if(pInfo) pInfo->life_time = life_time;
		}
	}else{
		SBoneInfo* bi			= get_bone_info(bone_id); VERIFY(bi);
		SParticlesInfo* pInfo = bi->FindParticles	(ps_name);
		if(pInfo) pInfo->life_time = life_time;
	}
}
struct SRP
{
	bool operator	() (CParticlesPlayer::SParticlesInfo& pi)
	{
		return ! pi.ps;
	}
};
void CParticlesPlayer::UpdateParticles()
{
	VERIFY	(m_bActiveBones);
	m_bActiveBones			= false;

    CObject* object			= m_self_object;
	VERIFY	(object);

	for(BoneInfoVecIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
		SBoneInfo& b_info	= *b_it;

		for (ParticlesInfoListIt p_it=b_info.particles.begin(); p_it!=b_info.particles.end(); p_it++){
			SParticlesInfo& p_info	= *p_it;
			VERIFY(p_info.ps);
			//�������� ������� ���������
			Fmatrix xform;
			//MakeXFORM				(object,b_info.index,p_info.dir,b_info.offset,xform);
			xform.setHPB(p_info.angles.x,p_info.angles.y,p_info.angles.z);
			GetBonePos(object,b_info.index,b_info.offset,xform.c);
			p_info.ps->UpdateParent(xform, parent_vel);

			//�������� ����� �������������
			//p_info.cur_time += Device.dwTimeDelta;
			//if(p_info.life_time>0 && p_info.auto_stop && p_info.cur_time>p_info.life_time)
			//	p_info.ps->Stop();
			if(p_info.life_time!=u32(-1))
			{
				if(p_info.life_time>Device.dwTimeDelta)	p_info.life_time-=Device.dwTimeDelta;
				else 
				{
					p_info.ps->Stop();
					p_info.life_time=u32(-1);
				}
			}
			if(!p_info.ps->IsPlaying()){
				CParticlesObject::Destroy(p_info.ps);
				//ParticlesInfoListIt cur_it	= p_it++;
				//b_info.particles.erase		(cur_it);
			}
			else
				m_bActiveBones  = true;
		}

		ParticlesInfoListIt RI=std::remove_if(b_info.particles.begin(),b_info.particles.end(),SRP());
		b_info.particles.erase(RI,b_info.particles.end());
	}
}


void CParticlesPlayer::GetBonePos	(CObject* pObject, u16 bone_id, const Fvector& offset, Fvector& result)
{
	VERIFY(pObject);
	CKinematics* pKinematics = smart_cast<CKinematics*>(pObject->Visual()); VERIFY(pKinematics);
	CBoneInstance&		l_tBoneInstance = pKinematics->LL_GetBoneInstance(bone_id);

	result = offset;
	l_tBoneInstance.mTransform.transform_tiny(result);
	pObject->XFORM().transform_tiny(result);
}

void CParticlesPlayer::MakeXFORM	(CObject* pObject, u16 bone_id, const Fvector& dir, const Fvector& offset, Fmatrix& result)
{
	generate_orthonormal_basis(dir,result);
	GetBonePos(pObject, bone_id, offset, result.c);
}

u16 CParticlesPlayer::GetNearestBone	(CKinematics* K, u16 bone_id)
{
	u16 play_bone	= bone_id;
	//u16 cur_bone = play_bone;

	while((BI_NONE!=play_bone)&&!(bone_mask&(u64(1)<<u64(play_bone))))
	{
		//cur_bone = play_bone;
		play_bone	= K->LL_GetData(play_bone).GetParentID();
	}
	//return cur_bone;
	return play_bone;
}

void CParticlesPlayer::net_SpawnParticles	()
{
	VERIFY				(!m_self_object);
	m_self_object		= smart_cast<CObject*>(this);
	VERIFY				(m_self_object);
}
