#include "stdafx.h"
#include "../CustomMonster.h"
#include "ai_monster_mem.h"
#include "../actor.h"

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
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_ITEM_TAKING,			WEAPON_TAKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_ITEM_HIDING,			WEAPON_HIDING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WEAPON_EMPTY_CLICKING,	WEAPON_EMPTY_CLICKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WEAPON_BULLET_HIT,	WEAPON_BULLET_RICOCHET);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_DYING,		MONSTER_DYING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_INJURING,	MONSTER_INJURING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_STEP,		MONSTER_WALKING);
	
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_TALKING,		MONSTER_TALKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_ATTACKING,	MONSTER_ATTACKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WORLD_OBJECT_BREAKING,	OBJECT_BREAKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WORLD_OBJECT_COLLIDING,	OBJECT_FALLING);


	return NONE_DANGEROUS_SOUND;
}


void CSoundMemory::HearSound(const SoundElem &s)
{
	if (MONSTER_DYING			== s.type)				return;		// todo
	if (NONE_DANGEROUS_SOUND	== s.type)				return;		// todo
	if (WEAPON_BULLET_RICOCHET	== s.type)				return;		// todo
	if (MONSTER_INJURING		== s.type)				return;		// todo
	
	// не регистрировать звуки, у которых владелец не известен
	if (!s.who) return;									// todo
	
	// не регистрировать звуки, у которых владелец - труп // todo
	const CEntityAlive* E = dynamic_cast<const CEntityAlive*> (s.who);
	if (E) if (!E->g_Alive()) return;

	// поиск в массиве звука
	xr_vector<SoundElem>::iterator it;
	
	for (it = Sounds.begin(); Sounds.end() != it; ++it) {
		if ((s.who == it->who) && (it->type ==	s.type)) {
			if (s.time > it->time) *it = s;
			return;
		}
	}

	Sounds.push_back(s);

	// отсортировать по "опасности" звука
	std::sort(Sounds.begin(),Sounds.end());
}

void CSoundMemory::HearSound(const CObject* who, int eType, const Fvector &Position, float power, TTime time)
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

	// удалить звуки, от муртвых объектов
}

void CSoundMemory::CheckValidObjects()
{
	xr_vector<SoundElem>::iterator Result = std::remove_if(Sounds.begin(), Sounds.end(), remove_offline_sound_pred());
	Sounds.erase   (Result,Sounds.end());
}

void CSoundMemory::RemoveSoundOwner(const CObject *pO)
{
	xr_vector<SoundElem>::iterator Result = std::remove_if(Sounds.begin(), Sounds.end(), remove_sound_owner_pred(pO));
	Sounds.erase   (Result,Sounds.end());
}

void CSoundMemory::RemoveDeadObjects()
{
	xr_vector<SoundElem>::iterator Result = std::remove_if(Sounds.begin(), Sounds.end(), remove_dead_objects_pred());
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

	
	string128 name;
	strcpy(name,pMonster->cName());


	// получить список видимых объектов
	VisionElem ve;
	{
		xr_set<const CEntityAlive *>::const_iterator	I = pMonster->enemies().begin();
		xr_set<const CEntityAlive *>::const_iterator	E = pMonster->enemies().end();
		for ( ; I != E; ++I) {
			ve.Set	(*I,timeCurrent);
			AddEnemy(ve);
		}
	}

	{
		xr_set<const CGameObject *>::const_iterator	I = pMonster->CItemManager::items().begin();
		xr_set<const CGameObject *>::const_iterator	E = pMonster->CItemManager::items().end();
		for ( ; I != E; ++I) {
			ve.Set	(dynamic_cast<const CEntity*>(*I),timeCurrent);
			AddObject(ve);
		}
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
	if (Objects.end() == res) Objects.push_back(ve);
	else *res = ve;

}

void CVisionMemory::AddEnemy(const VisionElem &ve)
{
	ITERATOR_VE res;

	res = std::find(Enemies.begin(), Enemies.end(), ve);
	if (Enemies.end() == res) Enemies.push_back(ve);
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
	if (ENEMY == obj_type) ObjectsVector = &Enemies;
	else ObjectsVector = &Objects;
	
	optimal_val = 1000 * pMonster->Position().distance_to((*ObjectsVector)[index].position) * (timeCurrent - (*ObjectsVector)[index].time + 1);

	for (u32 i=1; i < ObjectsVector->size(); ++i) {
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
	I = remove_if(Enemies.begin(), Enemies.end(), predicate_remove_old_enemies(timeCurrent,timeMemory,pMonster->Position(), pMonster->eye_range));
	Enemies.erase(I,Enemies.end());
}

void CVisionMemory::CheckValidObjects()
{
	ITERATOR_VE Result = remove_if(Enemies.begin(), Enemies.end(), predicate_remove_offline());
	Enemies.erase   (Result,Enemies.end());

	Result = remove_if(Objects.begin(), Objects.end(), predicate_remove_offline());
	Objects.erase   (Result,Objects.end());
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
	for (ITERATOR_VE IT = IgnoreObjects.begin(); IgnoreObjects.end() != IT; ++IT)	{
		ITERATOR_VE I = remove_if(Objects.begin(), Objects.end(), predicate_remove_ignore_objects(IT->obj));
		Objects.erase(I,Objects.end());

		I = remove_if(Enemies.begin(), Enemies.end(), predicate_remove_ignore_objects(IT->obj));
		Enemies.erase(I,Enemies.end());
	}
}

void CVisionMemory::AddIgnoreObject(const CEntity *pObj)
{
	VisionElem ve;
	ve.Set(pObj, timeCurrent);
	
	ITERATOR_VE res;
	res = std::find(IgnoreObjects.begin(), IgnoreObjects.end(), ve);
	if (IgnoreObjects.end() == res) IgnoreObjects.push_back(ve);
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

	RemoveOldDangerousEnemies();

	// AddVisualObjects if there's sounds from them
	for (u32 i=0;i<Sounds.size();i++) {
		if (Sounds[i].who && (Sounds[i].time == curtime)) {
			for (u32 k = 0; k<Enemies.size(); k++) {
				if ((Sounds[i].who->ID() == Enemies[k].obj->ID()) && (Enemies[k].time != curtime)) {
					Enemies[k].time = curtime; 
					LOG_EX("Vision memory updated with sound memory!");
				}
			}
		}
	}

}

void CMonsterMemory::AddDangerousEnemy(const CObject *pO, TTime ttr)
{
	DANGER_ENEMIES_VEC_IT res;
	SDangerousEnemies new_elem;

	res = std::find(danger_enemies.begin(), danger_enemies.end(), pO);
	if (res == danger_enemies.end()) {
		new_elem.who = pO;
		new_elem.time_till_remember = Level().timeServer() + ttr;	
		danger_enemies.push_back(new_elem);
	} else {
		if (res->time_till_remember < Level().timeServer() + ttr) 
			res->time_till_remember = Level().timeServer() + ttr;
	}
}

bool CMonsterMemory::IsDangerousEnemy(const CObject *pO)
{
	DANGER_ENEMIES_VEC_IT res;

	res = std::find(danger_enemies.begin(), danger_enemies.end(), pO);
	if (res == danger_enemies.end()) return false;
	else return true;
}

void CMonsterMemory::RemoveOldDangerousEnemies()
{
	// удалить 'старых' врагов 
	DANGER_ENEMIES_VEC_IT I = remove_if(danger_enemies.begin(), danger_enemies.end(), predicate_remove_old_dangerous_enemies(Level().timeServer()));
	danger_enemies.erase(I,danger_enemies.end());
}



