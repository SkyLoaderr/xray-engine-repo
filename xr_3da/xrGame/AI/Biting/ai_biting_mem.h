#pragma once

//#pragma once
//
////*********************************************************************************************************
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// MEMORY MANAGMENT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
////*********************************************************************************************************
//
//typedef u32 TTime;
//
//class CAI_Biting;
//class Feel::Vision;
//
//typedef enum {
//	WEAPON_SHOOTING = 0,
//	MONSTER_ATTACKING,
//	WEAPON_BULLET_RICOCHET,
//	WEAPON_RECHARGING,
//	
//	WEAPON_TAKING,
//	WEAPON_HIDING,			
//	WEAPON_CHANGING,
//	WEAPON_EMPTY_CLICKING,
//
//	MONSTER_DYING,	
//	MONSTER_INJURING,
//	MONSTER_WALKING,			
//	MONSTER_JUMPING,			
//	MONSTER_FALLING,			
//	MONSTER_TALKING,			
//
//	DOOR_OPENING,		
//	DOOR_CLOSING,		
//	OBJECT_BREAKING,	
//	OBJECT_FALLING,
//	NONE_DANGEROUS_SOUND
//} TSoundDangerValue;
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// CSoundMemory class
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//typedef struct tagSoundElement
//{
//	CObject				*who;
//	TSoundDangerValue	type;
//	Fvector				Position;
//	float				power;
//	TTime				time;			// врем€ обнаружени€ звука
//
//	tagSoundElement() {
//		who = 0; 
//		type = NONE_DANGEROUS_SOUND; 
//		Position.set(0,0,0); 
//		power = 0.f; 
//		time = 0;
//	}
//	
//	bool operator < (const tagSoundElement &s) const  {
//		return (type < s.type);
//	}
//
//	void operator = (const tagSoundElement &s) {
//		who = s.who; type = s.type; Position = s.Position; power = s.power; time = s.time;
//	}
//
//} SoundElem;
//
//
//class CSoundMemory
//{
//	TTime	MemoryTime;				// врем€ хранени€ звуков
//	xr_vector<SoundElem> Sounds;
//
//	TTime	CurrentTime;			// текущее врем€
//public:
//	void Init(TTime mem_time) {MemoryTime = mem_time;}
//	void Deinit() {Sounds.clear();}
//
//	void HearSound(const SoundElem &s);
//	bool IsRememberSound();		//
//	void GetMostDangerousSound(SoundElem &s);	// возвращает самый опасный звук
//
//	TSoundDangerValue ConvertSoundType(ESoundTypes stype);
//
//	void UpdateHearing(TTime dt);
//};
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// CVisionMemory class
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//typedef struct tagVisionElem
//{
//	CEntity			*obj;
//	Fvector			position;
//	NodeCompressed	*node;
//	u32				node_id;
//	TTime			time;
//
//	void Set(CEntity *pE, TTime t) {
//		obj = pE;
//		position = pE->Position();
//		node = pE->AI_Node;
//		node_id = pE->AI_NodeID;
//		time = t;
//	}
//
//	bool operator == (const tagVisionElem &ve) {
//		return (obj == ve.obj);
//	}
//} VisionElem;
//
//
//class CVisionMemory : public Feel::Vision
//{
//	TTime	MemoryTime;				// врем€ хранени€ визуальных объектов
//
//	xr_vector<VisionElem>	Objects;
//	xr_vector<VisionElem>	Enemies;
//
//	TTime	CurrentTime;			// текущее врем€
//public:
//	void Init(TTime mem_time) {MemoryTime = mem_time;}
//	void Deinit() {Objects.clear(); Enemies.clear();}
//	
//	void UpdateVision(TTime dt, CAI_Biting *pThis);
//	
//	IC bool IsRememberVisual() {return (IsEnemy() || IsObject());}
//	IC bool IsEnemy() {return (!Enemies.empty());}	 
//	IC bool IsObject() {return (!Objects.empty());}	 
//
//	void GetNearestEnemy(const Fvector &pos,CEntity *e);
//	void GetNearestObject(const Fvector &pos,CEntity *e);
//
//private:
//	void AddObject(const VisionElem &ve);
//	void AddEnemy(const VisionElem &ve);
//};
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//// CMonsterMemory class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//class CMonsterMemory : public CSoundMemory, public CVisionMemory {
//
//public:
//	void Init(TTime sound_mem, TTime vision_mem){
//		CSoundMemory::Init(sound_mem);
//		CVisionMemory::Init(vision_mem);
//	}
//	void Deinit() {
//		CSoundMemory::Deinit();
//		CVisionMemory::Deinit();
//	}
//
//	void UpdateMemory();
//};
