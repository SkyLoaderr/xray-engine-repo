#pragma once

class CPsyAura : public Feel::Touch {

	// �������� ����
	CObject		*object;
	
	struct {
		float	value;				// ������� �������� �������
		float	restore_vel;		// �������� �������������� (ltx-param)
		float	critical_value;		// ����������� �������� ������� ����, ������ �������� ���� ����� ���������(ltx-param)
	} m_power;
	
	// ������� ����
	float		m_power;
	
	// ���������� ����
	bool		m_active;

public:
					CPsyAura			();
	virtual			~CPsyAura			();
	
			void	reload				(LPCSTR section);
			void	init_external		(CObject *obj);

	virtual void	feel_touch_new		(CObject* O){};
	virtual void	feel_touch_delete	(CObject* O){};
	virtual BOOL	feel_touch_contact	(CObject* O){return FALSE;}

			void	frame_update		(){}
			void	schedule_update		(){}

			void	update_power		(){}

			void	activate			(){m_active = true;}
			void	deactivate			(){m_active = false;}
			bool	is_active			(){return m_active;}

	virtual	void	on_activate			() {};
	virtual	void	on_deactivate		() {};
};



//// ����, ��������� �� ��������� ��� ����� � ����
//ref_sound	m_sound;
//
//// �������� � ������ ��� ����� � ����
//CEffector	m_effector;

