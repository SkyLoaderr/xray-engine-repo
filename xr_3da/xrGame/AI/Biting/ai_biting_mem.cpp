#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_mem.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSoundMemory implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
TSoundDangerValue tagSoundElement::ConvertSoundType(ESoundTypes stype)
{
	TSoundDangerValue tValue;

	switch (stype) {
		case SOUND_TYPE_WEAPON_RECHARGING:		tValue = WEAPON_RECHARGING; break;
		case SOUND_TYPE_WEAPON_SHOOTING:		tValue = WEAPON_SHOOTING; break;	
		case SOUND_TYPE_WEAPON_TAKING:			tValue = WEAPON_TAKING; break;
		case SOUND_TYPE_WEAPON_HIDING:			tValue = WEAPON_HIDING; break;
		case SOUND_TYPE_WEAPON_CHANGING:		tValue = WEAPON_CHANGING; break;
		case SOUND_TYPE_WEAPON_EMPTY_CLICKING:  tValue = WEAPON_EMPTY_CLICKING; break;
		case SOUND_TYPE_WEAPON_BULLET_RICOCHET:	tValue = WEAPON_BULLET_RICOCHET; break;
		case SOUND_TYPE_MONSTER_DYING:			tValue = MONSTER_DYING; break;
		case SOUND_TYPE_MONSTER_INJURING:		tValue = MONSTER_INJURING; break;
		case SOUND_TYPE_MONSTER_WALKING:		tValue = MONSTER_WALKING; break;
		case SOUND_TYPE_MONSTER_JUMPING:		tValue = MONSTER_JUMPING; break;
		case SOUND_TYPE_MONSTER_FALLING:		tValue = MONSTER_FALLING; break;
		case SOUND_TYPE_MONSTER_TALKING:		tValue = MONSTER_TALKING; break;
		case SOUND_TYPE_MONSTER_ATTACKING:		tValue = MONSTER_ATTACKING; break;
		case SOUND_TYPE_WORLD_DOOR_OPENING:		tValue = DOOR_OPENING; break;
		case SOUND_TYPE_WORLD_DOOR_CLOSING:		tValue = DOOR_CLOSING; break;
		case SOUND_TYPE_WORLD_OBJECT_BREAKING:  tValue = OBJECT_BREAKING; break;
		case SOUND_TYPE_WORLD_OBJECT_FALLING:	tValue = OBJECT_FALLING; break;
		default:								tValue = NONE_DANGEROUS_SOUND;
	}

	return tValue;
}


void CSoundMemory::HearSound(const SoundElem &s)
{
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
	s.Set(who,eType,Position,power,time);

	HearSound(s);
}

bool CSoundMemory::IsRememberSound()
{
	if (!Sounds.empty()) return true;
	return false;
}

void CSoundMemory::GetMostDangerousSound(SoundElem &s)
{
	if (!IsRememberSound()) return;

	// удаление устаревших звуков
	for (u32 i = 0; i < Sounds.size(); i++) {
		if ((TTime)Sounds[i].time + MemoryTime > CurrentTime) {
			Sounds[i] = Sounds.back();
			Sounds.pop_back();
		}
	}

	// отсортировать по "опасности" звука
	std::sort(Sounds.begin(),Sounds.end());

	// возврат самого опасного
	s = Sounds.front();
}

void CSoundMemory::UpdateHearing(TTime dt)
{
	CurrentTime = dt;
}

void CSoundMemory::ShowDbgInfo()
{
	Msg("-- Sound Memory:  Show dbg Info ---- ");
	Msg("Current time: [%i];  num of sounds: [%i];  remember sound? [%i]",CurrentTime,Sounds.size(),IsRememberSound());

	SoundElem s;
	GetMostDangerousSound(s);
	if (s.who) Msg("Most_Dangerous_Sound:  Name = [%s] ",s.who->cName());
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVisionMemory implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Обработка видимой информации, обновление и классификация видимых объектов
void CVisionMemory::UpdateVision(TTime dt, CAI_Biting *pThis) 
{
	CurrentTime	= dt;
	
	VisionElem ve;

	xr_vector<feel_visible_Item>::iterator I=pThis->feel_visible.begin(),E=pThis->feel_visible.end();
	for (; I!=E; I++) if (positive(I->fuzzy)) {

		if (I->O->CLS_ID!=CLSID_ENTITY)	continue;

		CEntityAlive *pE = dynamic_cast<CEntityAlive *>(I->O);
		if (!pE) continue;
		

		ve.Set(pE,CurrentTime);

		if (!pE->g_Alive()) {
			if (pE->g_Team() == pThis->g_Team()) continue;	// труп своего
			AddObject(ve);
		} else AddEnemy(ve);
	}

	// удалить старые объекты
	u32 i;
	for (i = 0; i<Objects.size(); i++){
		if (Objects[i].time < CurrentTime - MemoryTime){
			Objects[i] = Objects.back();
			Objects.pop_back();
		}
	}
	for (i = 0; i<Enemies.size(); i++){
 		if ((Enemies[i].time < CurrentTime - MemoryTime) || (!Enemies[i].obj->g_Alive())){
			Enemies[i] = Enemies.back();
			Enemies.pop_back();
		}
	}
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
}

// Выбрать ближайшего врага к позиции pos
void CVisionMemory::GetNearestEnemy(const Fvector &pos, CEntity *e)
{
	VisionElem tE;
	float	   optimal_val = 0.f;
	float	   cur_val;
	
	if (IsEnemy())
		tE = Enemies[0];

	for (u32 i=1; i < Enemies.size(); i++) {
		cur_val = pos.distance_to(Enemies[i].position) * Enemies[i].time;

		if ( cur_val < optimal_val){
			tE = Enemies[i];
			optimal_val = cur_val;
		}
	}
	if (tE.obj)	e = tE.obj;
}

// Выбрать ближайший объект к позиции pos
void CVisionMemory::GetNearestObject(const Fvector &pos, CEntity *e)
{
	VisionElem tE;
	float	   optimal_val = 0.f;
	float	   cur_val;

	if (IsObject())
		tE = Objects[0];

	for (u32 i=1; i <  Objects.size(); i++) {
		cur_val = pos.distance_to(Objects[i].position) * Objects[i].time;

		if (cur_val < optimal_val)	
			tE = Objects[i];
			optimal_val = cur_val;
	}

	e = tE.obj;
}

void CVisionMemory::ShowDbgInfo()
{
	Msg("-- Vision Memory:  Show dbg Info ---- ");
	Msg("Current time: [%i];  num of objects: [%i];  num of enemies [%i]",CurrentTime,Objects.size(),Enemies.size());
	for (u32 i=0; i<Objects.size(); i++) Msg("Visual object %i: Name = [%s], time = [%i]",i,Objects[i].obj->cName(), Objects[i].time);
	for (i=0; i<Enemies.size(); i++) Msg("Visual enemy %i: Name = [%s], time = [%i]",i,Enemies[i].obj->cName(), Enemies[i].time);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMonsterMemory implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMonsterMemory::UpdateMemory()
{
	TTime curtime = Level().timeServer();

	UpdateVision(curtime,pData);
	UpdateHearing(curtime);
}

void CMonsterMemory::ShowDbgInfo()
{
	CSoundMemory::ShowDbgInfo();
	CVisionMemory::ShowDbgInfo();
}
