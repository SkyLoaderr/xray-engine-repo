#pragma once

#define AXIS_X	(1 << 0)
#define AXIS_Y	(1 << 1)
#define AXIS_Z	(1 << 2)

// ��������� �������� ����������� ��� ���������� ��� � ����
struct bonesAxis {
	float			cur_yaw;
	float			target_yaw;
	float			r_speed;
};

// ���� � ����������� �������� �� ����
struct bonesBone {
	CBoneInstance	*bone;
	bonesAxis		axis[3];
	u32				axis_used;
	
			bonesBone	() {bone = 0; axis_used = 0;}
	bool	NeedTurn	(u32 p_axis);					// ��������� ������� �� ��� p_axis
	void	Turn		(u32 dt, u32 p_axis);			// ��������� ������� �� ��� p_axis
	void	Apply		();								// ���������� ���� � ����
};

// ������ ��� ��� �������� ���������� ��� ����������� + ��������� ��������
struct bonesMotion {
	xr_vector<bonesBone>	bones;				// ������� ��������, ��������� ����� ��������� ������
	u32						time;				// ����� ���������� � ������� ��� ���

	void AddBone	(CBoneInstance *bone, u32 axis_used, float target_yaw_1, float r_speed_1, 
					 float target_yaw_2, float r_speed_2, float target_yaw_3, float r_speed_3, u32 time);
	void Clear() { bones.clear(); time = 0;}
};

// ���������� ���������� ������
class bonesManipulation {
	xr_vector<bonesMotion>				motions; 
	xr_vector<bonesMotion>::iterator	cur_motion;

	bool	growth;						// ���� ��� ������� �� �����
	u32		time_started;
	u32		time_last_update;
public:
	void Init				();
	
	void Update				(u32 time);
	void AddMotion			(bonesMotion m);
	//void Stop				();

	bool isActive()			{return (!motions.empty());}
};


