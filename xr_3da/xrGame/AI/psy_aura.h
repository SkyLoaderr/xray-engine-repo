#pragma once

class CPsyAura : public Feel::Touch {

	// владелец поля
	CObject		*object;
	
	struct {
		float	value;				// текущее значение энергии
		float	restore_vel;		// скорость восстановления (ltx-param)
		float	critical_value;		// критическое значение энергии поля, меньше которого поле будет отключено(ltx-param)
	} m_power;
	
	// энергия поля
	float		m_power;
	
	// активность поля
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



//// звук, исходящий от владельца при входе в поле
//ref_sound	m_sound;
//
//// эффектор у актера при входе в поле
//CEffector	m_effector;

