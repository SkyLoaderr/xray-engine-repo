#pragma once
#include "../state_manager_second.h"

class CPoltergeist;

class CStateManagerPoltergeist : public CStateManagerSecond<CPoltergeist> {
	typedef CStateManagerSecond<CPoltergeist> inherited;


public:
						CStateManagerPoltergeist		(CPoltergeist *obj);
	virtual				~CStateManagerPoltergeist	();

	virtual void		reinit						();
	virtual	void		execute						();

private:

			u32			time_next_flame_attack;
			u32			time_next_tele_attack;
			u32			time_next_scare_attack;

			void		polter_attack				();
			


};
