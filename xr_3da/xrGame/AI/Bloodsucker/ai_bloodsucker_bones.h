#pragma once

#define AXIS_X	(1 << 0)
#define AXIS_Y	(1 << 1)
#define AXIS_Z	(1 << 2)

// параметры движения характерные для конкретной оси в боне
struct bonesAxis {
	float			cur_yaw;
	float			target_yaw;
	float			r_speed;
};

// бона с параметрами движения по осям
struct bonesBone {
	CBoneInstance	*bone;
	bonesAxis		axis[3];
	u32				axis_used;
	
			bonesBone	() {bone = 0; axis_used = 0;}
	bool	NeedTurn	(u32 p_axis);					// необходим поворот по оси p_axis
	void	Turn		(u32 dt, u32 p_axis);			// выполнить поворот по оси p_axis
};

// массив бон для движения нескольких бон дновременно + параметры движения
struct bonesMotion {
	xr_vector<bonesBone>	bones;				// элемент движения, вращается сразу несколько костей
	u32						time;				// время нахождения в таргете для бон

	void AddBone	(CBoneInstance *bone, u32 axis_used, float target_yaw_1, float r_speed_1, 
					 float target_yaw_2, float r_speed_2, float target_yaw_3, float r_speed_3, u32 time);
};

// управление движениями костей
class bonesManipulation {
	xr_vector<bonesMotion>				motions; 
	xr_vector<bonesMotion>::iterator	cur_motion;

	bool growth;						// если идёт прирост по углам
public:
	void Update				();
	void AddMotion			(bonesMotion m);
	void SetReturnParams	(bonesMotion m);
	void Stop				();
};

