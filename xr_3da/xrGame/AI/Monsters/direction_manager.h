#pragma once

class CBaseMonster;

class CDirectionManager {
	CBaseMonster	*m_object;


	u32				m_time_last_faced;
	u32				m_delay;

public:
		void	init_external		(CBaseMonster *obj) {m_object = obj;}
		void	reinit				();

		void	face_target			(const Fvector &position,	u32 delay = 0);
		void	face_target			(const CObject *obj,		u32 delay = 0);
	IC	void	set_delay			(u32 delay) {m_delay = delay;}

		bool	is_face_target		(const Fvector &position,	float eps_angle);
		bool	is_face_target		(const CObject *obj,		float eps_angle);
		
		void	use_path_direction	(bool reversed = false);

		void	force_direction		(const Fvector &dir);
};
