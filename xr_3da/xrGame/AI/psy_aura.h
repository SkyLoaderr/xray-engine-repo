#pragma once
#include "../../feel_touch.h"

class CGameObject;

class CPsyAura : public Feel::Touch {

	// владелец поля
	CGameObject	*object;

	// энергия поля
	struct {
		float	value;				// текущее значение энергии
		float	restore_vel;		// скорость восстановления (ltx-param)
		float	decline_vel;		// скорость уменьшения энергии при активном поле (ltx-param)	
		float	critical_value;		// критическое значение энергии поля, меньше которого поле будет отключено (ltx-param)
		float	activate_value;		// значение энергии поля, больше значения которого поле может быть активировано (ltx-param)
		u32		time_last_update;	// время последнего обновления энергии
	} m_power;
	
	// радиус поля
	float		m_radius;
	
	// активность поля
	bool		m_active;

	// автоматическое включение / выключение поля
	bool		m_auto_activate;

public:
					CPsyAura				();
	virtual			~CPsyAura				();
	
			void	init_external			(CGameObject *obj) {object = obj;}
	virtual	void	reinit					();
	virtual	void	reload					(LPCSTR section);
	
	virtual BOOL	feel_touch_contact		(CObject* O){return FALSE;}
			
	virtual	void	frame_update			(){}
	virtual	void	schedule_update			();

	virtual	void	on_activate				() {};
	virtual	void	on_deactivate			() {};

	virtual void	process_objects_in_aura	() {}

	// активность поля
			void	activate				();
			void	deactivate				();
			bool	is_active				(){return m_active;}
			bool	can_activate			(){return (m_power.value > m_power.activate_value);}
			void	set_auto_activate		(bool b_auto = true)  {m_auto_activate = b_auto;}

	// свойства поля
			void	set_radius				(float R) {m_radius = R;}
			float	get_radius				(){return m_radius;}

		CGameObject	*get_object				(){return object;}

private:
			void	update_power			();
};



