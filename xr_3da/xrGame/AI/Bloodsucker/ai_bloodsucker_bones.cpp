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
	dy = dt * axis[index].r_speed;
	
	if (_abs(axis[index].target_yaw - axis[index].cur_yaw) < dy) axis[index].cur_yaw = axis[index].target_yaw;
	else axis[index].cur_yaw += ((axis[index].target_yaw > axis[index].cur_yaw) ? dy : -dy);
}


//****************************************************************************************************
// class bonesMotion
//****************************************************************************************************

void bonesMotion::AddBone(CBoneInstance *bone, u32 axis_used, float target_yaw_1, float r_speed_1, float target_yaw_2, float r_speed_2, float target_yaw_3, float r_speed_3, u32 time)
{
	bonesBone b;

	b.bone = bone;
	b.axis_used = axis_used;

	b.axis[0].target_yaw = target_yaw_1;
	b.axis[0].r_speed = r_speed_1;
	b.axis[1].target_yaw = target_yaw_2;
	b.axis[1].r_speed = r_speed_2;
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
}

void bonesManipulation::SetReturnParams(bonesMotion m)
{
	
}

void bonesManipulation::Update()
{
	bonesMotion curMotion;

//	if (motions.empty()) curMotion = return_params;			// движение по Return Params
//	else curMotion = *cur_motion;
	
	// провести обработку всех костей
	bonesBone bone;
	bool bones_were_turned = false;
	u32 dt = 0;

	for (u32 i=0; i<curMotion.bones.size(); i++) {
	
		bone = curMotion.bones[i];

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

	if (growth && !bones_were_turned) {
		xr_vector<bonesMotion>::iterator	is_last_elem;
		is_last_elem = cur_motion;
		is_last_elem++;
		if (is_last_elem == motions.end()) {
			growth = true;
		}
	}
}





//// code
//MBoneManip		BoneManip;
//MMotionBones	MotionBones;
//xr_vector<MMotionBones> bones_vector;
//
//// смотреть влево
//// будут вращаться 3 боны
//MotionBones.AddBone(BoneInstance, ..);
//MotionBones.AddBone(BoneInstance, ..);
//MotionBones.AddBone(BoneInstance, ..);
//
//BoneManip.AddMotion(MotionBones);
//
//// смотреть вправо
//// будут вращаться 3 боны
//MotionBones.AddBone(BoneInstance, ..);
//MotionBones.AddBone(BoneInstance, ..);
//MotionBones.AddBone(BoneInstance, ..);
//
//BoneManip.AddMotion(MotionBones);
//BoneManip.SetReturnParams();
//
//
//


