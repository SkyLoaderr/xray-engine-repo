#pragma once
#include "../../feel_touch.h"

class CPsyAura : public Feel::Touch {

	// �������� ����
	CObject		*object;

	// ������� ����
	struct {
		float	value;				// ������� �������� �������
		float	restore_vel;		// �������� �������������� (ltx-param)
		float	decline_vel;		// �������� ���������� ������� ��� �������� ���� (ltx-param)	
		float	critical_value;		// ����������� �������� ������� ����, ������ �������� ���� ����� ��������� (ltx-param)
		float	activate_value;		// �������� ������� ����, ������ �������� �������� ���� ����� ���� ������������ (ltx-param)
		u32		time_last_update;	// ����� ���������� ���������� �������
	} m_power;
	
	// ������ ����
	float		m_radius;
	
	// ���������� ����
	bool		m_active;

	// �������������� ��������� / ���������� ����
	bool		m_auto_activate;

public:
					CPsyAura				();
	virtual			~CPsyAura				();
	
			void	init_external			(CObject *obj) {object = obj;}
	virtual	void	reinit					();
	virtual	void	reload					(LPCSTR section);
	
	virtual BOOL	feel_touch_contact		(CObject* O){return FALSE;}
			
	virtual	void	frame_update			(){}
	virtual	void	schedule_update			();

	virtual	void	on_activate				() {};
	virtual	void	on_deactivate			() {};

	virtual void	process_objects_in_aura	() {}

	// ���������� ����
			void	activate				();
			void	deactivate				();
			bool	is_active				(){return m_active;}
			bool	can_activate			(){return (m_power.value > m_power.activate_value);}
			void	set_auto_activate		(bool b_auto = true)  {m_auto_activate = b_auto;}

	// �������� ����
			void	set_radius				(float R) {m_radius = R;}
			float	get_radius				(){return m_radius;}

			CObject	*get_object				(){return object;}

private:
			void	update_power			();
};



