///////////////////////////////////////////////////////////////
// ParticlesPlayer.h
// ��������� ��� ������������ ��������� �� �������
///////////////////////////////////////////////////////////////

#pragma once

#include "ParticlesObject.h"


DEFINE_VECTOR(CParticlesObject*, PARTICLES_PTR_VECTOR, PARTICLES_PTR_VECTOR_IT);


class CParticlesPlayer 
{
private:
	//��������� � ���������� ����������� � ��������
	struct SParticlesInfo
	{
		CParticlesObject*	ps;
		Fvector				dir;
		u16					sender_id;
	};
	DEFINE_LIST				(SParticlesInfo,ParticlesInfoList,ParticlesInfoListIt);
	//��������� ��� �������� � ������� ���������� ���������
	struct SBoneInfo
	{
		u16					index;
		Fvector				offset;
		ParticlesInfoList	particles;
		SParticlesInfo*		FindParticles			(ref_str ps_name);
	public:
							SBoneInfo				(u16 idx, const Fvector& offs):index(idx),offset(offs){;}
		SParticlesInfo*		AppendParticles			(CObject* object, ref_str ps_name);
		void				StopParticles			(ref_str ps_name);
		void				StopParticles			(u16 sender_id);
	} ;
	DEFINE_VECTOR			(SBoneInfo,BoneInfoVec,BoneInfoVecIt);

	// ������ ������
	u64						bone_mask; // ������������ �����
	BoneInfoVec				m_Bones;
private:
	IC SBoneInfo*			get_bone_info			(u16 bone_index)
	{
		if (BI_NONE==bone_index) return 0;
		for (BoneInfoVecIt it=m_Bones.begin(); it!=m_Bones.end(); it++)
			if (it->index==bone_index) return &(*it);
		return 0;
	}
	SBoneInfo*				get_nearest_bone_info	(CKinematics* K, u16 bone_index);
public:
							CParticlesPlayer		(void);
	virtual					~CParticlesPlayer		(void);
	void					Load					(CKinematics* K);
	
	void					UpdateParticles			();

	void					StartParticles			(ref_str ps_name, u16 bone_num,  const Fvector& dir, u16 sender_id);
	void					StartParticles			(ref_str ps_name, const Fvector& dir, u16 sender_id);

	void					StopParticles			(u16 sender_ID, u16 bone_id=BI_NONE);
	void					StopParticles			(ref_str particles_name, u16 bone_id=BI_NONE);

	void					MakeXFORM				(CObject* pObject, u16 bone_id, const Fvector& dir, const Fvector& offset, Fmatrix& result);
	u16						GetNearestBone			(CKinematics* K, u16 bone_id);
	IC u16					GetRandomBone			(){return m_Bones.size()?Random.randI(m_Bones.size()):BI_NONE;}
};