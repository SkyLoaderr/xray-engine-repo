#include "stdafx.h"
#include "ai_bloodsucker_bones.h"

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
	dy = deg2rad(dt * axis[index].r_speed / 1000);  // учитываем милисек и радианную меры
	
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

	// создать матрицу вращения и умножить на mTransform боны
	Fmatrix M;
	M.setXYZi (x, y, z);
	bone->mTransform.mulB(M);
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
	growth				= true;
}


void bonesManipulation::Update(u32 time)
{
	if (motions.empty()) return;
	
	// провести обработку всех костей
	bool bones_were_turned = false;
	u32 dt = time - time_last_update;
	time_last_update = time;

	for (u32 i=0; i<cur_motion->bones.size(); i++) {
	
		bonesBone &bone = cur_motion->bones[i];

		if (bone.NeedTurn(AXIS_X)){
			bone.Turn(dt,AXIS_X);
			bones_were_turned = true;
		}
		if (bone.NeedTurn(AXIS_Y)) {
			bone.Turn(dt,AXIS_Y);
			bones_were_turned = true;
		}

		if (bone.NeedTurn(AXIS_Z)) {
			bone.Turn(dt,AXIS_Z);
			bones_were_turned = true;
		}
	}
	
	// если выполняется наращивание угла и ни одна кость не повернулась (достигли таргета...)
	if (growth && !bones_were_turned) {
		// 
		if ((time_started == 0) && (cur_motion->time > 0)) { // начинаем ждать
			time_started = time;
		}

		if ((time_started != 0) && (time_started + cur_motion->time > time)) { // время вышло?
			time_started = 0;
			// проверить является ли данный элемент последним
			xr_vector<bonesMotion>::iterator	is_last_elem;
			is_last_elem = cur_motion;
			is_last_elem++;
			
			if (is_last_elem == motions.end()) { // последний
				// удаляем все кроме первого и начинаем возврат
				growth	= false;
				cur_motion = motions.begin();
				motions.erase(cur_motion + 1, motions.end());
				// установить у всех костей таргеты в 0
				for (u32 i = 0; i<cur_motion->bones.size(); i++) {
					cur_motion->bones[i].axis[0].target_yaw = 0.f;
					cur_motion->bones[i].axis[1].target_yaw = 0.f;
					cur_motion->bones[i].axis[2].target_yaw = 0.f;
				}
			} else {
				// переключаем на следующий элемент
				cur_motion++;
			}
		} 
	} 

	// если выполняется возврат и ни одна кость не повернулась (всё в 0)
	if (!growth && !bones_were_turned) {
		Init();
		return;
	}

	// Установить параметры из cur_motion
	for (u32 i = 0; i<cur_motion->bones.size(); i++) {
		cur_motion->bones[i].Apply();
	}
}





