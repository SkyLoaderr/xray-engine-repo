#pragma once

enum ETelekineticState {
	TS_None,
	TS_Raise,
	TS_Keep,
	TS_Fire,
};

class CTelekineticObject {

public:
	ETelekineticState	state;
	CGameObject			*object;
	
	float				target_height;

	u32					time_keep_started;
	u32					time_keep_updated;

	u32					time_to_keep;
	
	u32					time_fire_started;

	float				strength;
	
public:
						CTelekineticObject		();
						~CTelekineticObject		();
	
	bool				init					(CGameObject *obj, float s, float h, u32 ttk); 
	
	void				raise					(float power);
	
	void				prepare_keep			();
	void				keep					();
	void				release					();
	void				fire					(const Fvector &target);
	void				fire					(const Fvector &target, float power);


	ETelekineticState	get_state				() {return state;}
	CGameObject			*get_object				() {return object;}

	bool				check_height			();
		
	bool				time_keep_elapsed		();
	bool				time_fire_elapsed		();

	void				enable					();

	bool				operator==				(const CGameObject *obj) {
		return (object == obj);
	}

	

};
