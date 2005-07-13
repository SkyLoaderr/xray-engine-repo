#pragma once

class CActor;

class CControlledActor {
	CActor *m_actor;
	
	Fvector m_target_point;
	
	bool	m_turned_yaw;
	bool	m_turned_pitch;

public:
	void	reinit				();

	// disable any input
	void	take_control		();
	// restore input
	void	free_from_control	();

	void	look_point			(const Fvector &point);
	bool	is_turning			();

	void	frame_update		();
	bool	is_controlled		();

private:
	void	reset				();
	
	void	update_turn			();
	
};	

