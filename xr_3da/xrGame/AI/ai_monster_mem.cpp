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
}

void CSoundMemory::HearSound(CObject* who, int eType, const Fvector &Position, float power, TTime time)
{
	SoundElem s;
	s.SetConvert(who,eType,Position,power,time);

	HearSound(s);
}

bool CSoundMemory::IsRememberSound()
{
	if (!Sounds.empty()) return true;
	return false;
}

void CSoundMemory::GetMostDangerousSound(SoundElem &s, bool &bDangerous)
{
	if (!IsRememberSound()) return;

	// возврат самого опасного
	s = Sounds.front();

	if (s.type > WEAPON_EMPTY_CLICKING) bDangerous = false;
	else bDangerous = true;
}

void CSoundMemory::UpdateHearing(TTime dt)
{
	CurrentTime = dt;

	// удаление устаревших звуков
	for (u32 i = 0; i < Sounds.size(); i++) {
		if ((TTime)Sounds[i].time + MemoryTime < CurrentTime) {
			Sounds[i] = Sounds.back();
			Sounds.pop_back();
		}
	}

	// отсортировать по "опасности" звука
	std::sort(Sounds.begin(),Sounds.end());
}


//---------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVisionMemory implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------

void CVisionMemory::Init(TTime mem_time) 
{
	Deinit();

	MemoryTimeDefault	= MemoryTime = mem_time;
	Selected.obj		= 0;
	
	pMonster = dynamic_cast<CCustomMonster *>(this);
	if (!pMonster) R_ASSERT("Cannot dynamic_cast from CVisionMemory to CCustomMonster!");
}
void CVisionMemory::Deinit() 
{
	Objects.clear	(); 
	Enemies.clear	();
	Saved.obj		= 0;
	Selected.obj	= 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Обработка видимой информации, обновление и классификация видимых объектов
// Заполняет массивы Objects и Enemies
void CVisionMemory::UpdateVision(TTime dt) 
{
	CurrentTime	= dt;

	// получить список видимых объектов
	VisionElem ve;
	objVisible &VisibleEnemies = Level().Teams[pMonster->g_Team()].Squads[pMonster->g_Squad()].KnownEnemys;

	for (int i=0, n=VisibleEnemies.size(); i<n; i++) {
		CEntityAlive *pE = dynamic_cast<CEntityAlive *>(VisibleEnemies[i].key);
		if (!pE) R_ASSERT("Visible object is not of class CEntityAlive. Check feel_vision_isRelevant!");

		ve.Set(pE,CurrentTime);
 
		if (!pE->g_Alive()) AddObject(ve);
		else AddEnemy(ve);
	}

	// удалить старые объекты
	for (u32 i = 0; i<Objects.size(); i++){
		if (Objects[i].time < CurrentTime - MemoryTime){
			Objects[i] = Objects.back();
			Objects.pop_back();
		}
	}
	// удалить старых врагов и тех, расстояние до которых > 30м
	for (i = 0; i<Enemies.size(); i++){
		if ((Enemies[i].time < CurrentTime - MemoryTime) || (!Enemies[i].obj->g_Alive()) || 
			((Enemies[i].obj->Position().distance_to(pMonster->Position()) > 30) && (Enemies[i].time != CurrentTime))){
			Enemies[i] = Enemies.back();
			Enemies.pop_back();
		}
	}

	// обновить Selected
	if (IsEnemy()) SelectEnemy();
	else if (IsObject()) SelectCorpse();
	else Selected.obj = 0;
}
 

void CVisionMemory::AddObject(const VisionElem &ve)
{
	xr_vector<VisionElem>::iterator res;

	res = std::find(Objects.begin(), Objects.end(), ve);
	if (res == Objects.end()) Objects.push_back(ve);
	else *res = ve;

}

void CVisionMemory::AddEnemy(const VisionElem &ve)
{
	xr_vector<VisionElem>::iterator res;

	res = std::find(Enemies.begin(), Enemies.end(), ve);
	if (res == Enemies.end()) Enemies.push_back(ve);
	else *res = ve;

	if (ve.obj == Selected.obj) {
		Selected = ve;
		SaveEnemy();
	}

}


void CVisionMemory::SelectEnemy()
{
	if (Selected.obj) {
		// враг уже не живой?
		if (!Selected.obj->g_Alive()) Selected.obj = 0;

		// Выбранный враг давно исчез из поля зрения?
		if (Selected.time + MemoryTime < CurrentTime) Selected.obj = 0;

		// пришло время перевыбора врага
		if (Selected.time + TIME_TO_RESELECT_ENEMY < CurrentTime) Selected.obj = 0;
	}

	// Необходимо выбрать другого врага?
	if (!Selected.obj) {
		Selected = GetNearestObject();
		Selected.time = CurrentTime;
	}
}

void CVisionMemory::SelectCorpse()
{
	Selected = GetNearestObject(OBJECT);
}

bool CVisionMemory::Get(VisionElem &ve)
{
	if (!Selected.obj) {
		ve = Saved;
		return false;
	} else ve = Selected;
	return true;
}


// Выбрать ближайший объект к позиции pos
// массив Enemies/Objects (в зависимости от obj_type) не должен быть пустым!
VisionElem &CVisionMemory::GetNearestObject(EObjectType obj_type)
{
	float		optimal_val;
	float		cur_val;
	int			index = 0;		// Индекс ближайшего объекта в массиве объектов 

	xr_vector<VisionElem> *ObjectsVector = 0;
	if (obj_type == ENEMY) ObjectsVector = &Enemies;
	else ObjectsVector = &Objects;
	
	optimal_val = 1000 * pMonster->Position().distance_to((*ObjectsVector)[index].position) * (CurrentTime - (*ObjectsVector)[index].time + 1);

	for (u32 i=1; i < ObjectsVector->size(); i++) {
		cur_val = 1000 * pMonster->Position().distance_to((*ObjectsVector)[i].position) * (CurrentTime - (*ObjectsVector)[i].time + 1);

		if ( cur_val < optimal_val){
			optimal_val = cur_val;
			index = i;
		}
	}
	return (*ObjectsVector)[index];
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

