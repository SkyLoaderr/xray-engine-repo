#pragma once

class CAI_Bloodsucker;
class CAlienEffector;
class CAlienEffectorPP;
class CCameraBase;

class CBloodsuckerAlien {
	
	CCameraBase					*m_camera;
	CAI_Bloodsucker				*m_object;
	
	bool						m_active;
	CObject						*m_saved_current_entity;

	CAlienEffector				*m_effector;
	CAlienEffectorPP			*m_effector_pp;
	
	Fmatrix						m_prev_eye_matrix;

	float						m_inertion;

	float						m_current_fov;
	float						m_target_fov;

public:
			CBloodsuckerAlien	();
			~CBloodsuckerAlien	();
	
	void	init_external		(CAI_Bloodsucker *obj);	
	void	reinit				();

	void	update_frame		(); 
	void	update_scheduled	();

	void	activate			();
	void	deactivate			();
	
	void	update_camera		();

private:
	void	smooth_eye_matrix	();
};