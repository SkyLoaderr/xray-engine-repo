#pragma once

class CEntity;

// “ипы команд
enum ESquadCommand {
	SC_EXPLORE,
	SC_ATTACK,
	SC_THREATEN,
	SC_COVER,
	SC_FOLLOW,
	SC_FEEL_DANGER,
	SC_NONE
};

// “ипы состо€ний задачи
enum ESquadTaskState {
	TS_REQUEST,		// запрос на выполнение задачи
	TS_PROGRESS,	// задача в процессе выполнени€
	TS_REFUSED,		// в выполнении задачи - отказано
};

// —осто€ние лидера
enum ELeaderState {
	LS_IDLE,
	LS_EAT,
	LS_WANDER,
	LS_ATTACK, 
	LS_DEFENCE
};

struct SEntityState {
	CEntity *pEnemy;		// текущий враг. обновл€етс€ со стороны NPC
	Fvector target;			// результирующа€ позици€ обновл€етс€ со стороны Squad (или вектор целевого направлени€)
	u32		last_updated;	// врем€ последнего обновлени€ pEnemy со стороны NPC
	u32		last_repos;		// врем€ последнего обновлени€ target со стороны Squad (если 0 - то не брать target)
};

struct SMemberEnemy {
	CEntity *member;
	CEntity *enemy;
};

// «адача
struct GTask {
	struct {
		ESquadCommand		command;
		ESquadTaskState		type;
		u32					ttl;			// врем€, до которого состо€ние считаетс€ активным
		bool				need_reset;		// если type = TS_REQUEST, и need_reset=true - клиент должен очистить предыдущее состо€ние
	} state;

	struct {
		Fvector				pos;
		CObject				*entity;
		u32					node;
	} target;
};

/////////////////////////////////////////////////////////////////////////////////////////
// MonsterSquad Class
class CMonsterSquad {
	DEFINE_MAP		(CEntity*, GTask,	SQUAD_MAP,	SQUAD_MAP_IT);
	DEFINE_VECTOR	(CEntity*, ENTITY_VEC,	ENTITY_VEC_IT);
	DEFINE_MAP		(CEntity*, SEntityState, ENTITY_STATE_MAP, ENTITY_STATE_MAP_IT);

	DEFINE_VECTOR	(SMemberEnemy, MEMBER_ENEMY_VEC, MEMBER_ENEMY_VEC_IT);

	CEntity				*leader;
	ELeaderState		leader_state;
	
	SQUAD_MAP			squad;
	
	u8					id;

	ENTITY_STATE_MAP	states;

	MEMBER_ENEMY_VEC	vect_copy;
	MEMBER_ENEMY_VEC	general_enemy;		


	struct _elem {
		CEntity *pE;
		Fvector p_from;
		float	yaw;
	};

	xr_vector<_elem>	lines;

public:
				CMonsterSquad(u8 i) : id(i), leader(0) {}
				~CMonsterSquad() {}
	
	// -----------------------------------------------------------------

	void		RegisterMember		(CEntity *pE);
	void		RemoveMember		(CEntity *pE);

	// -----------------------------------------------------------------

	void		SetLeader			(CEntity *pE) {leader = pE;}
	CEntity		*GetLeader			() {return leader;}
	void		SetLeaderState		(ELeaderState ls) {leader_state = ls;}

	// -----------------------------------------------------------------

	// ÷ентрализованное прин€тие решений
	void		ProcessGroupIntel	();
	void		ProcessGroupIntel	(const GTask &task);

	// -----------------------------------------------------------------
	
	// ƒецентрализованное управление
	void		UpdateMonsterData	(CEntity *pE, CEntity *pEnemy);
	void		UpdateDecentralized	();
	Fvector		GetTargetPoint		(CEntity *pE, u32 &time);


	// ѕроверить, стоит активность группы
	bool			SquadActive		();

	// -----------------------------------------------------------------

	// ѕолучить задачу дл€ NPC
	GTask		&GetTask			(CEntity *pE);

	// ѕолучить id своего squad
	u8			GetID				() {return id;}

	// DEBUG
	void		Dump				();

private:
	ENTITY_VEC	enemies;
	ENTITY_VEC	members;
	u32			dest_node;

	void		AskMember			(CEntity *pE, const GTask &new_task);

private:
	void		TaskIdle			();
	void		Explore				(xr_vector<u32> &nodes, const Fvector &centroid, const Fvector &dir, CEntity *pE, GTask *pTask);
	bool		IsPriorityHigher	(ESquadCommand com_new, ESquadCommand com_old);
	
	void		InitTask			(GTask *task);
	bool		ActiveTask			(GTask *task);


	CEntity		*GetNearestEnemy	(CEntity *t, ENTITY_VEC *ev);
	
	// decentralized
	void		SetupMemeberPositions_Deviation (MEMBER_ENEMY_VEC &members, CEntity *enemy);
	void		SetupMemeberPositions_TargetDir (MEMBER_ENEMY_VEC &members, CEntity *enemy);

};


class CSquadManager {
	DEFINE_MAP(u8, CMonsterSquad*, SQUADS_MAP, SQUADS_MAP_IT);
	SQUADS_MAP squads;

public:
					CSquadManager	();
					~CSquadManager	();
		
	void			RegisterMember	(u8 squad_id, CEntity *e);
	void			RemoveMember	(u8 squad_id, CEntity *e);

	//-------------------------------------------------------------------

	CMonsterSquad	*GetSquad		(u8 squad_id);
	
	//-------------------------------------------------------------------
	// Utilities
	
	u8				TransformPriority(ESquadCommand com);
	
	//-------------------------------------------------------------------

	void			Dump			();
};


