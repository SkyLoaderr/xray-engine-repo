#include "stdafx.h"
#include "ai_bloodsucker_bones.h"


void bonesMotion::AddBone(CBoneInstance *bone, u32 axis_used, float target_yaw_1, float r_speed_1, float target_yaw_2, float r_speed_2, float target_yaw_3, float r_speed_3, u32 time);
{
	MBone b;

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


void bonesManipulation::AddMotion(bonesMotion m)
{
	motions.push_back(m);
}

void bonesManipulation::SetreturnParams(bonesMotion m)
{
	return_params = m;
}

void bonesManipulation::Update()
{
	
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


