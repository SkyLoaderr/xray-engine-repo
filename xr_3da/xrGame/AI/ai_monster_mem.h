#pragma once

#include "ai_monster_defs.h"
#include "../entity.h"
#include "../level_graph.h"

class CCustomMonster;

//*********************************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMORY MANAGMENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************************

#define TIME_TO_RESELECT_ENEMY			3000
#define TIME_TO_UPDATE_IGNORE_OBJECTS	3000

#define DEFINE_THIS_CLASS_AS_POLYMORPHIC() virtual void __VirtualFunctionThatMakesClassPolymorphic() {}

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
	const CObject		*who;
	TSoundDangerValue	type;
	Fvector				position;		// позиция звука, не объекта, издавшего звук
	float				power;
	TTime				time;			// время обнаружения звука

	tagSoundElement() {  
		who = 0; type = NONE_DANGEROUS_SOUND; position.set(0,0,0); power = 0.f; time = 0;
	}
	bool operator < (const tagSoundElement &s) const  { 
		return (type < s.type);
	}
	IC void SetConvert(const CObject* who, int eType, const Fvector &position, float power, TTime time) {
		this->who = who; type = ConvertSoundType((ESoundTypes)eType); this->position = position; this->power = power; this->time = time;
	}
	TSoundDangerValue ConvertSoundType(ESoundTypes stype);
} SoundElem;

// удаление звуков от объектов, перешедших в оффлайн
struct remove_offline_sound_pred {
	bool operator() (const SoundElem &x){ return (x.who && x.who->getDestroy()); }
};

// предикат удаления 'старых' старых
struct predicate_remove_old_sounds {
	TTime new_time;
	predicate_remove_old_sounds(TTime time) { new_time = time; }
	bool operator() (const SoundElem &x) { return (x.time < new_time); }
};

// удалить все звуки, принадлежащие данному объекту
struct remove_sound_owner_pred {
	const CObject *pO;
	remove_sound_owner_pred(const CObject *o) {o = pO;}
	bool operator() (const SoundElem &x){ return (x.who == pO); }
};

struct remove_dead_objects_pred {
	bool operator() (const SoundElem &x){ 
		if (x.who) {
			const CEntityAlive *pE = dynamic_cast<const CEntityAlive*> (x.who);
			if (pE && pE->g_Alive()) return false;
		}
		return true;
	}
};

class CSoundMemory
{
	TTime					timeMemory;				// время хранения звуков
	xr_vector<SoundElem>	Sounds;

	TTime					timeCurrent;			// текущее время

	friend class CMonsterMemory;

public:
		void	HearSound				(const SoundElem &s);
		void	HearSound				(const CObject* who, int eType, const Fvector &Position, float power, TTime time);
	IC	bool	IsRememberSound			() {return (!Sounds.empty());}		
		void	GetSound				(SoundElem &s, bool &bDangerous);	// возвращает самый опасный звук

protected:
	IC	void	Init					(TTime mem_time) {Deinit(); timeMemory = mem_time; }
	IC	void	Deinit					() {Sounds.clear();}

		void	UpdateHearing			(TTime dt);

		void    RemoveSoundOwner		(const CObject *pO);	//удалить все звуки принадлежащие данному объекту
private:
		void	CheckValidObjects		(); 			// удалить объекты которые не прошли тест на GetDestroyed(), т.е. ушли в оффлайн
		void	RemoveDeadObjects		();				// удалить звуки, от мёртвых объектов
};


//---------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVisionMemory class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------

typedef struct tagVisionElem
{
	const CEntity				*obj;
	Fvector						position;
	const CLevelGraph::CVertex	*vertex;
	u32							node_id;
	TTime						time;

	tagVisionElem() {obj = 0;}

	IC void Set(const CEntity *pE, TTime t) {
		obj = pE; 	position = pE->Position(); 	vertex = pE->level_vertex();  node_id = pE->level_vertex_id(); time = t; 
	}
	void operator = (const tagVisionElem &ve) {
		obj = ve.obj; 	position = ve.position; vertex = ve.vertex;  node_id = ve.node_id; time = ve.time; 
	}

	bool operator == (const tagVisionElem &ve) {
		return (ve.obj == obj);
	}

} VisionElem;


// предикат удаления 'старых' объектов
struct predicate_remove_old_objects {

	TTime new_time;
	predicate_remove_old_objects(TTime time) { new_time = time; }
	bool operator() (const VisionElem &x) { return (x.time < new_time); }

};

// предикат удаления 'старых' врагов
struct predicate_remove_old_enemies {

	TTime	cur_time;
	TTime	mem_time;
	Fvector monster_pos;
	predicate_remove_old_enemies(TTime c_time, TTime m_time, Fvector m_pos) { 
		cur_time = c_time;	mem_time = m_time;	monster_pos = m_pos;
	}
	bool operator() (const VisionElem &x) { 
		return ((x.time + mem_time < cur_time) || (!x.obj) || (!x.obj->g_Alive()) || 
			    ((x.obj->Position().distance_to(monster_pos) > 30) && (x.time != cur_time))); 
	}
};


// предикат удаления 'старых' врагов
struct predicate_remove_ignore_objects {
	const CEntity *pObj;

	predicate_remove_ignore_objects(const CEntity *p) { pObj = p; }
	
	bool operator() (const VisionElem &x) { 
		return (pObj == x.obj); 
	}
};


// предикат удаления старых объектов, перешедших в оффлайн
struct predicate_remove_offline {
	bool operator() (const VisionElem &x) { return (x.obj && x.obj->getDestroy()); }
};


/////////////////////////////////////////////////////////////////////////////////////////////
// CVisionMemory class
class CVisionMemory
{
	TTime					timeMemoryDefault;
	TTime					timeMemory;				// время хранения визуальных объектов
	TTime					timeCurrent;			// текущее время
	
	typedef xr_vector			<VisionElem> VECTOR_VE;
	typedef VECTOR_VE::iterator	ITERATOR_VE;	

	VECTOR_VE				Objects;
	VECTOR_VE				Enemies;

	VisionElem				Selected;
	enum EObjectType {ENEMY, OBJECT};

	CCustomMonster			*pMonster;
	
	VECTOR_VE				IgnoreObjects;
	TTime					timeLastUpdateIgnoreObjects;

	friend class CMonsterMemory;
		
public:
	IC	bool		IsEnemy			() {return (!Enemies.empty());}	 
	IC	bool		IsObject		() {return (!Objects.empty());}	 

	IC	bool		GetEnemy		(VisionElem &ve) {if (IsEnemy())  return Get(ve); else return false;} 	
	IC	bool		GetCorpse		(VisionElem &ve) {if (IsObject()) return Get(ve); else return false;}
	
	IC	void		AddCorpse		(const VisionElem &ve) {AddObject(ve);}

	IC	void		SetMemoryTime	(TTime t) {timeMemory = t;}
	IC	void		SetMemoryTimeDef() {timeMemory = timeMemoryDefault;}

		void		AddIgnoreObject	(const CEntity *pObj);
protected:
		void		Init			(TTime mem_time);
		void		Deinit			();

		// Заполняет массивы Objects и Enemies и Selected
		void		UpdateVision	(TTime dt);
	
		DEFINE_THIS_CLASS_AS_POLYMORPHIC();

private:
		void		AddObject		(const VisionElem &ve);
		void		AddEnemy		(const VisionElem &ve);
	
		bool		Get				(VisionElem &ve);

		void		SelectEnemy		();
		void		SelectCorpse	();

		VisionElem	&GetNearestObject(EObjectType obj_type = ENEMY);
		
		// удалить 'старые' объекты и врагов
		void		RemoveOldElems	();
		
		// удалить объекты которые не прошли тест на getDestroyed(), т.е. ушли в оффлайн
		void		CheckValidObjects();

		// удаление старых игнор-объектов 
		void		RemoveOldIgnoreObjects();
		
		// удалить все объекты которые также занесены в массив игнорируемых объектов
		void		UpdateWithIgnoreObjects();		
};

//---------------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMonsterMemory class
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------

typedef struct tagDangerousEnemies {
	const CObject	*who;
	TTime			time_till_remember;	

	bool operator == (const tagDangerousEnemies &obj) {return (who == obj.who);}
	bool operator == (const CObject *obj) {return (who == obj);}
} SDangerousEnemies;

// предикат удаления 'старых' объектов
struct predicate_remove_old_dangerous_enemies {
	TTime new_time;
	predicate_remove_old_dangerous_enemies(TTime time) { new_time = time; }
	bool operator() (const SDangerousEnemies &x) { return (x.time_till_remember < new_time); }
};


class CMonsterMemory : public CSoundMemory, public CVisionMemory {
	
	DEFINE_VECTOR(SDangerousEnemies, DANGER_ENEMIES_VEC, DANGER_ENEMIES_VEC_IT);

	DANGER_ENEMIES_VEC	danger_enemies;
public:
	void	InitMemory					(TTime sound_mem, TTime vision_mem) {
		CSoundMemory::Init(sound_mem);
		CVisionMemory::Init(vision_mem);
	}
	void	DeinitMemory				() {
		CSoundMemory::Deinit();
		CVisionMemory::Deinit();
	}

	void	UpdateMemory				();

	//-------------------------------------------------------

	void	AddDangerousEnemy			(const CObject *pO, TTime ttr);
	bool	IsDangerousEnemy			(const CObject *pO);

private:

	void	RemoveOldDangerousEnemies	();
};

