#pragma once

enum ETelekineticState {
	TS_None,
	TS_Raise,
	TS_Keep,
};

class CTelekineticObject {

	ETelekineticState	state;
	CGameObject			*object;
	
	float				target_height;

	u32					time_keep_started;
	u32					time_keep_updated;

	u32					time_to_keep;


public:
						CTelekineticObject		();
						~CTelekineticObject		();
	
	bool				init					(CGameObject *obj, float h, u32 ttk); 
	
	void				raise					(float power);
	
	void				prepare_keep			();
	void				keep					();
	void				release					();
	void				fire					(const Fvector &target);

	ETelekineticState	get_state				() {return state;}
	CGameObject			*get_object				() {return object;}

	bool				check_height			();
		
	bool				time_keep_elapsed		();

	void				enable					();
	
};
