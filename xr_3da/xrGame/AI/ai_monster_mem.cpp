#include "stdafx.h"
#include "..\\CustomMonster.h"
#include "ai_monster_mem.h"
#include "..\\actor.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSoundMemory implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CHECK_SOUND_TYPE(a,b,c) { if ((a & b) == b) return c; }

TSoundDangerValue tagSoundElement::ConvertSoundType(ESoundTypes stype)
{
	
	if (((stype & SOUND_TYPE_WEAPON) != SOUND_TYPE_WEAPON) && 
		((stype & SOUND_TYPE_MONSTER) != SOUND_TYPE_MONSTER) && 
		((stype & SOUND_TYPE_WORLD) != SOUND_TYPE_WORLD)) return NONE_DANGEROUS_SOUND;

	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WEAPON_RECHARGING,	WEAPON_RECHARGING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WEAPON_SHOOTING,		WEAPON_SHOOTING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WEAPON_TAKING,		WEAPON_TAKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WEAPON_HIDING,		WEAPON_HIDING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WEAPON_CHANGING,		WEAPON_CHANGING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WEAPON_EMPTY_CLICKING,	WEAPON_EMPTY_CLICKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WEAPON_BULLET_RICOCHET,	WEAPON_BULLET_RICOCHET);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_DYING,		MONSTER_DYING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_INJURING,	MONSTER_INJURING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_WALKING,		MONSTER_WALKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_JUMPING,		MONSTER_JUMPING);

	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_FALLING,		MONSTER_FALLING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_TALKING,		MONSTER_TALKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_ATTACKING,	MONSTER_ATTACKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WORLD_DOOR_OPENING,	DOOR_OPENING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WORLD_DOOR_CLOSING,	DOOR_CLOSING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WORLD_OBJECT_BREAKING,	OBJECT_BREAKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WORLD_OBJECT_FALLING,	OBJECT_FALLING);


	return NONE_DANGEROUS_SOUND;
}


void CSoundMemory::HearSound(const SoundElem &s)
{
	if (s.type == SOUND_TYPE_MONSTER_DYING) return;		// todo
	if (s.type == NONE_DANGEROUS_SOUND) return;			// todo
	if (s.type == WEAPON_BULLET_RICOCHET) return;		// todo
	
	// не регистрировать звуки, у которых владелец не известен
	if (!s.who) return;									// todo

	// поиск в массиве звука
	xr_vector<SoundElem>::iterator it;
	
	for (it = Sounds.begin(); it != Sounds.end(); it++) {
		if ((it->who == s.who) && (it->type ==	s.type)) {
			if (s.time > it->time) *it = s;
			return;
		}
	}

	Sounds.push_back(s);

	// отсортировать по "опасности" звука
	std::sort(Sounds.begin(),Sounds.end());
}

void CSoundMemory::HearSound(CObject* who, int eType, const Fvector &Position, float power, TTime time)
{
	SoundElem s;
	s.SetConvert(who,eType,Position,power,time);

	HearSound(s);
}

void CSoundMemory::GetSound(SoundElem &s, bool &bDangerous)
{
	if (!IsRememberSound()) return;

	// возврат самого опасного
	s = Sounds.front();

	if (s.type > WEAPON_EMPTY_CLICKING) bDangerous = false;
	else bDangerous = true;
}

void CSoundMemory::UpdateHearing(TTime dt)
{
	timeCurrent = dt;

	// удаление устаревших звуков
	xr_vector<SoundElem>::iterator I = remove_if(Sounds.begin(), Sounds.end(), predicate_remove_old_sounds(timeCurrent - timeMemory));
	Sounds.erase(I,Sounds.end());

	// удалить объекты, которые ушли в оффлайн
	CheckValidObjects();
}

void CSoundMemory::CheckValidObjects()
{
	xr_vector<SoundElem>::iterator Result = std::remove_if(Sounds.begin(), Sounds.end(), remove_offline_sound_pred());
	Sounds.erase   (Result,Sounds.end());
}

//---------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVisionMemory implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------

void CVisionMemory::Init(TTime mem_time) 
{
	Deinit();

	timeMemoryDefault	= timeMemory = mem_time;
	timeLastUpdateIgnoreObjects = 0;

	Selected.obj		= 0;

	pMonster = dynamic_cast<CCustomMonster *>(this);
	if (!pMonster) R_ASSERT("Cannot dynamic_cast from CVisionMemory to CCustomMonster!");
}
void CVisionMemory::Deinit() 
{
	Objects.clear	(); 
	Enemies.clear	();
	Selected.obj	= 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Обработка видимой информации, обновление и классификация видимых объектов
// Заполняет массивы Objects и Enemies
void CVisionMemory::UpdateVision(TTime dt) 
{
	timeCurrent	= dt;

	// получить список видимых объектов
	VisionElem ve;
	objVisible &VisibleEnemies = Level().Teams[pMonster->g_Team()].Squads[pMonster->g_Squad()].KnownEnemys;

	for (int i=0, n=VisibleEnemies.size(); i<n; i++) {
		CEntityAlive *pE = dynamic_cast<CEntityAlive *>(VisibleEnemies[i].key);
		if (!pE) R_ASSERT("Visible object is not of class CEntityAlive. Check feel_vision_isRelevant!");

		ve.Set(pE,timeCurrent);
 
		if (!pE->g_Alive()) AddObject(ve);
		else AddEnemy(ve);
	}

	// удаление объектов, перешедших в оффлайн
	CheckValidObjects();

	// удалить 'старые' объекты и враги
	RemoveOldElems();

	// Удалить старые игнор-объекты
	RemoveOldIgnoreObjects();
	// удалить все объекты которые также занесены в массив игнорируемых объектов
	UpdateWithIgnoreObjects();
	
	// обновить Selected
	if (IsEnemy()) SelectEnemy();
	else if (IsObject()) SelectCorpse();
	else Selected.obj = 0;
	
}
 
void CVisionMemory::AddObject(const VisionElem &ve)
{
	ITERATOR_VE res;

	res = std::find(Objects.begin(), Objects.end(), ve);
	if (res == Objects.end()) Objects.push_back(ve);
	else *res = ve;

}

void CVisionMemory::AddEnemy(const VisionElem &ve)
{
	ITERATOR_VE res;

	res = std::find(Enemies.begin(), Enemies.end(), ve);
	if (res == Enemies.end()) Enemies.push_back(ve);
	else *res = ve;

	if (ve.obj == Selected.obj) Selected = ve;
}


void CVisionMemory::SelectEnemy()
{
	if (Selected.obj) {
		// враг уже не живой?
		if (!Selected.obj->g_Alive()) Selected.obj = 0;

		// Выбранный враг давно исчез из поля зрения?
		if (Selected.time + timeMemory < timeCurrent) Selected.obj = 0;

		// пришло время перевыбора врага
		if (Selected.time + TIME_TO_RESELECT_ENEMY < timeCurrent) Selected.obj = 0;
	}

	// Необходимо выбрать другого врага?
	if (!Selected.obj) {
		Selected = GetNearestObject(ENEMY);
		Selected.time = timeCurrent;
	}
}

void CVisionMemory::SelectCorpse()
{
	Selected = GetNearestObject(OBJECT);
}

bool CVisionMemory::Get(VisionElem &ve)
{
	if (!Selected.obj) return false;
	else ve = Selected;
	return true;
}


// Выбрать ближайший объект к позиции pos
// массив Enemies/Objects (в зависимости от obj_type) не должен быть пустым!
VisionElem &CVisionMemory::GetNearestObject(EObjectType obj_type)
{
	float		optimal_val;
	float		cur_val;
	int			index = 0;		// Индекс ближайшего объекта в массиве объектов 

	VECTOR_VE *ObjectsVector = 0;
	if (obj_type == ENEMY) ObjectsVector = &Enemies;
	else ObjectsVector = &Objects;
	
	optimal_val = 1000 * pMonster->Position().distance_to((*ObjectsVector)[index].position) * (timeCurrent - (*ObjectsVector)[index].time + 1);

	for (u32 i=1; i < ObjectsVector->size(); i++) {
		cur_val = 1000 * pMonster->Position().distance_to((*ObjectsVector)[i].position) * (timeCurrent - (*ObjectsVector)[i].time + 1);

		if ( cur_val < optimal_val){
			optimal_val = cur_val;
			index = i;
		}
	}
	return (*ObjectsVector)[index];
}

void CVisionMemory::RemoveOldElems()
{
	// удалить 'старые' объекты
	ITERATOR_VE I = remove_if(Objects.begin(), Objects.end(), predicate_remove_old_objects(timeCurrent - timeMemory));
	Objects.erase(I,Objects.end());
	
	// удалить 'старых' врагов и тех, расстояние до которых > 30м и др.
	I = remove_if(Enemies.begin(), Enemies.end(), predicate_remove_old_enemies(timeCurrent,timeMemory,pMonster->Position()));
	Enemies.erase(I,Enemies.end());
}

void CVisionMemory::CheckValidObjects()
{
	ITERATOR_VE Result = remove_if(Enemies.begin(), Enemies.end(), predicate_remove_offline());
	Enemies.erase   (Result,Enemies.end());
}

void CVisionMemory::RemoveOldIgnoreObjects()
{
	if ((timeLastUpdateIgnoreObjects + TIME_TO_UPDATE_IGNORE_OBJECTS) > timeCurrent) return;

	// удалить 'старые' объекты
	ITERATOR_VE I = remove_if(IgnoreObjects.begin(), IgnoreObjects.end(), predicate_remove_old_objects(timeCurrent - timeMemory));
	IgnoreObjects.erase(I,IgnoreObjects.end());

	timeLastUpdateIgnoreObjects = timeCurrent;
}

void CVisionMemory::UpdateWithIgnoreObjects()
{
	for (ITERATOR_VE IT = IgnoreObjects.begin(); IT !=  IgnoreObjects.end(); IT++)	{
		ITERATOR_VE I = remove_if(Objects.begin(), Objects.end(), predicate_remove_ignore_objects(IT->obj));
		Objects.erase(I,Objects.end());

		I = remove_if(Enemies.begin(), Enemies.end(), predicate_remove_ignore_objects(IT->obj));
		Enemies.erase(I,Enemies.end());
	}
}

void CVisionMemory::AddIgnoreObject(CEntity *pObj)
{
	VisionElem ve;
	ve.Set(pObj, timeCurrent);
	
	ITERATOR_VE res;
	res = std::find(IgnoreObjects.begin(), IgnoreObjects.end(), ve);
	if (res == IgnoreObjects.end()) IgnoreObjects.push_back(ve);
	else *res = ve;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMonsterMemory implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMonsterMemory::UpdateMemory()
{
	TTime curtime = Level().timeServer();

	UpdateVision(curtime);
	UpdateHearing(curtime);

}

