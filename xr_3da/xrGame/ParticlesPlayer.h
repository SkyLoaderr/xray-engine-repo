///////////////////////////////////////////////////////////////
// ParticlesPlayer.h
// интерфейс дл€ проигрывани€ партиклов на объекте
///////////////////////////////////////////////////////////////

#pragma once

#include "ParticlesObject.h"


//информаци€ о косточке на которой будут проигрыватьс€ партиклы
typedef struct
{
	int index;
	Fvector offset;
} SBoneInfo;


DEFINE_VECTOR(CParticlesObject*, PARTICLES_PTR_VECTOR, PARTICLES_PTR_VECTOR_IT);
DEFINE_VECTOR(SBoneInfo, BONE_INFO_VECTOR, BONE_INFO_VECTOR_IT);

//структура с внутренней информацией о партикле
typedef struct 
{
	ref_str particles_name;
	
	int bone;
	Fvector bone_pos;
	Fvector dir;

	bool auto_remove;
	u16 sender_id;
} SParticlesInfo;


class CParticlesPlayer 
{
public:
	CParticlesPlayer			(void);
	virtual ~CParticlesPlayer	(void);

	
	//функции, которые определ€ютс€ в порожденном объекте
	virtual IRender_Sector*	Sector  () = 0;
	virtual void PHGetLinearVell (Fvector& velocity) = 0;

	virtual void UpdateParticles	();

	virtual CParticlesObject* StartParticles(ref_str particles_name,
											 int bone_num, 
											 const Fvector& bone_pos,
											 const Fvector& dir,
											 u16 sender_id,
											 bool auto_remove = true);
	
	virtual void StopParticles(CParticlesObject* particles_object);
	virtual void StopParticles(PARTICLES_PTR_VECTOR& particles_vector);
	
	virtual void StopParticles(u32 sender_ID);
	virtual void StopParticles(ref_str particles_name);


	virtual void StartParticlesOnAllBones(PARTICLES_PTR_VECTOR& particles_vector,
										  ref_str particles_name,
										  u16 sender_id,
										  bool auto_remove = true);

	virtual void UpdateParticlesPosition(CParticlesObject* pParticles,
										 const SParticlesInfo* pInfo);

	virtual BONE_INFO_VECTOR& GetParticleBones() {return m_ParticlesBonesList;}

protected:
	DEFINE_MAP(CParticlesObject*, SParticlesInfo*, PARTICLES_INFO_MAP, PARTICLES_INFO_MAP_IT);
	PARTICLES_INFO_MAP m_ParticlesInfoMap;

	//список косточек на которые могут цепл€тьс€ партиклы
	BONE_INFO_VECTOR m_ParticlesBonesList;
};