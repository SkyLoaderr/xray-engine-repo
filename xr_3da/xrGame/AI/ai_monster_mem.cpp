#include "stdafx.h"
#include "..\\CustomMonster.h"
#include "ai_monster_mem.h"
#include "..\\actor.h"

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
	// ����� � ������� �����
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

void CSoundMemory::GetMostDangerousSound(SoundElem &s, bool &bDangerous)
{
	if (!IsRememberSound()) return;

	// ������� ������ ��������
	s = Sounds.front();

	if (s.type > WEAPON_EMPTY_CLICKING) bDangerous = false;
	else bDangerous = true;
}

void CSoundMemory::UpdateHearing(TTime dt)
{
	CurrentTime = dt;

	// �������� ���������� ������
	for (u32 i = 0; i < Sounds.size(); i++) {
		if ((TTime)Sounds[i].time + MemoryTime < CurrentTime) {
			Sounds[i] = Sounds.back();
			Sounds.pop_back();
		}
	}

	// ������������� �� "���������" �����
	std::sort(Sounds.begin(),Sounds.end());
}


//---------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVisionMemory implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------

void CVisionMemory::Init(TTime mem_time) 
{
	MemoryTimeDefault	= MemoryTime = mem_time;
	Selected.obj		= 0;
	
	pMonster = dynamic_cast<CCustomMonster *>(this);
	if (!pMonster) R_ASSERT("Cannot dynamic_cast from CVisionMemory to CCustomMonster!");
}
void CVisionMemory::Deinit() 
{
	Objects.clear(); 
	Enemies.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ��������� ������� ����������, ���������� � ������������� ������� ��������
// ��������� ������� Objects � Enemies
void CVisionMemory::UpdateVision(TTime dt) 
{
	CurrentTime	= dt;

	// �������� ������ ������� ��������
	xr_vector<CObject*> Visible_Objects;
	xr_vector<CObject*>::iterator I, E;
	pMonster->feel_vision_get(Visible_Objects);

	I = Visible_Objects.begin();
	E = Visible_Objects.end();

	VisionElem ve;

	for (; I!=E; I++) {

		CEntityAlive *pE = dynamic_cast<CEntityAlive *>(*I);
		if (!pE) R_ASSERT("Visible object is not of class CEntityAlive. Check feel_vision_isRelevant!");

		ve.Set(pE,CurrentTime);

		if (!pE->g_Alive()) AddObject(ve);
		else AddEnemy(ve);
	}

	// ������� ������ �������
	for (u32 i = 0; i<Objects.size(); i++){
		if (Objects[i].time < CurrentTime - MemoryTime){
			Objects[i] = Objects.back();
			Objects.pop_back();
		}
	}
	// ������� ������ ������ � ���, ���������� �� ������� > 30�
	for (i = 0; i<Enemies.size(); i++){
		if ((Enemies[i].time < CurrentTime - MemoryTime) || (!Enemies[i].obj->g_Alive()) || 
			((Enemies[i].obj->Position().distance_to(pMonster->Position()) > 30) && (Enemies[i].time != CurrentTime))){
			Enemies[i] = Enemies.back();
			Enemies.pop_back();
		}
	}

	// �������� Selected
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
		// ���� ��� �� �����?
		if (!Selected.obj->g_Alive()) Selected.obj = 0;

		// ��������� ���� ����� ����� �� ���� ������?
		if (Selected.time + MemoryTime < CurrentTime) Selected.obj = 0;

		// ������ ����� ���������� �����
		if (Selected.time + TIME_TO_RESELECT_ENEMY < CurrentTime) Selected.obj = 0;
	}

	// ���������� ������� ������� �����?
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


// ������� ��������� ������ � ������� pos
// ������ Enemies/Objects (� ����������� �� obj_type) �� ������ ���� ������!
VisionElem &CVisionMemory::GetNearestObject(EObjectType obj_type)
{
	float		optimal_val;
	float		cur_val;
	int			index = 0;		// ������ ���������� ������� � ������� �������� 

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

