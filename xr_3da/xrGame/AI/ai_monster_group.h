#pragma once

class CEntity;

enum ESquadCommand {
	SC_EXPLORE,
	SC_ATTACK,
	SC_THREATEN,
	SC_COVER,
	SC_FOLLOW,
	SC_FEEL_DANGER
};

enum ESquadTaskState {
	TS_REQUEST,		// запрос на выполнение задачи
	TS_PROGRESS,	// задача в процессе выполнения
	TS_REFUSED,		// в выполнении задачи - отказано
};

struct GTask {
	struct {
		ESquadCommand		command;
		ESquadTaskState		type;
		u32					ttl;	// время, до которого состояние считается активным
	} state;

	struct {
		Fvector			pos;
		CObject			*entity;
		float			radius;
	} target;
};


class CMonsterSquad {
	DEFINE_MAP(CEntity*, GTask, SQUAD_MAP,	SQUAD_MAP_IT);
	DEFINE_VECTOR(CEntity*, ENTITY_VEC, ENTITY_VEC_IT);
	
	CEntity		*leader;
	SQUAD_MAP	squad;
	
	u8			id;
public:
				CMonsterSquad(u8 i) : id(i), leader(0) {}
				~CMonsterSquad() {}
	
	void		RegisterMember		(CEntity *pE);
	void		RemoveMember		(CEntity *pE);

	void		SetLeader			(CEntity *pE) {leader = pE;}
	CEntity		*GetLeader			() {return leader;}

	void		ProcessGroupIntel	();
	GTask		&GetTask			(CEntity *pE);

	u8			GetID				() {return id;}

	void		Dump				();

private:
	void		InitTask			(GTask *task);

	void		CommonAttack		(ENTITY_VEC &enemies, ENTITY_VEC &members);
	CEntity		*GetNearestEnemy	(CEntity *t, ENTITY_VEC *ev);
	
	
};


class CSquadManager {
	DEFINE_MAP(u8, CMonsterSquad*, SQUADS_MAP, SQUADS_MAP_IT);
	SQUADS_MAP squads;

public:
					CSquadManager	();
					~CSquadManager	();
		
	void			RegisterMember	(u8 squad_id, CEntity *e);
	void			RemoveMember	(u8 squad_id, CEntity *e);

	CMonsterSquad	*GetSquad		(u8 squad_id);

	void			Dump			();
};


