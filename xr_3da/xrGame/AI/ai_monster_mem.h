#pragma once

//*********************************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMORY MANAGMENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************************

#define TIME_TO_RESELECT_ENEMY	3000

#define DEFINE_THIS_CLASS_AS_POLYMORPHIC() virtual void __VirtualFunctionThatMakesClassPolymorphic() {}

typedef u32 TTime;


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
	Fvector				Position;		// ������� �����, �� �������, ��������� ����
	float				power;
	TTime				time;			// ����� ����������� �����

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
	TTime	MemoryTime;				// ����� �������� ������
	xr_vector<SoundElem> Sounds;

	TTime	CurrentTime;			// ������� �����
public:
	void HearSound(const SoundElem &s);
	void HearSound(CObject* who, int eType, const Fvector &Position, float power, TTime time);
	bool IsRememberSound();		//
	void GetMostDangerousSound(SoundElem &s, bool &bDangerous);	// ���������� ����� ������� ����

protected:
	void Init(TTime mem_time) {MemoryTime = mem_time;}
	void Deinit() {Sounds.clear();}

	void UpdateHearing(TTime dt);
};


//---------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVisionMemory class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------

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
	void operator = (const tagVisionElem &ve) {
		obj = ve.obj; 	position = ve.position; node = ve.node;  node_id = ve.node_id; time = ve.time; 
	}

	bool operator == (const tagVisionElem &ve) {
		return (obj == ve.obj);
	}
} VisionElem;

/////////////////////////////////////////////////////////////////////////////////////////////
// CVisionMemory class
class CVisionMemory
{
	TTime					MemoryTime;				// ����� �������� ���������� ��������
	TTime					CurrentTime;			// ������� �����
	
	xr_vector<VisionElem>	Objects;
	xr_vector<VisionElem>	Enemies;

	VisionElem				Selected;
	VisionElem				Saved;

	enum EObjectType {ENEMY, OBJECT};

	CCustomMonster			*pMonster;

public:
	IC	bool		IsEnemy() {return (!Enemies.empty());}	 
	IC	bool		IsObject() {return (!Objects.empty());}	 

	IC	bool		GetEnemy(VisionElem &ve) {return Get(ve);} 	
	IC	bool		GetCorpse(VisionElem &ve) {return Get(ve);}

	IC	void		SaveEnemy() {if (Selected.obj) Saved = Selected;}

protected:
		void		Init(TTime mem_time);
		void		Deinit();

		// ��������� ������� Objects � Enemies � Selected
		void		UpdateVision(TTime dt);
	
	DEFINE_THIS_CLASS_AS_POLYMORPHIC();

private:
		void		AddObject(const VisionElem &ve);
		void		AddEnemy(const VisionElem &ve);
	
		bool		Get(VisionElem &ve);

		void		SelectEnemy();
		void		SelectCorpse();

		VisionElem	&GetNearestObject(EObjectType obj_type = ENEMY);
};

//---------------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMonsterMemory class
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------

class CMonsterMemory : public CSoundMemory, public CVisionMemory {
	
public:
	void InitMemory(TTime sound_mem, TTime vision_mem){
		CSoundMemory::Init(sound_mem);
		CVisionMemory::Init(vision_mem);
	}
	void DeinitMemory() {
		CSoundMemory::Deinit();
		CVisionMemory::Deinit();
	}

	void UpdateMemory();
};
