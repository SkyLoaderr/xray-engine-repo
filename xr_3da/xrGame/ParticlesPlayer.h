///////////////////////////////////////////////////////////////
// ParticlesPlayer.h
// интерфейс для проигрывания партиклов на объекте
///////////////////////////////////////////////////////////////

#pragma once

#include "ParticlesObject.h"


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

	
	//функции, которые определяются в порожденном объекте
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
	virtual void StopParticles(u32 sender_ID);
	virtual void StopParticles(ref_str particles_name);

	virtual void UpdateParticlesPosition(CParticlesObject* pParticles,
										 const SParticlesInfo* pInfo);


protected:
	DEFINE_MAP(CParticlesObject*, SParticlesInfo*, PARTICLES_INFO_MAP, PARTICLES_INFO_MAP_IT);
	PARTICLES_INFO_MAP m_ParticlesInfoMap;
};