#include "stdafx.h"
#include "ai_bloodsucker_bones.h"

//  temp
#include "..\\..\\hudmanager.h"

//****************************************************************************************************
// class bonesBone
//****************************************************************************************************
bool bonesBone::NeedTurn(u32 p_axis)
{
	int index;
	
	if (p_axis == AXIS_X) index = 0; 
	else if (p_axis == AXIS_Y) index = 1; 
	else index = 2; 

	if ((axis_used & p_axis) == p_axis) {
		if (!fsimilar(axis[index].cur_yaw, axis[index].target_yaw, EPS_L)) return true;
	}

	return false;
}

void bonesBone::Turn(u32 dt, u32 p_axis)
{
	int index;

	if (p_axis == AXIS_X) index = 0; 
	else if (p_axis == AXIS_Y) index = 1; 
	else index = 2; 

	float dy;
	dy =  axis[index].r_speed * dt / 1000;  // учитываем милисек и радианную меры
	
	if (_abs(axis[index].target_yaw - axis[index].cur_yaw) < dy) axis[index].cur_yaw = axis[index].target_yaw;
	else axis[index].cur_yaw += ((axis[index].target_yaw > axis[index].cur_yaw) ? dy : -dy);

	//Msg("cur_yaw = [%f] target_yaw = [%f]", axis[index].cur_yaw, axis[index].target_yaw);
}

void bonesBone::Apply()
{
	float x = 0.f, y = 0.f, z = 0.f;
	
	if ((axis_used & AXIS_X) == AXIS_X) x = axis[0].cur_yaw;
	if ((axis_used & AXIS_Y) == AXIS_Y) y = axis[1].cur_yaw;
	if ((axis_used & AXIS_Z) == AXIS_Z) z = axis[2].cur_yaw;

	HUD().pFontSmall->OutSet (300,450);
	HUD().pFontSmall->OutNext("BoneX : CY = [%f] ", x);

	// создать матрицу вращения и умножить на mTransform боны
	Fmatrix M;
	M.setXYZi (x, y, z);
	bone->mTransform.mulB(M);
}

void bonesBone::Setup (CBoneInstance *b, u32 a)
{
	bone		= b; 
	axis_used	= a;

	for (int i=0; i<3; i++) {
		axis[i].cur_yaw		= 0.f; 
		axis[i].target_yaw	= 0.f; 
		axis[i].r_speed		= 0.f;
	}
}


//****************************************************************************************************
// class bonesMotion
//****************************************************************************************************

void bonesMotion::AddBone(CBoneInstance *bone, u32 axis_used, float target_yaw_1, float r_speed_1, float target_yaw_2, float r_speed_2, float target_yaw_3, float r_speed_3, u32 time)
{
	bonesBone b;

	b.bone = bone;
	b.axis_used = axis_used;

	b.axis[0].cur_yaw = 0.f;
	b.axis[0].target_yaw = target_yaw_1;
	b.axis[0].r_speed = r_speed_1;
	b.axis[1].cur_yaw = 0.f;
	b.axis[1].target_yaw = target_yaw_2;
	b.axis[1].r_speed = r_speed_2;
	b.axis[2].cur_yaw = 0.f;
	b.axis[2].target_yaw = target_yaw_3;
	b.axis[2].r_speed = r_speed_3;

	this->time = time;

	bones.push_back(b);
}

//****************************************************************************************************
// class bonesManipulation
//****************************************************************************************************

void bonesManipulation::AddMotion(bonesMotion m)
{
	motions.push_back(m);
	cur_motion = motions.begin();
}

void bonesManipulation::Init()
{
	motions.clear		();
	cur_motion			= motions.end();
	time_started		= 0;
	time_last_update	= 0;
	in_return_state		= false;
}


void bonesManipulation::Update(u32 time)
{
	// todo если ничего не произошло - выход

	if (time == time_last_update) return;

	// провести обработку всех костей
	bool bones_were_turned = false;
	u32 dt = time - time_last_update;
	time_last_update = time;

	for (u32 i=0; i<m_Bones.size(); i++) {

		if (m_Bones[i].NeedTurn(AXIS_X)){
			m_Bones[i].Turn(dt,AXIS_X);
			bones_were_turned = true;
		}
		if (m_Bones[i].NeedTurn(AXIS_Y)) {
			m_Bones[i].Turn(dt,AXIS_Y);
			bones_were_turned = true;
		}
		if (m_Bones[i].NeedTurn(AXIS_Z)) {
			m_Bones[i].Turn(dt,AXIS_Z);
			bones_were_turned = true;
		}

//		HUD().pFontSmall->OutSet (300,400);
//		HUD().pFontSmall->OutNext("BoneX : CY = [%f]  TY = [%f] RS = [%f]", m_Bones[i].axis[0].cur_yaw, m_Bones[i].axis[0].target_yaw, m_Bones[i].axis[0].r_speed);
//
//		HUD().pFontSmall->OutSet (300,500);
//		HUD().pFontSmall->OutNext("DayTime = [%i]    time = [%i]", Level().GetDayTime(), Level().GetGameTime());


	}
	
	// если добавлены ещё элементы во время возврата
	if (in_return_state && !motions.empty()) {
		in_return_state = false;
		cur_motion = motions.begin();
		// устанавливаем новые таргеты и угловые скорости
		for (u32 i = 0; i<m_Bones.size(); i++) {
			for (int index = 0; index < 3; index++) {
				m_Bones[i].axis[index].target_yaw	= cur_motion->bones[i].axis[index].target_yaw;
				m_Bones[i].axis[index].r_speed		= cur_motion->bones[i].axis[index].r_speed;
			}
		}
	}
	
	// если процесс возврата завершен
	if (!bones_were_turned && motions.empty() && in_return_state) {
		Init();
		in_return_state = false;
		return;
	}
	
	// если выполняется наращивание угла и ни одна кость не повернулась (достигли таргета...)
	if (!bones_were_turned && !motions.empty() && !in_return_state) {
		if ((time_started == 0) && (cur_motion->time > 0)) { // начинаем ждать
			time_started = time;
		}

		if ((time_started != 0) && (time_started + cur_motion->time < time)) { // время вышло?
			time_started = 0;

			// проверить является ли данный элемент последним
			xr_vector<bonesMotion>::iterator	is_last_elem;
			is_last_elem = cur_motion;
			is_last_elem++;

			if (is_last_elem == motions.end()) { // последний
				// удаляем все и начинаем возврат
				in_return_state	= true;
				motions.clear();
				// установить у всех костей в m_Bone таргеты в 0
				for (u32 i = 0; i<m_Bones.size(); i++) {
					m_Bones[i].axis[0].target_yaw = 0.f;
					m_Bones[i].axis[1].target_yaw = 0.f;
					m_Bones[i].axis[2].target_yaw = 0.f;
				}
			} else { // если не последний
				// переключаем на следующий элемент
				cur_motion++;
				
				// устанавливаем новые таргеты и угловые скорости
				for (u32 i = 0; i<m_Bones.size(); i++) {
					for (int index = 0; index < 3; index++) {
						m_Bones[i].axis[index].target_yaw	= cur_motion->bones[i].axis[index].target_yaw;
						m_Bones[i].axis[index].r_speed		= cur_motion->bones[i].axis[index].r_speed;
					}
				}

			}
		}
	}

	// Установить параметры из m_Bones
	for (u32 i = 0; i<m_Bones.size(); i++) {
		m_Bones[i].Apply();
	}
}

