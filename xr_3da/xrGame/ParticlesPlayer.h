///////////////////////////////////////////////////////////////
// ParticlesPlayer.h
// ��������� ��� ������������ ��������� �� �������
///////////////////////////////////////////////////////////////

#pragma once

#include "ParticlesObject.h"


DEFINE_VECTOR(CParticlesObject*, PARTICLES_PTR_VECTOR, PARTICLES_PTR_VECTOR_IT);


class CParticlesPlayer 
{

public:
	//��������� � ���������� ����������� � ��������
	struct SParticlesInfo
	{
		CParticlesObject*	ps;
		Fvector				dir;

		u16					sender_id;	//id - �������, ������� �������� ��������
		int					life_time;	//����� ����� �������� (-1) - ����������
		
		int					cur_time;	//������� ����� ������������� ��������
		bool				auto_stop;	//�������������� ��������� ���������, ����� ���������� �����
	};
	DEFINE_LIST				(SParticlesInfo,ParticlesInfoList,ParticlesInfoListIt);

	//��������� ��� �������� � ������� ���������� ���������
	struct SBoneInfo
	{
		u16					index;
		Fvector				offset;
		ParticlesInfoList	particles;
		SParticlesInfo*		FindParticles			(const ref_str& ps_name);
	public:
							SBoneInfo				(u16 idx, const Fvector& offs):index(idx),offset(offs){;}
		SParticlesInfo*		AppendParticles			(CObject* object, const ref_str& ps_name);
		void				StopParticles			(const ref_str& ps_name);
		void				StopParticles			(u16 sender_id);
	};
	DEFINE_VECTOR			(SBoneInfo,BoneInfoVec,BoneInfoVecIt);

private:
	// ������ ������
	u64						bone_mask; // ������������ �����
	BoneInfoVec				m_Bones;
	bool					m_bActiveBones;	//���� �� �������� �� ������� �������� ��������
public:
	IC SBoneInfo*			get_bone_info			(u16 bone_index)
	{
		if (BI_NONE==bone_index) return 0;
		for (BoneInfoVecIt it=m_Bones.begin(); it!=m_Bones.end(); it++)
			if (it->index==bone_index) return &(*it);
		return 0;
	}
	SBoneInfo*				get_nearest_bone_info	(CKinematics* K, u16 bone_index);
	Fvector					parent_vel;
public:
							CParticlesPlayer		(void);
	virtual					~CParticlesPlayer		(void);
	void					Load					(CKinematics* K);
	void					net_DestroyParticles	();
	
	void					UpdateParticles			();

	void					StartParticles			(const ref_str& ps_name, u16 bone_num,  const Fvector& dir, u16 sender_id, int life_time = -1, bool auto_stop = true);
	void					StartParticles			(const ref_str& ps_name, const Fvector& dir, u16 sender_id, int life_time = -1, bool auto_stop = true);

	void					StopParticles			(u16 sender_ID, u16 bone_id=BI_NONE);
	void					StopParticles			(const ref_str& particles_name, u16 bone_id=BI_NONE);
	void					AutoStopParticles		(const ref_str& ps_name, u16 bone_id);

	static void				MakeXFORM				(CObject* pObject, u16 bone_id, const Fvector& dir, const Fvector& offset, Fmatrix& result);
	static void				GetBonePos				(CObject* pObject, u16 bone_id, const Fvector& offset, Fvector& result);
	u16						GetNearestBone			(CKinematics* K, u16 bone_id);
	IC u16					GetRandomBone			(){return m_Bones.size()?(u16)Random.randI(m_Bones.size()):BI_NONE;}

	void					SetParentVel			(const Fvector& vel) {parent_vel = vel;}
};