#pragma once

//*********************************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMORY MANAGMENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************************

typedef u32 TTime;

class CAI_Biting;
class Feel::Vision;

typedef enum {
	WEAPON_SHOOTING = 0,
	MONSTER_ATTACKING,
	WEAPON_BULLET_RICOCHET,
	WEAPON_RECHARGING,
	
	WEAPON_TAKING,
	WEAPON_HIDING,			
	WEAPON_CHANGING,
	WEAPON_EMPTY_CLICKING,

	MONSTER_DYING,	
	MONSTER_INJURING,
	MONSTER_WALKING,			
	MONSTER_JUMPING,			
	MONSTER_FALLING,			
	MONSTER_TALKING,			

	DOOR_OPENING,		
	DOOR_CLOSING,		
	OBJECT_BREAKING,	
	OBJECT_FALLING,
	NONE_DANGEROUS_SOUND
} TSoundDangerValue;


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSoundMemory class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct tagSoundElement
{
	CObject				*who;
	TSoundDangerValue	type;
	Fvector				Position;
	float				power;
	TTime				time;			// врем€ обнаружени€ звука

	tagSoundElement() {
		Fvector pos;
		pos.set(0,0,0);
		Set(0,NONE_DANGEROUS_SOUND,pos,0.f,0);
	}
	bool operator < (const tagSoundElement &s) const  {
		return (type < s.type);
	}
	void operator = (const tagSoundElement &s) {
		Set(s.who, s.type, s.Position, s.power, s.time);
	}
	IC void Set(CObject* who, int eType, const Fvector &Position, float power, TTime time) {
		this->who = who; type = ConvertSoundType((ESoundTypes)eType); this->Position = Position; this->power = power; this->time = time;
	}
	TSoundDangerValue ConvertSoundType(ESoundTypes stype);
} SoundElem;


class CSoundMemory
{
	TTime	MemoryTime;				// врем€ хранени€ звуков
	xr_vector<SoundElem> Sounds;

	TTime	CurrentTime;			// текущее врем€
public:
	void HearSound(const SoundElem &s);
	void HearSound(CObject* who, int eType, const Fvector &Position, float power, TTime time);
	bool IsRememberSound();		//
	void GetMostDangerousSound(SoundElem &s);	// возвращает самый опасный звук

protected:
	void Init(TTime mem_time) {MemoryTime = mem_time;}
	void Deinit() {Sounds.clear();}

	void UpdateHearing(TTime dt);
	void ShowDbgInfo();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVisionMemory class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct tagVisionElem
{
	CEntity			*obj;
	Fvector			position;
	NodeCompressed	*node;
	u32				node_id;
	TTime			time;

	tagVisionElem() {obj = 0;}

	IC void Set(CEntity *pE, TTime t) {
		obj = pE; 	position = pE->Position(); 	node = pE->AI_Node;  node_id = pE->AI_NodeID; time = t; 
	}

	bool operator == (const tagVisionElem &ve) {
		return (obj == ve.obj);
	}
} VisionElem;


class CVisionMemory : public Feel::Vision
{
	TTime	MemoryTime;				// врем€ хранени€ визуальных объектов

	xr_vector<VisionElem>	Objects;
	xr_vector<VisionElem>	Enemies;

	TTime	CurrentTime;			// текущее врем€
public:
	IC bool IsRememberVisual() {return (IsEnemy() || IsObject());}
	IC bool IsEnemy() {return (!Enemies.empty());}	 
	IC bool IsObject() {return (!Objects.empty());}	 

	void GetNearestEnemy(const Fvector &pos,CEntity *e);
	void GetNearestObject(const Fvector &pos,CEntity *e);

	
protected:
	void Init(TTime mem_time) {MemoryTime = mem_time;}
	void Deinit() {Objects.clear(); Enemies.clear();}

	void UpdateVision(TTime dt, CAI_Biting *pThis);
	void ShowDbgInfo();

private:
	void AddObject(const VisionElem &ve);
	void AddEnemy(const VisionElem &ve);
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMonsterMemory class
/////////////////////////////////////////////////////////////////////////////////////////////////////////
class CMonsterMemory : public CSoundMemory, public CVisionMemory {
	CAI_Biting	*pData;
public:
	void Init(TTime sound_mem, TTime vision_mem, CAI_Biting *ptr){
		CSoundMemory::Init(sound_mem);
		CVisionMemory::Init(vision_mem);
		pData = ptr;
	}
	void Deinit() {
		CSoundMemory::Deinit();
		CVisionMemory::Deinit();
	}

	void UpdateMemory();

	void ShowDbgInfo();
};
