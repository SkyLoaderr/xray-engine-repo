#pragma once

class CEntityAlive;

enum EGroupCommand {
	GC_EXPLORE,
	GC_ATTACK,
	GC_THREATEN,
	GC_COVER
};

struct GTask {
	Fvector			pos;
	CObject			*target_entity;
	EGroupCommand	command;
	bool			active;

					GTask	() {active = false;}
};

class CMonsterGroup {
	typedef CEntityAlive TEntity;
	DEFINE_MAP(TEntity*, GTask, GROUP_MAP,	GROUP_MAP_IT);
	
	TEntity		*leader;
	GROUP_MAP	group;
public:
			CMonsterGroup		();
			~CMonsterGroup		();

	void	AddMember			(TEntity *pE);
	void	DeleteMember		(TEntity *pE);
	
private:
	void	CommonEncircle			() {}
	void	CommonExplore			() {} 
};


