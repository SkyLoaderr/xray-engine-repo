#pragma once

class CActor;

class CControlledActor {
	CActor *m_actor;
	
	float	m_speed_yaw;
	float	m_speed_pitch;
	
	bool	m_active_turn;

	float	target_yaw, target_pitch;
	
	u8		yaw_dir;		// 0 - right dir,	1 - left dir,	2 - none
	u8		pitch_dir;		// 0 - up dir,		1 - bottom dir, 2 - none


public:
	void	reinit				();

	// disable any input
	void	take_control		();
	// restore input
	void	free_from_control	();

	void	look_point			(float speed, const Fvector &point);
	bool	is_turning			() {return m_active_turn;}
	void	update_look_point	(const Fvector &point);

	void	frame_update		();

	bool	is_controlled		();

private:
	void	reset				();
	
	void	update_turn			();
	
};	

